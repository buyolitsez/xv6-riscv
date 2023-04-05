#include "kernel/types.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define CNT 5

char buff[PGSIZE * CNT];

void assert_eq(int x, int y) {
  if (x != y) {
	printf("test failed!\n");
	printf("x = %d, y = %d\n", x, y);
	exit(-1);
  }
}

int main() {
  uint64 mask;
  pgaccess((uint64)buff, CNT, (uint64)&mask);
  assert_eq(mask, 0);
  for (int i = 0; i < PGSIZE * CNT; i++)
	buff[i] = i;
  pgaccess((uint64)buff, CNT, (uint64)&mask);
  assert_eq(mask, (1L << CNT) - 1);

  pgaccess((uint64)buff, CNT, (uint64)&mask);
  assert_eq(mask, 0);

  buff[PGSIZE * 2] = 111;
  buff[PGSIZE * 3] = 111;
  pgaccess((uint64)buff, CNT, (uint64)&mask);
  assert_eq(mask, (1 << 2) + (1 << 3));
  printf("test passed!\n");
  return 0;
}
