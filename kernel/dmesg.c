#include <stdarg.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"

#include "dmesg.h"

#define N_SIZE 10

#define SIZE SIZE_DMESG_BUFFER

struct dmesg_s {
    char buf[SIZE];
    struct spinlock lock;
    int p; // buffer pointer
    int current_len;
} kerm;

void pr_init() { // initialize
  initlock(&kerm.lock, "dmesg");
  for (int i = 0; i < SIZE; ++i)
    kerm.buf[i] = '\0';
  kerm.p = 0;
  kerm.current_len = 0;

}

static char get_digit(uint x, int pos) { // get digit from number
  pos = N_SIZE - 1 - pos;
  while (pos > 0) {
    x /= (uint) 10;
    pos--;
  }
  return '0' + x % 10;
}

static void bfree() { // delete single message
  if (kerm.buf[kerm.p] == '\0')
    return;
  int pos = kerm.p;
  while (kerm.buf[pos] != '\n') {
    kerm.buf[pos] = '\0';
    pos = (pos + 1) % SIZE;
  }
  kerm.buf[pos] = '\0';
}

void bset(char x) { // set current symbol and move pointer
  bfree();
  kerm.buf[kerm.p] = x;
  kerm.p = (kerm.p + 1) % SIZE;
  kerm.current_len++;
  if (kerm.current_len > SIZE)
    panic("pr_msg: too big message");
}

void pr_msg(const char *fmt, ...) {
  uint tick = ticks;
  kerm.current_len = 0;

  acquire(&kerm.lock);

  bset('[');
  for (int i = 0; i < N_SIZE; ++i)
    bset(get_digit(tick, i));
  bset(']');
  bset(' ');

  va_list args;
  va_start(args, fmt);

  printing(fmt, TO_DMESG, args);

  va_end(args);

  bset('\n');

  release(&kerm.lock);
}

char user_buf[SIZE];
int user_buf_position = 0;

static void put_char(char x) {
  user_buf[user_buf_position++] = x;
}

static int put_word(int pos) { // print single message
  while (1) {
    put_char(kerm.buf[pos]);
    if (kerm.buf[pos] == '\n')
      break;
    pos = (pos + 1) % SIZE;
  }
  return (pos + 1) % SIZE;
}

static void print_dmesg(struct dmesg_s *m) {
  int start = m->p;
  int i;
  if (m->buf[start] == '\0')
    i = (start + 1) % SIZE;
  else
    i = put_word(start);
  while (i != start) {
    if (m->buf[i] != '\0')
      i = put_word(i);
    else
      i = (i + 1) % SIZE;
  }
}

void pr_copy(uint64 p) {
  pr_msg("dmesg: pr_copy on address %p called!", p);
  acquire(&kerm.lock);
  for (int i = 0; i < SIZE; ++i)
    user_buf[i] = 0;
  user_buf_position = 0;
  print_dmesg(&kerm);
  int code = copyout(myproc()->pagetable, p, (char *)&user_buf, sizeof(user_buf));
  release(&kerm.lock);
  if (code != 0)
    panic("dmesg: cannot copy to user space");
}