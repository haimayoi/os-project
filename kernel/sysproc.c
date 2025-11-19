#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "procinfo.h"
#include "pinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
uint64
sys_hello(void)
{
  struct proc *p = myproc();
  printf("kernel: hello() called by pid %d (running in kernel)\n", p->pid);
  return 0; 
}

extern struct proc proc[NPROC];

uint64
sys_procinfo(void)
{
  struct procinfo p_info[NPROC];
  int i = 0;

  // 1. Lặp qua bảng tiến trình của kernel
  for (struct proc *p = proc; p < &proc[NPROC]; p++) {
    if (p->state != UNUSED) {
      // 2. Thu thập thông tin
      p_info[i].pid = p->pid;
      p_info[i].state = p->state;
      strncpy(p_info[i].name, p->name, 16);
      i++;
    }
  }

  // 3. Lấy địa chỉ con trỏ mà chương trình người dùng đã truyền vào
  uint64 user_ptr;
  
  // Sửa lỗi 1: Không kiểm tra giá trị trả về của argaddr
  argaddr(0, &user_ptr);

  // 4. Sao chép an toàn dữ liệu từ kernel ra không gian người dùng
  // Sửa lỗi 2: Thêm tham số đầu tiên là pagetable và gọi đúng thứ tự
  if (copyout(myproc()->pagetable, user_ptr, (char *)p_info, i * sizeof(struct procinfo)) < 0) {
    return -1;
  }

  // Trả về số lượng tiến trình đã tìm thấy
  return i;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  // 1. Lấy tham số (số ticks)
  argint(0, &n);
  
  // 2. Bắt đầu ngủ
  acquire(&tickslock);
  ticks0 = ticks;
  
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    // Ngủ và chờ đợi
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_getprocs(void)
{
  uint64 user_addr; // Địa chỉ user-space truyền vào
  struct pinfo kbuf[NPROC]; // Buffer tạm trong kernel
  struct proc *p;
  int i = 0;

  // 1. Lấy địa chỉ con trỏ từ user (tham số đầu tiên)
  argaddr(0, &user_addr);

  // 2. Duyệt qua bảng tiến trình toàn cục
  // Ta phải duyệt hết NPROC slot để tìm các process đang hoạt động
  for(p = proc; p < &proc[NPROC]; p++) {
    
    // A. ACQUIRE LOCK
    // Bắt buộc phải lock để đảm bảo tính toàn vẹn dữ liệu
    // Tránh trường hợp process bị kill hoặc thay đổi trạng thái khi đang đọc
    acquire(&p->lock);

    // B. Chỉ lấy các process không ở trạng thái UNUSED
    if(p->state != UNUSED) {
      kbuf[i].pid = p->pid;
      kbuf[i].state = p->state;
      
      // Copy tên process an toàn
      safestrcpy(kbuf[i].name, p->name, sizeof(kbuf[i].name));

      // Xử lý PPID (Parent PID)
      // Nếu có cha thì lấy pid cha, nếu không (là init) thì ppid = 0
      if(p->parent) {
        kbuf[i].ppid = p->parent->pid;
      } else {
        kbuf[i].ppid = 0;
      }
      
      i++; // Tăng đếm số lượng process tìm thấy
    }

    // C. RELEASE LOCK
    // Nhả lock ngay sau khi đọc xong để không làm chậm hệ thống
    release(&p->lock);
  }

  // 3. COPYOUT: Chuyển dữ liệu từ Kernel -> User
  // copyout(pagetable, đích_user, nguồn_kernel, kích_thước)
  if(copyout(myproc()->pagetable, user_addr, (char *)kbuf, i * sizeof(struct pinfo)) < 0) {
    return -1; // Trả về lỗi nếu copy thất bại
  }

  // 4. Trả về số lượng process thực tế
  return i;
}