Mini OS Simulator

A C++ based operating system simulator with a modern, web-based graphical user interface (GUI). This project simulates core OS concepts, including process scheduling and memory management, allowing for interactive visualization and comparison of different algorithms.

(Note: You can replace this placeholder URL with a screenshot from your own project)

✨ Features

Interactive Web-Based GUI: A clean and responsive frontend built with HTML, CSS, and JavaScript.

Dynamic Process Creation: Add processes with custom arrival times, burst times, memory requirements, and priorities.

CPU Scheduling Algorithms:

First-Come, First-Served (FCFS)

Shortest Job First (SJF) (Non-Preemptive)

Round Robin (RR)

Preemptive Priority

Memory Management:

Block-based memory simulation.

Supports First-Fit, Best-Fit, and Worst-Fit allocation algorithms.

Visual Results:

Dynamically generated Gantt chart for process execution.

A results table with detailed metrics per process.

Calculates and displays Average Waiting Time and Average Turnaround Time.

🚀 Tech Stack

Backend: C++17

Web Server/API: httplib.h (A single-file C++ web server library)

JSON Handling: nlohmann/json.hpp (A single-file C++ JSON library)

Frontend: HTML5, CSS3, JavaScript

Build System: Makefile (compatible with mingw32-make on Windows and make on Linux/macOS)

🛠️ How to Run

This repository is self-contained. All C++ libraries (httplib.h, json.hpp) are included.

Prerequisites

Git: To clone the repository.

A C++17 Compiler:

Windows: g++ via MSYS2 / MinGW-w64 (64-bit) is required.

Linux: g++

macOS: clang++ (via Xcode Command Line Tools)

1. Clone the Repository

git clone [https://github.com/diyaRawat0/MiniOS.git](https://github.com/diyaRawat0/MiniOS.git)
cd MiniOS


2. Build the Backend

Run the make command from the root of the project folder.

On Windows (PowerShell):

mingw32-make


On Linux / macOS (Terminal):

make


This will compile all .cpp files and create a single executable named minios (or minios.exe).

3. Run the Server

After a successful build, run the executable.

On Windows (PowerShell):

.\minios


On Linux / macOS (Terminal):

./minios


Your terminal should now say:
Starting Mini OS Server at http://localhost:8080

4. Access the GUI

Open your web browser (Chrome, Firefox, etc.) and navigate to:

http://localhost:8080/index.html

You can now add processes, select your simulation settings, and run the simulations!

📁 Project Structure

.
├── MemoryManager.cpp       # Logic for First-Fit, Best-Fit, Worst-Fit
├── MemoryManager.h
├── Process.cpp             # Process data structure
├── Process.h
├── Scheduler.cpp           # Logic for FCFS, SJF, RR, Priority
├── Scheduler.h
├── main.cpp                # The C++ web server and all API endpoints
│
├── index.html              # The complete HTML/CSS/JS frontend
│
├── httplib.h               # C++ Web server library
├── json.hpp                # C++ JSON library
│
├── Makefile                # The build script
├── .gitignore              # Ignores compiled files
└── README.md               # You are here


👥 Contributors

Diya Rawat

Avdhesh Sayana

Mohit Rautela

Animesh Tripathi
