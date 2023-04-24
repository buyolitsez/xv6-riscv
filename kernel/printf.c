//
// formatted console output -- printf, panic.
//

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

// lock to avoid interleaving concurrent printf's.
static struct {
  struct spinlock lock;
  int locking;
} pr;

// Print to the console. only understands %d, %x, %p, %s.
void
printf(char *fmt, ...)
{
  int locking;

  locking = pr.locking;
  if(locking)
    acquire(&pr.lock);

  va_list args;
  va_start(args, fmt);

  printing(fmt, TO_CONSOLE, args);

  va_end(args);

  if(locking)
    release(&pr.lock);
}

void
printfinit(void)
{
  initlock(&pr.lock, "pr");
  pr.locking = 1;
}
