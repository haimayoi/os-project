// user/ps.c
#include "kernel/types.h"
#include "user/user.h" // user.h đã include procinfo.h
#include "kernel/param.h"

int main(int argc, char *argv[])
{
  struct procinfo p_table[NPROC];
  int count;

  printf("Hai Van's syscall");

  // Gọi system call
  count = procinfo(p_table);
  if (count < 0) {
    printf("ps: procinfo failed\n");
    exit(1);
  }

  // In tiêu đề
  printf("PID\tSTATE\tNAME\n");

  // In thông tin từng tiến trình
  for (int i = 0; i < count; i++) {
    printf("%d\t%d\t%s\n", p_table[i].state, p_table[i].pid, p_table[i].name);
  }

  exit(0);
}
