#include "kernel/types.h"
#include "user/user.h"

// Tiến trình con tính toán nặng (để thấy trạng thái RUNNING/RUNNABLE)
void cpu_burner() {
    printf("CPU Burner (PID %d) started...\n", getpid());
    double x = 0;
    while(1) {
        x += 0.0001; // Chạy vòng lặp vô hạn
    }
}

// Tiến trình tạo ra một chuỗi cha-con-cháu sâu 3 tầng
void deep_tree(int level) {
    if (level <= 0) {
        printf("Deep leaf (PID %d) sleeping...\n", getpid());
        sleep(1000);
        exit(0);
    }
    int pid = fork();
    if (pid == 0) {
        deep_tree(level - 1);
    } else {
        sleep(1000);
        wait(0);
    }
}

// Tiến trình tạo ra Zombie
void zombie_maker() {
    int pid = fork();
    if (pid == 0) {
        // Con thoát ngay lập tức
        exit(0); 
    } else {
        // Cha ngủ lâu và KHÔNG gọi wait() ngay
        printf("Zombie Child (PID %d) created. Parent (PID %d) sleeping...\n", pid, getpid());
        sleep(1000);
        wait(0);
    }
}

int main() {
    printf("Starting Workload Generator (PID %d)...\n", getpid());

    // 1. Tạo 1 tiến trình RUNNING (CPU intensive)
    if (fork() == 0) cpu_burner();

    // 2. Tạo 1 tiến trình SLEEPING
    if (fork() == 0) {
        printf("Sleeper (PID %d) started...\n", getpid());
        sleep(1000);
        exit(0);
    }

    // 3. Tạo 1 Zombie
    if (fork() == 0) zombie_maker();

    // 4. Tạo 1 cây sâu (Deep Hierarchy)
    if (fork() == 0) deep_tree(2);

    // Tiến trình gốc của workload cũng đi ngủ để giữ cây
    sleep(1000);
    
    // Thu dọn (thực tế ít khi chạy đến đây vì ta sẽ thoát QEMU)
    while(wait(0) > 0);
    exit(0);
}