//
// Pseudo-devices sharing one major number (MEMDEV):
//   null     (minor 0) -- read: end of file; write: data discarded.
//   zero     (minor 1) -- read: zero bytes; write: error.
//   nullstat (minor 2) -- write: data discarded, but the byte count
//                         is accumulated; read: exactly sizeof(uint64)
//                         bytes giving the total written since boot.
//

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "riscv.h"
#include "defs.h"

// several processes may write nullstat at once on different harts;
// the lock makes each "+= n" (a load and a store) atomic.
static struct {
  struct spinlock lock;
  uint64 written; // total bytes written to nullstat
} nullstat;

// fill [dst, dst+n) with zero bytes.
static int
zeroread(int user_dst, uint64 dst, int n)
{
  static const char zeros[64];
  int i = 0;

  while (i < n) {
    int nn = sizeof(zeros);
    if (nn > n - i)
      nn = n - i;
    if (either_copyout(user_dst, dst + i, (void *)zeros, nn) == -1)
      return i > 0 ? i : -1;
    i += nn;
  }
  return i;
}

static int
nullstatread(int user_dst, uint64 dst, int n)
{
  uint64 total;

  if (n != sizeof(total))
    return -1;

  acquire(&nullstat.lock);
  total = nullstat.written;
  release(&nullstat.lock);

  if (either_copyout(user_dst, dst, &total, sizeof(total)) == -1)
    return -1;
  return sizeof(total);
}

static int
memdevread(int user_dst, uint64 dst, int n, int minor)
{
  switch (minor) {
  case MEMDEV_NULL:
    return 0;
  case MEMDEV_ZERO:
    return zeroread(user_dst, dst, n);
  case MEMDEV_NULLSTAT:
    return nullstatread(user_dst, dst, n);
  default:
    return -1;
  }
}

static int
memdevwrite(int user_src, uint64 src, int n, int minor)
{
  switch (minor) {
  case MEMDEV_NULL:
    return n;
  case MEMDEV_ZERO:
    return -1;
  case MEMDEV_NULLSTAT:
    acquire(&nullstat.lock);
    nullstat.written += n;
    release(&nullstat.lock);
    return n;
  default:
    return -1;
  }
}

void
memdevinit(void)
{
  initlock(&nullstat.lock, "nullstat");
  nullstat.written = 0;

  devsw[MEMDEV].read = memdevread;
  devsw[MEMDEV].write = memdevwrite;
}
