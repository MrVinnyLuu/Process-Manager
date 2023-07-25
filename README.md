# Process Manager

Project 1 for COMP30023_2023_SM1 Computer Systems.

"In this project, you will implement a process manager capable of allocating memory to processes and scheduling them for execution. This project has two phases. In the first phase, a scheduling algorithm will allocate the CPU to processes, making the assumption that memory requirements are always satisfied. In the second phase, a memory allocation algorithm will be used to allocate memory to processes, before the scheduling takes place. Both the memory allocation and the process scheduling are simulated. There is also a challenge task that requires controlling the execution of real processes during the process scheduling. Process scheduling decisions will be based on a scheduling algorithm with the assumption of a single CPU (i.e, only one process can be running at a time)."

## Running the Project

- Download the repository
- Run `make`
- Run `./allocate -f <filename> -s (SJF|RR) -m (infinite|best-fit) -q (1|2|3)`
