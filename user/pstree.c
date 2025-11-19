#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pinfo.h"

// Task 4.4: Hàm đệ quy in cây
// ptable: Mảng chứa dữ liệu tất cả process
// num_procs: Tổng số process
// current_ppid: Chúng ta đang tìm con của ai? (PID cha hiện tại)
// indent: Độ sâu thụt lề (0, 1, 2...)
void
print_tree(struct pinfo *ptable, int num_procs, int current_ppid, int indent)
{
  int i, j;
  
  // Duyệt qua toàn bộ danh sách để tìm con của 'current_ppid'
  for(i = 0; i < num_procs; i++){
    
    // Nếu process này có cha là 'current_ppid' -> Nó là CON
    if(ptable[i].ppid == current_ppid){
      
      // 1. In thụt lề (Task 4.4)
      // Mỗi cấp độ indent sẽ in 2 dấu cách
      for(j = 0; j < indent; j++)
        printf("  "); 
      
      // 2. In thông tin process
      // Ví dụ: `- sh (pid: 2)
      printf("`- %s (pid: %d)\n", ptable[i].name, ptable[i].pid);

      // 3. ĐỆ QUY (Recursive Call)
      // Bây giờ, tìm con của process này (pid của nó sẽ là ppid của lớp sau)
      // Tăng indent lên 1
      print_tree(ptable, num_procs, ptable[i].pid, indent + 1);
    }
  }
}

int
main(int argc, char *argv[])
{
  struct pinfo ptable[NPROC];
  int n;

  // Gọi syscall lấy dữ liệu
  n = getprocs(ptable);
  
  if(n < 0){
    printf("pstree: error calling getprocs\n");
    exit(1);
  }

  // Task 4.5: Gọi hàm đệ quy thay vì vòng lặp for cũ
  printf("Process Tree:\n");
  
  // Bắt đầu từ GỐC (Root).
  // Process gốc (init) có PPID là 0.
  print_tree(ptable, n, 0, 0);

  exit(0);
}