#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/my_lib.h"

static int get(char* s, int pos) {
  return s[pos] - '0';
}

int main(int argc, char* argv[])
{
  exitOnError((argc != 2) ? -1 : 0, "interrupt - switching - syscall\n for example: 010\n");
  exitOnError((strlen(argv[1]) != 3) ? -1 : 0, "flags len should be 3\n");
  start_log_dmesg(get(argv[1], 0) + get(argv[1], 1) * 2 + get(argv[1], 2) * 4);
  return 0;
}
