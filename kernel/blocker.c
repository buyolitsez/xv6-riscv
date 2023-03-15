#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "file.h"

#include "blocker.h"

struct {
  struct spinlock lock;
  struct sleeplock locks[NLOCKS];
  int deleted[NLOCKS];
} ltable;

void blockerinit() {
  initlock(&ltable.lock, "ltable");

  for(int i = 0; i < NLOCKS; ++i) {
	initsleeplock(&ltable.locks[i], "ltablelock");
  }
}

int blget() {
  int result = -1;

  acquire(&ltable.lock);
  for (int i = 0; i < NLOCKS; ++i) {
	if (ltable.deleted[i] || ltable.locks[i].locked)
	  continue;
	result = i;
	break;
  }

  release(&ltable.lock);
  return result;
}

int isOkId(int id) {
  return id >= 0 && id < NLOCKS && !ltable.deleted[id];
}

int bllock(int id) {
  if (!isOkId(id))
	return -1;
  acquiresleep(&ltable.locks[id]);
  return 0;
}

int blrelease(int id) {
  if (!isOkId(id))
	return -1;
  releasesleep(&ltable.locks[id]);
  return 0;
}

int bldelete(int id) {
  if (!isOkId(id))
	return -1;
  ltable.deleted[id] = 1;
  return 0;
}
