#define NPROC 64

struct procinfo {
  int pid;
  int state;
  char name[16];
};
