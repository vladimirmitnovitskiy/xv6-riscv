#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "usage: hexdump n file\n");
    exit(1);
  }

  int n = atoi(argv[1]);
  if(n <= 0){
    fprintf(2, "hexdump: invalid count\n");
    exit(1);
  }

  int fd = open(argv[2], O_RDONLY);
  if(fd < 0){
    fprintf(2, "hexdump: cannot open %s\n", argv[2]);
    exit(1);
  }

  char buf[512];
  int total = 0;
  while(total < n){
    int want = n - total;
    if(want > sizeof(buf))
      want = sizeof(buf);

    int r = read(fd, buf, want);
    if(r < 0){
      printf("Read error\n");
      close(fd);
      exit(1);
    }
    if(r == 0)
      break;

    for(int i = 0; i < r; i++){
      if(i > 0 || total > 0)
        printf(" ");
      printf("%02x", (unsigned char)buf[i]);
    }
    total += r;
  }
  printf("\n");

  close(fd);
  exit(0);
}