#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "user/user.h"

static char* states[] = {
    "UNUSED", "USED", "SLEEPING", "RUNNABLE", "RUNNING", "ZOMBIE"
};

int
main(int argc, char** argv)
{
    int lim = 10;
    struct procinfo* procs = malloc(lim * sizeof(struct procinfo));
    int total = 0;

    while(1) {
        total = ps_listinfo(procs, lim);

        if (total < 0) {
            printf("ps: ps_listinfo failed\n");
            free(procs);
            exit(1);
        }

        if (total > lim) {
            lim = total;
            free(procs);
            procs = malloc(lim * sizeof(struct procinfo));
        } else {
            break;
        }
    }

    printf("pid\tppid\tstate\t\tname\n");

    for (int i = 0; i < total; i++) {
        printf(
            "%d\t%d\t%s\t\t%s\n",
            procs[i].pid,
            procs[i].parent_pid,
            states[procs[i].state],
            procs[i].name
        );
    }

    free(procs);
    exit(0);
}
