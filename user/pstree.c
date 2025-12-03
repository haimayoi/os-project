#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pinfo.h"

#define MAX_PREFIX 64

void
strcat_custom(char *dest, const char *src)
{
  while(*dest) dest++;
  while(*src) *dest++ = *src++;
  *dest = 0;
}

int
strcmp_custom(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

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

void
print_tree(struct pinfo *ptable, int num_procs, int current_ppid, char *prefix)
{
  int i;
  char new_prefix[MAX_PREFIX];

  for(i = 0; i < num_procs; i++){
    if(ptable[i].ppid == current_ppid){
      
      int last = is_last_child(ptable, num_procs, current_ppid, ptable[i].pid);

      printf("%s", prefix);

      if(last) {
        printf("`-- ");
      } else {
        printf("|-- "); 
      }

      uint64 mem_kb = ptable[i].sz / 1024;

      if (mem_kb == 0) {
        printf("%s(pid:%d) [Mem: %d b]\n", ptable[i].name, ptable[i].pid, (int)ptable[i].sz);
      } else {
        printf("%s(pid:%d) [Mem: %d KB]\n", ptable[i].name, ptable[i].pid, (int)mem_kb);
      }

      strcpy(new_prefix, prefix);
      
      if(last){
        strcat_custom(new_prefix, "    ");
      } else {
        strcat_custom(new_prefix, "|   ");
      }

      print_tree(ptable, num_procs, ptable[i].pid, new_prefix);
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
      printf("PSTREE REAL-TIME MONITOR (Press Ctr + A, then X to esc)\n\n");
    }

    n = getprocs(ptable);
    
    if(n < 0){
      printf("pstree: error calling getprocs\n");
      exit(1);
    }

    root_prefix[0] = 0;

    if(!monitor_mode) printf("Process Tree:\n");
    
    print_tree(ptable, n, 0, root_prefix);

    if(monitor_mode) {
      sleep(10); 
    }
  } while(monitor_mode);
  exit(0);
}