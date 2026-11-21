#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
put(char *s, int n)
{
  if (write(1, s, n) != n) {
    fprintf(2, "echo: write error\n");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  int i;

  for (i = 1; i < argc; i++) {
    put(argv[i], strlen(argv[i]));
    if (i + 1 < argc) {
      put(" ", 1);
    } else {
      put("\n", 1);
    }
  }
  exit(0);
}
