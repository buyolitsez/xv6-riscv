#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#include "dmesg.h"

//#define SIZE (DMESG_BFS * PGSIZE)
#define SIZE 80
#define MARK_SIZE 14
#define N_SIZE 10

struct {
  char buf[SIZE];
  struct spinlock lock;
  int p; // buffer pointer
} m;

void pr_init() { // initialize
  initlock(&m.lock, "dmesg");
  for (int i = 0; i < SIZE; ++i)
	m.buf[i] = '\0';
  m.p = 0;
}

char get_digit(uint x, int pos) { // get digit from number
  pos = N_SIZE - 1 - pos;
  while (pos > 0) {
	x /= (uint)10;
	pos--;
  }
  return '0' + x % 10;
}

void bfree() { // delete single message
  if (m.buf[m.p] == '\0')
	return;
  int pos = m.p;
  while (m.buf[pos] != '\n') {
	m.buf[pos] = '\0';
	pos = (pos + 1) % SIZE;
  }
  m.buf[pos] = '\0';
}

void bset(char x) { // set current symbol and move pointer
  bfree();
  m.buf[m.p] = x;
  m.p = (m.p + 1) % SIZE;
}

void pr_msg(const char *str) {
  uint tick = ticks;
  int len = strlen(str);
  if (len + MARK_SIZE > SIZE)
	panic("pr_msg: too big message");
  for (int i = 0; i < len; ++i)
	if (str[i] == '\n')
	  panic("pr_msg: message should be in one line");

  acquire(&m.lock);

  bset('[');
  for (int i = 0; i < N_SIZE; ++i)
	bset(get_digit(tick, i));
  bset(']');
  bset(' ');
  for (int i = 0; i < len; ++i)
	bset(str[i]);
  bset('\n');

  release(&m.lock);
}

int put_word(int pos) { // print single message
  while (1) {
	char toprint[2];
	toprint[0] = m.buf[pos];
	toprint[1] = '\0';
	printf("%s", toprint);
	if (m.buf[pos] == '\n')
	  break;
	pos = (pos + 1) % SIZE;
  }
  return (pos + 1) % SIZE; 
}

void pr_dmesg() {
  acquire(&m.lock);
  int start = m.p;
  int i = start;
  if (m.buf[start] == '\0')
	i = (start + 1) % SIZE;
  else
	i = put_word(start);
  while (i != start) {
	if (m.buf[i] != '\0')
	  i = put_word(i);
	else
	  i = (i + 1) % SIZE;
  }
  release(&m.lock);
}
