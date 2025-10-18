# Define the C++ compiler to use
CXX = g++

# Define compiler flags: -std=c++17 for modern C++, -Wall
# -D_WIN32_WINNT=0x0A00 is for Windows 10+ APIs
CXXFLAGS = -std=c++17 -Wall -D_WIN32_WINNT=0x0A00

# The name of the final executable
TARGET = minios

# List all C++ source files
SOURCES = main.cpp Process.cpp MemoryManager.cpp Scheduler.cpp

# Generate object file names from source file names
OBJECTS = $(SOURCES:.cpp=.o)

# Add networking library based on OS
# For Windows (PowerShell/MinGW)
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32
# For Linux/macOS
else
    LDFLAGS = -lpthread
endif

# The default rule to build the target executable
all: $(TARGET)

# Rule to link all object files into the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Rule to compile a .cpp source file into a .o object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up build files
clean:
	-del $(OBJECTS) $(TARGET).exe 2>NUL || rm -f $(OBJECTS) $(TARGET)