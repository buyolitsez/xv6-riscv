#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/my_lib.h"

int main(int argc, char* argv[])
{
  start_log_dmesg();
  int p[2];
  exitOnError(pipe(p), "Cannot create pipe!");
  int childPid = fork();
  exitOnError(childPid, "Cannot create child process!");
  if (childPid == 0) {
	exitOnError(close(0), "Cannot close stdin!");
	exitOnError(dup(p[0]), "Cannot create dup of fd");
	exitOnError(close(p[0]), "Cannot close pipe in");
	exitOnError(close(p[1]), "Cannot close pipe out");

	char* wcargv[2];
	wcargv[0] = "wc";
	wcargv[1] = 0;
	exec("wc", wcargv);
	exitOnError(-1, "Cannot run exec 'wc'!");
  } else if (childPid > 0) {
	exitOnError(close(p[0]), "Cannot close pipe in!");

	// read line from input stream
	const int BUF_SIZE = 100;
	char buf[BUF_SIZE + 1];
	exitOnError(read(0, buf, BUF_SIZE), "Read error!");
	int cnt = 0;
	while (cnt < BUF_SIZE && buf[cnt] != '\n')
	  ++cnt;
	buf[cnt++] = '\n';
	// put given string to channel
	exitOnError(write(p[1], buf, cnt), "Write error!");
	exitOnError(close(p[1]), "Close pip out error!");
  }
  return 0;
}
