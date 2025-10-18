#include "Process.h"
#include <vector>

// Constructor implementation
Process::Process(int id, int arrival, int burst, int memory, int prio)
    : pid(id),
      arrivalTime(arrival),
      burstTime(burst),
      memoryRequirement(memory),
      priority(prio),
      remainingBurstTime(burst), // <-- NEW
      isFinished(false),
      startTime(-1),
      completionTime(-1),
      waitingTime(-0),
      turnaroundTime(0) {
        
    // Assign a unique color for the Gantt chart visualization
    static const std::vector<std::string> colors = {
        "\033[41m", "\033[42m", "\033[43m", "\033[44m", 
        "\033[45m", "\033[46m", "\033[101m", "\033[102m", 
        "\033[103m", "\033[104m", "\033[105m", "\033[106m"
    };
    color = colors[ (id - 1) % colors.size() ];
}