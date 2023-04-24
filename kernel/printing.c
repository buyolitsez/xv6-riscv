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

volatile int panicked = 0;

static char digits[] = "0123456789abcdef";

void kputc(int c, enum printing_to to) {
  if (to == TO_CONSOLE)
    consputc(c);
  else if (to == TO_DMESG)
    bset(c);
}

static void
printint(int xx, int base, int sign, enum printing_to to) {
  char buf[16];
  int i;
  uint x;

  if (sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);

  if (sign)
    buf[i++] = '-';

  while (--i >= 0)
    kputc(buf[i], to);
}

static void
printptr(uint64 x, enum printing_to to) {
  int i;
  kputc('0', to);
  kputc('x', to);
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    kputc(digits[x >> (sizeof(uint64) * 8 - 4)], to);
}

// Only understands %d, %x, %p, %s.
void
printing(const char *fmt, enum printing_to to, va_list ap) {
  int i, c;
  char *s;

  if (fmt == 0)
    panic("null fmt");

  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      kputc(c, to);
      continue;
    }
    c = fmt[++i] & 0xff;
    if (c == 0)
      break;
    switch (c) {
      case 'd':
        printint(va_arg(ap,
        int), 10, 1, to);
        break;
      case 'x':
        printint(va_arg(ap,
        int), 16, 1, to);
        break;
      case 'p':
        printptr(va_arg(ap, uint64), to);
        break;
      case 's':
        if ((s = va_arg(ap, char*)) == 0)
        s = "(null)";
        for (; *s; s++)
          kputc(*s, to);
        break;
      case '%':
        kputc('%', to);
        break;
      default:
        // Print unknown % sequence to draw attention.
        kputc('%', to);
        kputc(c, to);
        break;
    }
  }
}

void
panic(char *s) {
  printf("panic: ");
  printf(s);
  printf("\n");
  panicked = 1; // freeze uart output from other CPUs
  for (;;);
}
