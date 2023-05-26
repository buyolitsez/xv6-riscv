#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void
bigfile(char *s)
{
  char buf[MAXOPBLOCKS + 2 * BSIZE];
  enum { N = 200, SZ=600 };
  int fd, i, total, cc;

  unlink("bigfile.dat");
  fd = open("bigfile.dat", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("%s: cannot create bigfile", s);
    exit(1);
  }
  for(i = 0; i < N; i++){
    memset(buf, i, SZ);
    if(write(fd, buf, SZ) != SZ){
      printf("%s: write bigfile failed\n", s);
      exit(1);
    }
  }
  close(fd);

  fd = open("bigfile.dat", 0);
  if(fd < 0){
    printf("%s: cannot open bigfile\n", s);
    exit(1);
  }
  total = 0;
  for(i = 0; ; i++){
    cc = read(fd, buf, SZ/2);
    if(cc < 0){
      printf("%s: read bigfile failed\n", s);
      exit(1);
    }
    if(cc == 0)
      break;
    if(cc != SZ/2){
      printf("%s: short read bigfile\n", s);
      exit(1);
    }
    if(buf[0] != i/2 || buf[SZ/2-1] != i/2){
      printf("%s: read bigfile wrong data\n", s);
      exit(1);
    }
    total += cc;
  }
  close(fd);
  if(total != N*SZ){
    printf("%s: read bigfile wrong total\n", s);
    exit(1);
  }
  unlink("bigfile.dat");
}


int main(int argc, char *argv[]) {
  bigfile("bigfile test");
  exit(0);
}