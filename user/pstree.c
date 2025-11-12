#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pinfo.h" 

int
main(int argc, char *argv[])
{
  int n;
  n = getprocs(0);

  printf("syscall getprocs returned %d\n", n);
  exit(0);
}