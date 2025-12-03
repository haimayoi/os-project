#define NPROC 64

struct pinfo {
  int pid;        // Process ID
  int ppid;       // Parent process ID
  int state;      // Trạng thái (UNUSED, SLEEPING, RUNNING, etc.)
  uint64 sz;      // Kích thước bộ nhớ - Bytes
  char name[16];  // Tên của process
};