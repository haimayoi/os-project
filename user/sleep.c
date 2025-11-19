#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;

  if(argc != 2){
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }

  // Chuyển chuỗi thành số nguyên
  i = atoi(argv[1]);
  
  // Gọi system call sleep
  sleep(i);
  
  exit(0);
}