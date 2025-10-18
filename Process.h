#ifndef PROCESS_H
#define PROCESS_H

#include <string>

// Defines the structure for a process with all its necessary attributes.
class Process {
public:
    int pid;
    int arrivalTime;
    int burstTime;
    int memoryRequirement;
    int priority; // <-- NEW
    
    int remainingBurstTime; // <-- NEW
    bool isFinished;

    // Results from simulation
    int startTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    
    // For UI
    std::string color; 

    // Constructor to initialize a new process object
    Process(int id, int arrival, int burst, int memory, int prio);
};

#endif // PROCESS_H