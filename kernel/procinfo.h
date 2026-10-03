#ifndef XV6_PROCINFO_H
#define XV6_PROCINFO_H


enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct procinfo {
    int pid; // Process ID
    int parent_pid; // Parent process ID
    enum procstate state; // Process state
    char name[16]; // Process name
};

#endif
