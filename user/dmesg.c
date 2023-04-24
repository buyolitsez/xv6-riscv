#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/riscv.h"

#include "kernel/dmesg.h"

char buf[SIZE_DMESG_BUFFER];

int main(int argc, char* argv[])
{
  copy_user_dmesg(buf);
  printf("%s", buf);
  return 0;
}

