#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pinfo.h" 

int
main(int argc, char *argv[])
{
  // 1. Khai báo mảng ptable (nơi kernel sẽ copy dữ liệu vào)
  struct pinfo ptable[NPROC];
  int n; // Biến lưu số lượng process nhận được

  // 2. Gọi system call:
  // Thay vì 'getprocs(0)', ta truyền con trỏ 'ptable'
  n = getprocs(ptable);

  // 3. Kiểm tra lỗi
  if(n < 0){
    printf("pstree: lỗi khi gọi getprocs\n");
    exit(1);
  }

  // 4. In kết quả (danh sách phẳng)
  // Đây là mục tiêu của Tuần 3: in ra danh sách thô
  printf("Tìm thấy %d tiến trình:\n", n);
  printf("------------------------------------\n");
  printf("PID\tPPID\tNAME\tSTATE\n");

  for(int i = 0; i < n; i++) {
    // In dữ liệu từ mảng ptable[i]
    printf("%d\t%d\t%s\t%d\n", 
           ptable[i].pid, 
           ptable[i].ppid, 
           ptable[i].name,
           ptable[i].state);
  }

  exit(0);
}