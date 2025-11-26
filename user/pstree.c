#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pinfo.h"

#define MAX_PREFIX 256

void
strcat_custom(char *dest, const char *src)
{
  while(*dest) dest++;
  while(*src) *dest++ = *src++;
  *dest = 0;
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

      printf("%s(%d)\n", ptable[i].name, ptable[i].pid);


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

  n = getprocs(ptable);
  
  if(n < 0){
    printf("pstree: error calling getprocs\n");
    exit(1);
  }

  root_prefix[0] = 0;

  printf("Process Tree:\n");
  
  print_tree(ptable, n, 0, root_prefix);

  exit(0);
}