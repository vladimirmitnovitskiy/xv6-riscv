enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct procinfo {
    int pid; // Process ID
    int parent_pid; // Parent process ID
    enum procstate state; // Process state
    char[16] name; // Process name
}
