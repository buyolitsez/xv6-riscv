#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/my_lib.h"

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

	readAndWrite(pChildRead[0], pParentRead[1]);

  } else {
	exitOnError((argc < 2 ? -1 : 0), "Too few arguments, need at least one");
	close(pParentRead[1]); // аналогично тому что в ребенке
	close(pChildRead[0]);

	// этап вывода
	for (int i = 0; argv[1][i] != '\0'; ++i)
		write(pChildRead[1], argv[1] + i, 1);
	close(pChildRead[1]);

	wait(0);

	readAndWrite(pParentRead[0], -1);

  }
  return 0;
}

