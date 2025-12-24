#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pinfo.h"

#define MAX_PREFIX 64

// Định nghĩa các tên trạng thái tương ứng với định nghĩa trong kernel/proc.h
char *proc_states[] = {
  [0] "UNUSED  ",
  [1] "USED    ",
  [2] "SLEEPING",
  [3] "RUNNABLE",
  [4] "RUNNING ",
  [5] "ZOMBIE  "
};

// Hàm nối chuỗi thủ công
void
strcat_custom(char *dest, const char *src)
{
  while(*dest) dest++;
  while(*src) *dest++ = *src++;
  *dest = 0;
}

// Hàm so sánh chuỗi (để kiểm tra tham số "-r")
int
strcmp_custom(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

// Kiểm tra con út
int
is_last_child(struct pinfo *ptable, int num_procs, int parent_pid, int child_pid)
{
  int last_child_found = -1;
  for(int i = 0; i < num_procs; i++){
    if(ptable[i].ppid == parent_pid){
      last_child_found = ptable[i].pid;
    }
  }
  return (child_pid == last_child_found);
}

// Hàm đệ quy in cây với đầy đủ thông tin: Tên, PID, Trạng thái, Bộ nhớ
void
print_tree(struct pinfo *ptable, int num_procs, int current_ppid, char *prefix)
{
  int i;
  char new_prefix[MAX_PREFIX];

  for(i = 0; i < num_procs; i++){
    if(ptable[i].ppid == current_ppid){
      
      int last = is_last_child(ptable, num_procs, current_ppid, ptable[i].pid);

      // 1. In tiền tố cây
      printf("%s", prefix);
      if(last) printf("`-- ");
      else     printf("|-- ");

      // 2. Lấy tên trạng thái an toàn
      char *state_str = "UNKNOWN ";
      if(ptable[i].state >= 0 && ptable[i].state <= 5) {
        state_str = proc_states[ptable[i].state];
      }

      // 3. Tính toán bộ nhớ (KB)
      uint64 mem_kb = ptable[i].sz / 1024;

      // 4. In thông tin chi tiết: name(pid) [STATE] [Mem]
      if (mem_kb == 0) {
        printf("%s(pid:%d) [%s] [%d b]\n", 
               ptable[i].name, ptable[i].pid, state_str, (int)ptable[i].sz);
      } else {
        printf("%s(pid:%d) [%s] [%d KB]\n", 
               ptable[i].name, ptable[i].pid, state_str, (int)mem_kb);
      }

      // 5. Chuẩn bị prefix cho đệ quy (Dùng tối đa 64 ký tự để tránh tràn stack)
      if(strlen(prefix) < MAX_PREFIX - 5) {
        strcpy(new_prefix, prefix);
        if(last) strcat_custom(new_prefix, "    ");
        else     strcat_custom(new_prefix, "|   ");

        print_tree(ptable, num_procs, ptable[i].pid, new_prefix);
      }
    }
  }
}

int
main(int argc, char *argv[])
{
  struct pinfo ptable[NPROC];
  int n;
  char root_prefix[MAX_PREFIX];
  int monitor_mode = 0;

  if(argc > 1 && strcmp_custom(argv[1], "-r") == 0){
    monitor_mode = 1;
  }

  do {
    if(monitor_mode) {
      printf("\033[2J\033[H");
      printf("=== PSTREE SYSTEM MONITOR (PID, STATE, MEMORY) ===\n\n");
    }

    n = getprocs(ptable);
    if(n < 0){
      printf("pstree: error calling getprocs\n");
      exit(1);
    }

    root_prefix[0] = 0;
    if(!monitor_mode) printf("Process Tree:\n");
    
    // Bắt đầu từ PID cha = 0
    print_tree(ptable, n, 0, root_prefix);

    if(monitor_mode) {
      sleep(10); 
    }
  } while(monitor_mode);

  exit(0);
}