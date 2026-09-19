#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: add <num1> <num2>\n");
        exit(-1);
    }
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int res = add(a,b);
    printf("%d\n", res);
    exit(0);
}