#include "types.h"
#include "riscv.h"
#include "defs.h"

uint64
sys_add(void)
{
  int a, b;

  argint(0, &a);
  argint(1, &b);

  int sum = a + b;

  printk("kernel sys_add: a = %d, b = %d -> sum = %d\n", a, b, sum);

  return sum;
}
