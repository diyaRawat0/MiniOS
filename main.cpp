#include <iostream>
#include <vector>
#include <string>
#include "httplib.h"
#include "json.hpp"
#include "Process.h"
#include "MemoryManager.h"
#include "Scheduler.h"

// Use nlohmann::json for convenience
using json = nlohmann::json;

// --- Global data for our application ---
std::vector<Process> process_queue;
MemoryManager memoryManager(1024); // 1GB default memory
Scheduler scheduler;

// --- JSON conversion functions ---
// Convert a Process object to JSON
void to_json(json& j, const Process& p) {
    j = json{
        {"pid", p.pid},
        {"arrival", p.arrivalTime},
        {"burst", p.burstTime},
        {"memory", p.memoryRequirement},
        {"priority", p.priority}, // <-- NEW
        {"start", p.startTime},
        {"completion", p.completionTime},
        {"wait", p.waitingTime},
        {"turnaround", p.turnaroundTime},
        {"color", p.color}
    };
}

// Convert a GanttChartEntry to JSON
void to_json(json& j, const GanttChartEntry& e) {
    j = json{
        {"pid", e.pid},
        {"start", e.startTime},
        {"end", e.endTime},
        {"color", e.color}
    };
}

// --- API Functions ---
json get_process_list() {
    return json(process_queue);
}

json add_process(const httplib::Request& req) {
    try {
        json j = json::parse(req.body);
        int arrival = j.at("arrival");
        int burst = j.at("burst");
        int memory = j.at("memory");
        int priority = j.at("priority"); // <-- NEW
        int new_pid = process_queue.size() + 1;
        
        process_queue.emplace_back(new_pid, arrival, burst, memory, priority);
        return {{"status", "success"}, {"pid", new_pid}};
    } catch (json::exception& e) {
        return {{"status", "error"}, {"message", e.what()}};
    }
}

json run_simulation(std::vector<Process>& processes, const std::string& memAlgo, std::function<std::vector<GanttChartEntry>()> run) {
    std::vector<GanttChartEntry> chart = run();
    
    // Calculate stats
    float totalWaitingTime = 0;
    float totalTurnaroundTime = 0;
    int successfulProcesses = 0;
    for (const auto& p : processes) {
        if (!p.isFinished || p.completionTime == -1) continue;
        
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
        successfulProcesses++;
    }
    
    json stats;
    if (successfulProcesses > 0) {
        stats["avgWaiting"] = totalWaitingTime / successfulProcesses;
        stats["avgTurnaround"] = totalTurnaroundTime / successfulProcesses;
    } else {
        stats["avgWaiting"] = 0;
        stats["avgTurnaround"] = 0;
    }
    
    return {
        {"chart", chart},
        {"stats", stats},
        {"processes", processes} // send back the updated processes with completion times
    };
}

// --- Main Server ---
int main() {
    httplib::Server svr;
    
    // Serve the index.html file from the current directory
    svr.set_base_dir(".");

    // API endpoint to get the current list of processes
    svr.Get("/api/get_processes", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(get_process_list().dump(), "application/json");
    });
    
    // API endpoint to add a new process
    svr.Post("/api/add_process", [](const httplib::Request& req, httplib::Response& res) {
        json result = add_process(req);
        res.set_content(result.dump(), "application/json");
    });

    // API endpoint to reset the queue
    svr.Post("/api/reset", [](const httplib::Request&, httplib::Response& res) {
        process_queue.clear();
        res.set_content(R"({"status":"success", "message":"Queue cleared"})", "application/json");
    });

    // --- NEW SIMULATION ENDPOINTS ---

    svr.Post("/api/run_simulation", [](const httplib::Request& req, httplib::Response& res) {
        std::vector<Process> processes_copy = process_queue;
        json result;

        try {
            json j = json::parse(req.body);
            std::string algo = j.at("algo");
            std::string memAlgo = j.at("memAlgo");

            if (algo == "fcfs") {
                result = run_simulation(processes_copy, memAlgo, [&]() {
                    return scheduler.runFCFS(processes_copy, memoryManager, memAlgo);
                });
            } else if (algo == "sjf") {
                result = run_simulation(processes_copy, memAlgo, [&]() {
                    return scheduler.runSJF(processes_copy, memoryManager, memAlgo);
                });
            } else if (algo == "rr") {
                int quantum = j.at("quantum");
                result = run_simulation(processes_copy, memAlgo, [&]() {
                    return scheduler.runRR(processes_copy, memoryManager, memAlgo, quantum);
                });
            } else if (algo == "priority") {
                result = run_simulation(processes_copy, memAlgo, [&]() {
                    return scheduler.runPriority(processes_copy, memoryManager, memAlgo);
                });
            } else {
                result = {{"status", "error"}, {"message", "Unknown algorithm"}};
            }

        } catch (json::exception& e) {
            result = {{"status", "error"}, {"message", e.what()}};
        }
        
        res.set_content(result.dump(), "application/json");
    });
    
    std::cout << "Starting Mini OS Server at http://localhost:8081" << std::endl;
    std::cout << "Open index.html in your browser to use the GUI." << std::endl;
    std::cout << "Press Ctrl+C to stop." << std::endl;
    
    svr.listen("localhost", 8081);
    return 0;
}