#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define NPROC   4
#define NWRITES 500
#define WSIZE   100

static int failures;

static void
check(int ok, char *what)
{
  printf("%s: %s\n", ok ? "ok  " : "FAIL", what);
  if (!ok)
    failures++;
}

static int
openx(char *path, int mode)
{
  int fd = open(path, mode);
  if (fd < 0) {
    printf("FAIL: open %s\n", path);
    exit(1);
  }
  return fd;
}

static uint64
nullstat(void)
{
  uint64 v = 0;
  int fd = openx("nullstat", O_RDONLY);
  if (read(fd, &v, sizeof(v)) != sizeof(v)) {
    printf("FAIL: read nullstat\n");
    exit(1);
  }
  close(fd);
  return v;
}

int
main(void)
{
  char buf[WSIZE];
  uint64 before, after, prev, v;
  int fd, i, j, pid, st, ok;

  fd = openx("null", O_RDWR);
  check(read(fd, buf, sizeof(buf)) == 0, "null: read returns EOF");
  check(write(fd, buf, sizeof(buf)) == sizeof(buf), "null: write succeeds");
  close(fd);

  fd = openx("zero", O_RDWR);
  memset(buf, 0xaa, sizeof(buf));
  ok = read(fd, buf, sizeof(buf)) == sizeof(buf);
  for (i = 0; i < sizeof(buf); i++)
    ok = ok && buf[i] == 0;
  check(ok, "zero: read returns zero bytes");
  check(write(fd, buf, 1) < 0, "zero: write fails");
  close(fd);

  fd = openx("nullstat", O_RDWR);
  check(read(fd, buf, 4) < 0, "nullstat: 4-byte read fails");
  check(read(fd, buf, 16) < 0, "nullstat: 16-byte read fails");
  before = nullstat();
  check(write(fd, buf, 10) == 10, "nullstat: write succeeds");
  check(nullstat() - before == 10, "nullstat: write is counted");
  close(fd);

  before = nullstat();
  for (i = 0; i < NPROC + 1; i++) {
    if ((pid = fork()) < 0) {
      printf("FAIL: fork\n");
      exit(1);
    }
    if (pid == 0) {
      if (i == NPROC) {
        for (j = 0, prev = 0; j < NWRITES; j++) {
          if ((v = nullstat()) < prev)
            exit(2);
          prev = v;
        }
        exit(0);
      }
      fd = openx("nullstat", O_WRONLY);
      for (j = 0; j < NWRITES; j++)
        if (write(fd, buf, WSIZE) != WSIZE)
          exit(1);
      exit(0);
    }
  }
  ok = 1;
  for (i = 0; i < NPROC + 1; i++) {
    wait(&st);
    ok = ok && st == 0;
  }
  check(ok, "concurrent: writes succeed, total never decreases");
  after = nullstat();
  check(after - before == (uint64)NPROC * NWRITES * WSIZE,
        "concurrent: every write counted exactly");

  if (failures) {
    printf("devtest: %d FAILED\n", failures);
    exit(1);
  }
  printf("devtest: all passed\n");
  exit(0);
}
