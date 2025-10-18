#include "Scheduler.h"
#include <iostream>
#include <vector>
#include <algorithm> // for std::sort
#include <queue> // for Round Robin

// Helper to sort by arrival time
bool compareArrival(Process& a, Process& b) {
    if (a.arrivalTime == b.arrivalTime)
        return a.pid < b.pid;
    return a.arrivalTime < b.arrivalTime;
}

// Helper to reset process stats
void Scheduler::resetProcesses(std::vector<Process>& processes) {
    for (auto& p : processes) {
        p.remainingBurstTime = p.burstTime;
        p.isFinished = false;
        p.startTime = -1;
        p.completionTime = -1;
        p.waitingTime = 0;
        p.turnaroundTime = 0;
    }
}

// --- FCFS Implementation ---
std::vector<GanttChartEntry> Scheduler::runFCFS(std::vector<Process>& processes, MemoryManager& mm, const std::string& memAlgo) {
    std::vector<GanttChartEntry> chart;
    mm.reset();
    resetProcesses(processes);
    
    std::sort(processes.begin(), processes.end(), compareArrival);
    
    int currentTime = 0;
    for (auto& p : processes) {
        if (currentTime < p.arrivalTime) {
            currentTime = p.arrivalTime;
        }

        if (mm.allocate(p, memAlgo)) {
            p.startTime = currentTime;
            p.waitingTime = p.startTime - p.arrivalTime;
            p.completionTime = p.startTime + p.burstTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.remainingBurstTime = 0;
            p.isFinished = true;
            
            chart.push_back({p.pid, p.startTime, p.completionTime, p.color});
            currentTime = p.completionTime;
            
            mm.deallocate(p.pid); // Free memory
        }
    }
    return chart;
}

// --- SJF (Non-Preemptive) Implementation ---
std::vector<GanttChartEntry> Scheduler::runSJF(std::vector<Process>& processes, MemoryManager& mm, const std::string& memAlgo) {
    std::vector<GanttChartEntry> chart;
    mm.reset();
    resetProcesses(processes);
    
    int n = processes.size();
    int currentTime = 0;
    int completedCount = 0;

    while (completedCount < n) {
        int shortestJobIndex = -1;
        int minBurst = 1e9;

        // Find the shortest job that has arrived and is not yet completed
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && !processes[i].isFinished) {
                if (processes[i].burstTime < minBurst) {
                    minBurst = processes[i].burstTime;
                    shortestJobIndex = i;
                }
            }
        }

        if (shortestJobIndex == -1) {
            currentTime++;
        } else {
            Process& p = processes[shortestJobIndex];
            if (mm.allocate(p, memAlgo)) {
                p.startTime = currentTime;
                p.waitingTime = p.startTime - p.arrivalTime;
                p.completionTime = p.startTime + p.burstTime;
                p.turnaroundTime = p.completionTime - p.arrivalTime;
                p.remainingBurstTime = 0;
                p.isFinished = true;

                chart.push_back({p.pid, p.startTime, p.completionTime, p.color});

                currentTime = p.completionTime;
                completedCount++;
                mm.deallocate(p.pid);
            } else {
                 p.isFinished = true; // Mark as "finished" to avoid infinite loop
                 completedCount++;
            }
        }
    }
    return chart;
}

// --- NEW: Round Robin (RR) Implementation ---
std::vector<GanttChartEntry> Scheduler::runRR(std::vector<Process>& processes, MemoryManager& mm, const std::string& memAlgo, int quantum) {
    std::vector<GanttChartEntry> chart;
    mm.reset();
    resetProcesses(processes);

    std::sort(processes.begin(), processes.end(), compareArrival);
    
    std::queue<int> readyQueue; // Stores indices of processes in the 'processes' vector
    int currentTime = 0;
    int completedCount = 0;
    int n = processes.size();
    int currentProcessIndex = 0; // To check for new arrivals

    while (completedCount < n) {
        // Add newly arrived processes to the ready queue
        while(currentProcessIndex < n && processes[currentProcessIndex].arrivalTime <= currentTime) {
            readyQueue.push(currentProcessIndex++);
        }

        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        int p_idx = readyQueue.front();
        readyQueue.pop();
        Process& p = processes[p_idx];

        // Try to allocate memory. If it fails, skip this process.
        // NOTE: This is a simple model. A real OS might keep it in queue.
        if (!mm.allocate(p, memAlgo)) {
            p.isFinished = true;
            completedCount++;
            continue;
        }

        if (p.startTime == -1) p.startTime = currentTime;
        
        int startTimeBlock = currentTime;
        int runTime = std::min(p.remainingBurstTime, quantum);
        
        p.remainingBurstTime -= runTime;
        currentTime += runTime;

        chart.push_back({p.pid, startTimeBlock, currentTime, p.color});

        // Add any processes that arrived *while* this one was running
        while(currentProcessIndex < n && processes[currentProcessIndex].arrivalTime <= currentTime) {
            readyQueue.push(currentProcessIndex++);
        }

        if (p.remainingBurstTime == 0) {
            p.isFinished = true;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
            completedCount++;
            mm.deallocate(p.pid); // Free memory
        } else {
            readyQueue.push(p_idx); // Put it back in the queue
            mm.deallocate(p.pid); // Free memory (so others can use it)
        }
    }
    return chart;
}

// --- NEW: Preemptive Priority Implementation ---
std::vector<GanttChartEntry> Scheduler::runPriority(std::vector<Process>& processes, MemoryManager& mm, const std::string& memAlgo) {
    std::vector<GanttChartEntry> chart;
    mm.reset();
    resetProcesses(processes);

    int n = processes.size();
    int currentTime = 0;
    int completedCount = 0;
    int lastPid = -1;

    while (completedCount < n) {
        int highestPriorityIndex = -1;
        int highestPriority = 1e9; // Lower number = higher priority

        // Find the highest priority job that has arrived
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && !processes[i].isFinished) {
                if (processes[i].priority < highestPriority) {
                    highestPriority = processes[i].priority;
                    highestPriorityIndex = i;
                }
            }
        }

        if (highestPriorityIndex == -1) {
            currentTime++;
            lastPid = -1; // CPU is idle
            continue;
        }

        Process& p = processes[highestPriorityIndex];
        
        // --- Memory Allocation ---
        // This is tricky in a preemptive sim. We'll deallocate the *previous* process
        // and allocate the *new* one.
        if (lastPid != p.pid) {
            if(lastPid != -1) mm.deallocate(lastPid);
            
            if (!mm.allocate(p, memAlgo)) {
                p.isFinished = true; // Cannot run, skip
                completedCount++;
                lastPid = -1; // No one is in memory
                continue;
            }
        }
        
        if (p.startTime == -1) p.startTime = currentTime;

        // Update chart
        if (chart.empty() || chart.back().pid != p.pid) {
            chart.push_back({p.pid, currentTime, currentTime + 1, p.color});
        } else {
            chart.back().endTime++;
        }

        p.remainingBurstTime--;
        currentTime++;
        lastPid = p.pid;

        if (p.remainingBurstTime == 0) {
            p.isFinished = true;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
            completedCount++;
            mm.deallocate(p.pid); // Free memory
            lastPid = -1;
        }
    }
    return chart;
}