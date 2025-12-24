#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  int pid = fork();

  if(pid < 0){
    printf("Fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Tiến trình CON: Kết thúc ngay để thành Zombie
    printf("Child (PID %d) exiting to become a zombie...\n", getpid());
    exit(0);
  } else {
    // Tiến trình CHA: Ngủ thật lâu và KHÔNG gọi wait()
    printf("Parent (PID %d) is sleeping without waiting for child...\n", getpid());
    printf("Run 'pstree' now to see the zombie!\n");
    sleep(100); // Ngủ 10 giây
    
    // Sau khi dậy mới dọn dẹp (hoặc thoát luôn để init dọn)
    wait(0);
  }
  exit(0);
}