#include <stdarg.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#include "dmesg.h"

#define SIZE (DMESG_BFS * PGSIZE)
#define N_SIZE 10

struct dmesg_s {
    char buf[SIZE];
    struct spinlock lock;
    int p; // buffer pointer
    int current_len;
} kerm, usrm;

static void dmesg_init(struct dmesg_s *m, char *lockname) {
  initlock(&m->lock, lockname);
  for (int i = 0; i < SIZE; ++i)
    m->buf[i] = '\0';
  m->p = 0;
  m->current_len = 0;
}

void pr_init() { // initialize
  dmesg_init(&kerm, "dmesg");
  dmesg_init(&usrm, "user-dmesg");
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

static int put_word(int pos) { // print single message
  while (1) {
    consputc(kerm.buf[pos]);
    if (kerm.buf[pos] == '\n')
      break;
    pos = (pos + 1) % SIZE;
  }
  return (pos + 1) % SIZE;
}

static void print_dmesg(struct dmesg_s *m) {
  acquire(&m->lock);
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
  release(&m->lock);
}

void pr_dmesg() {
  print_dmesg(&kerm);
}

void pr_user_dmesg() {
  print_dmesg(&usrm);
}

void pr_copy() {
  acquire(&kerm.lock);
  acquire(&usrm.lock);
  usrm.p = kerm.p;
  for (int i = 0; i < SIZE; ++i)
    usrm.buf[i] = kerm.buf[i];
  release(&usrm.lock);
  release(&kerm.lock);
}