#include "MemoryManager.h"
#include <iostream>
#include <algorithm> // for std::sort

MemoryManager::MemoryManager(int total) : totalMemory(total), nextBlockId(1) {
    reset();
}

void MemoryManager::reset() {
    blocks.clear();
    // Start with one single large free block
    blocks.emplace_back(0, 0, totalMemory, true, -1);
    nextBlockId = 1;
}

// Main allocation function, delegates to the chosen algorithm
bool MemoryManager::allocate(const Process& p, const std::string& algorithm) {
    if (algorithm == "bestfit") {
        return allocateBestFit(p);
    } else if (algorithm == "worstfit") {
        return allocateWorstFit(p);
    }
    // Default to First-Fit
    return allocateFirstFit(p);
}

// Deallocates memory by finding the block used by the process and marking it free
// This simple version doesn't merge free blocks, but a real one would.
void MemoryManager::deallocate(int processId) {
    for (auto& block : blocks) {
        if (block.processId == processId) {
            block.isFree = true;
            block.processId = -1;
            // TODO: Add logic to merge adjacent free blocks
            return;
        }
    }
}

// --- Allocation Algorithm Implementations ---

bool MemoryManager::allocateFirstFit(const Process& p) {
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i].isFree && blocks[i].size >= p.memoryRequirement) {
            // Found a block
            int originalSize = blocks[i].size;
            int start = blocks[i].startAddress;
            
            // Allocate by splitting the block
            blocks[i].size = p.memoryRequirement;
            blocks[i].isFree = false;
            blocks[i].processId = p.pid;
            
            // If there's leftover space, create a new free block
            if (originalSize > p.memoryRequirement) {
                int newSize = originalSize - p.memoryRequirement;
                int newStart = start + p.memoryRequirement;
                blocks.insert(blocks.begin() + i + 1, MemoryBlock(nextBlockId++, newStart, newSize, true, -1));
            }
            return true;
        }
    }
    return false; // No suitable block found
}

bool MemoryManager::allocateBestFit(const Process& p) {
    int bestFitIndex = -1;
    int minSize = totalMemory + 1; // Start with a size larger than possible

    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i].isFree && blocks[i].size >= p.memoryRequirement) {
            if (blocks[i].size < minSize) {
                minSize = blocks[i].size;
                bestFitIndex = i;
            }
        }
    }

    if (bestFitIndex != -1) {
        // Found the best block, now allocate it (same logic as First-Fit)
        int originalSize = blocks[bestFitIndex].size;
        int start = blocks[bestFitIndex].startAddress;
        
        blocks[bestFitIndex].size = p.memoryRequirement;
        blocks[bestFitIndex].isFree = false;
        blocks[bestFitIndex].processId = p.pid;
        
        if (originalSize > p.memoryRequirement) {
            int newSize = originalSize - p.memoryRequirement;
            int newStart = start + p.memoryRequirement;
            blocks.insert(blocks.begin() + bestFitIndex + 1, MemoryBlock(nextBlockId++, newStart, newSize, true, -1));
        }
        return true;
    }
    return false; // No suitable block found
}

bool MemoryManager::allocateWorstFit(const Process& p) {
    int worstFitIndex = -1;
    int maxSize = -1; // Start with a size smaller than possible

    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i].isFree && blocks[i].size >= p.memoryRequirement) {
            if (blocks[i].size > maxSize) {
                maxSize = blocks[i].size;
                worstFitIndex = i;
            }
        }
    }

    if (worstFitIndex != -1) {
        // Found the worst block, now allocate it (same logic as First-Fit)
        int originalSize = blocks[worstFitIndex].size;
        int start = blocks[worstFitIndex].startAddress;
        
        blocks[worstFitIndex].size = p.memoryRequirement;
        blocks[worstFitIndex].isFree = false;
        blocks[worstFitIndex].processId = p.pid;
        
        if (originalSize > p.memoryRequirement) {
            int newSize = originalSize - p.memoryRequirement;
            int newStart = start + p.memoryRequirement;
            blocks.insert(blocks.begin() + worstFitIndex + 1, MemoryBlock(nextBlockId++, newStart, newSize, true, -1));
        }
        return true;
    }
    return false; // No suitable block found
}


// --- Getters for UI ---

int MemoryManager::getTotalMemory() const {
    return totalMemory;
}

int MemoryManager::getUsedMemory() const {
    int used = 0;
    for (const auto& block : blocks) {
        if (!block.isFree) {
            used += block.size;
        }
    }
    return used;
}