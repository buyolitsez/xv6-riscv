#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/my_lib.h"

#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/blocker.h"

void readAndWrite(int readFrom, int writeTo)
{
  char a[1];
  while (read(readFrom, a, 1) > 0) {
	printf("%d: received %c\n", getpid(), a[0]);
	if (writeTo != -1)
	  write(writeTo, a, 1);
  }
  close(readFrom);
  close(writeTo);
}

int main(int argc, char* argv[])
{
  int pChildRead[2];
  int pParentRead[2];
  exitOnError(pipe(pChildRead), "Cannot create pipe!");
  exitOnError(pipe(pParentRead), "Cannot create pipe!");

  int childPid = fork();
  exitOnError(childPid, "Cannot create child process!");
  if (childPid == 0) {
	close(pParentRead[0]); // мы не будем читать из этого пайпа
	close(pChildRead[1]);  // мы не будем писать в свой же пайп
	int lock;
	exitOnError(read(pChildRead[0], &lock, sizeof(lock)) > 0 ? 0 : -1, "Cannot read lock id from child");

	exitOnError(blocker(BACQUIRE, lock), "Cannot lock sleeplock!");
	readAndWrite(pChildRead[0], pParentRead[1]);
	exitOnError(blocker(BRELEASE, lock), "Cannot release sleeplock!");

  } else {
	exitOnError((argc < 2 ? -1 : 0), "Too few arguments, need at least one");
	close(pParentRead[1]); // аналогично тому что в ребенке
	close(pChildRead[0]);

	int lock = blocker(BGET, -1);
	exitOnError(lock, "Cannot create new sleep lock!");

	{
	  write(pChildRead[1], &lock, sizeof(lock));

	  // этап вывода
	  for (int i = 0; argv[1][i] != '\0'; ++i)
		write(pChildRead[1], argv[1] + i, 1);
	  close(pChildRead[1]);
	}

	char a[1];
	int hasLock = 0;
	while (read(pParentRead[0], a, 1) > 0) {
	  if (!hasLock) {
		exitOnError(blocker(BACQUIRE, lock), "Cannot lock sleeplock!");
		hasLock = 1;
	  }
	  printf("%d: received %c\n", getpid(), a[0]);
	}
	exitOnError(blocker(BRELEASE, lock), "cannot release sleeplock!");
	close(pParentRead[0]);
  }
  return 0;
}

