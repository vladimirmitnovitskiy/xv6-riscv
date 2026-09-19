#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

  char mode = 'a';

  if (argc == 2) {
    mode = argv[1][0];
    if ((mode != 'a' && mode != 'b') || argv[1][1] != '\0') {
      fprintf(2, "error: %s [a|b]\n", argv[0]);
      exit(1);
    }
  } else if (argc > 2) {
    fprintf(2, "error: %s [a|b]\n", argv[0]);
    exit(1);
  }

  int id = fork();
  if (id < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (id == 0) {
    printf("child id = %d\n", getpid());
    pause(100);
    exit(1);
  }

  printf("parent id = %d, child id = %d, mode = %c\n", getpid(), id, mode);

  if (mode == 'b') {
    pause(10);
    if (kill(id) < 0)
      fprintf(2, "parent kill(%d) failed\n", id);
  }

  int status = 0;
  int code = wait(&status);
  if (code < 0) {
    fprintf(2, "parent wait failed\n");
    exit(1);
  }

  printf("parent: child %d exited, status = %d\n", code, status);
  exit(0);
}