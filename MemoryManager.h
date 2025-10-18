#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <string>
#include "Process.h"

// Represents a single block of memory
struct MemoryBlock {
    int id;
    int startAddress;
    int size;
    bool isFree;
    int processId; // Which process occupies this block (-1 if free)

    MemoryBlock(int i, int start, int s, bool free = true, int pId = -1)
        : id(i), startAddress(start), size(s), isFree(free), processId(pId) {}
};

class MemoryManager {
private:
    int totalMemory;
    std::vector<MemoryBlock> blocks;
    int nextBlockId;

    // Allocation algorithms
    bool allocateFirstFit(const Process& p);
    bool allocateBestFit(const Process& p);
    bool allocateWorstFit(const Process& p);

public:
    MemoryManager(int total);
    
    // Main allocation/deallocation functions
    bool allocate(const Process& p, const std::string& algorithm);
    void deallocate(int processId);
    void reset();
    
    // Getters for UI visualization
    int getTotalMemory() const;
    int getUsedMemory() const;
    // You could add a getBlocks() function to visualize fragmentation in the GUI
};

#endif // MEMORY_MANAGER_H