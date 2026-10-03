#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "user/user.h"

static char *states[] = {
    "UNUSED", "USED", "SLEEPING", "RUNNABLE", "RUNNING", "ZOMBIE"
};

int
main(int argc, char** argv)
{
    struct procinfo procs[64] = {0};
    int total = ps_listinfo(procs, 64);

    if (total < 0) {
        printf("ps: ps_listinfo failed\n");
        exit(1);
    }

    int copied = (total > 64) ? 64 : total;

    printf("pid\tppid\tstate\t\tname\n");

    for (int i = 0; i < copied; i++) {
        printf(
            "%d\t%d\t%s\t\t%s\n",
            procs[i].pid,
            procs[i].parent_pid,
            states[procs[i].state],
            procs[i].name
        );
    }

    exit(0);
}
