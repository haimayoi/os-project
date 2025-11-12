#define NPROC 64

struct pinfo {
  int pid;        // Process ID
  int ppid;       // Parent process ID
  int state;      // Trạng thái (UNUSED, SLEEPING, RUNNING, etc.)
  char name[16];  // Tên của process
};