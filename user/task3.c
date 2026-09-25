#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char** argv)
{
  int pipefd[2] = {0};

  if (pipe(pipefd) < 0) {
    fprintf(2, "pipe failed\n");
    exit(1);
  }

  int pid = fork();

  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    close(pipefd[1]);
    close(0);

    if (dup(pipefd[0]) != 0) {
        fprintf(2, "dup failed\n");
        exit(1);
    }

    close(pipefd[0]);

    char* wcargv[] = {"/wc", 0};
    exec("/wc", wcargv);

    fprintf(2, "exec /wc failed\n"); // only reached if exec fails
    exit(1);
  } else {
    close(pipefd[0]);

    for (int i = 1; i < argc; i++) {
      write(pipefd[1], argv[i], strlen(argv[i]));
      write(pipefd[1], "\n", 1);
    }

    close(pipefd[1]);
    wait(0);
    exit(0);
  }
}
