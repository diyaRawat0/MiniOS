#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include "Process.h"
#include "MemoryManager.h"

// Represents one bar in the Gantt chart
struct GanttChartEntry {
    int pid;
    int startTime;
    int endTime;
    std::string color;
};

// Implements CPU scheduling algorithms
class Scheduler {
private:
    // Helper to reset process stats before a simulation
    void resetProcesses(std::vector<Process>& processes);

public:
    std::vector<GanttChartEntry> runFCFS(std::vector<Process>& processes, MemoryManager& mm, const std::string& memAlgo);
    std::vector<GanttChartEntry> runSJF(std::vector<Process>& processes, MemoryManager& mm, const std::string& memAlgo);
    
    // --- NEW ---
    std::vector<GanttChartEntry> runRR(std::vector<Process>& processes, MemoryManager& mm, const std::string& memAlgo, int quantum);
    std::vector<GanttChartEntry> runPriority(std::vector<Process>& processes, MemoryManager& mm, const std::string& memAlgo);
};

#endif // SCHEDULER_H