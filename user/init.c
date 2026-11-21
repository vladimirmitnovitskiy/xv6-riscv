// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/file.h"


char *argv[] = {"sh", 0};

int
main(void)
{
  int pid, wpid;
  struct stat st;

  if (open("console", O_RDWR) < 0) {
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0); // stdout
  dup(0); // stderr

    
  
  if(stat("null", &st) < 0){
    mknod("null", MEMDEV, MEMDEV_NULL);
  }
  if(stat("zero", &st) < 0){
    mknod("zero", MEMDEV, MEMDEV_ZERO);
  }
  if(stat("nullstat", &st) < 0){
    mknod("nullstat", MEMDEV, MEMDEV_NULLSTAT);
  }
  if(stat("rtc", &st) < 0){
    mknod("rtc", RTCDEV, RTCDEV_RTC);
  }

  for (;;) {
    printf("init: starting sh\n");
    pid = fork();
    if (pid < 0) {
      printf("init: fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for (;;) {
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *)0);
      if (wpid == pid) {
        // the shell exited; restart it.
        break;
      } else if (wpid < 0) {
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
