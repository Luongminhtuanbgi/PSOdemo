#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// --- CẤU HÌNH THAM SỐ ---
#define NUM_PARTICLES 30    // Số lượng hạt
#define MAX_ITER 100        // Số vòng lặp tối đa
#define DIM 2               // Số chiều (x, y)
#define MIN_X -100.0        // Giới hạn tìm kiếm
#define MAX_X 100.0

// Tham số N-PSO (Thường w giữ cố định hoặc giảm nhẹ)
#define W 0.729             // Trọng số quán tính
#define C1 1.494            // Hệ số cá nhân
#define C2 1.494            // Hệ số xã hội (ở đây là lân cận)

typedef struct {
    double position[DIM];
    double velocity[DIM];
    double pBest_pos[DIM];
    double pBest_val;
    double fitness;
} Particle;

// Hàm mục tiêu: f(x) = x^2 + y^2
double objective_function(double pos[]) {
    double sum = 0;
    for (int i = 0; i < DIM; i++) sum += pos[i] * pos[i];
    return sum;
}

double rand_range(double min, double max) {
    return min + (double)rand() / RAND_MAX * (max - min);
}

double rand_01() {
    return (double)rand() / RAND_MAX;
}

int main() {
    srand(time(NULL));

    Particle swarm[NUM_PARTICLES];
    
    // Biến gBest này CHỈ DÙNG ĐỂ IN KẾT QUẢ CUỐI CÙNG
    // (Trong N-PSO, các hạt KHÔNG dùng biến này để di chuyển)
    double global_best_pos[DIM];
    double global_best_val = 1e15;

    // 1. KHỞI TẠO
    for (int i = 0; i < NUM_PARTICLES; i++) {
        for (int d = 0; d < DIM; d++) {
            swarm[i].position[d] = rand_range(MIN_X, MAX_X);
            swarm[i].velocity[d] = rand_range(-1.0, 1.0);
            swarm[i].pBest_pos[d] = swarm[i].position[d];
        }
        swarm[i].fitness = objective_function(swarm[i].position);
        swarm[i].pBest_val = swarm[i].fitness;

        // Lưu lại kết quả tốt nhất để báo cáo
        if (swarm[i].fitness < global_best_val) {
            global_best_val = swarm[i].fitness;
            for(int d=0; d<DIM; d++) global_best_pos[d] = swarm[i].position[d];
        }
    }

    printf("--- BAT DAU N-PSO (Local Topology) ---\n");

    // 2. VÒNG LẶP CHÍNH
    for (int iter = 0; iter < MAX_ITER; iter++) {
        
        for (int i = 0; i < NUM_PARTICLES; i++) {
            
            // TÌM HÀNG XÓM TỐT NHẤT (Local Best - lBest)
            // Giả sử xếp các hạt thành vòng tròn (Ring Topology)
            // Hạt i sẽ có hàng xóm là: (i-1) và (i+1)
            
            int left = (i - 1 + NUM_PARTICLES) % NUM_PARTICLES; // Xử lý số âm khi i=0
            int right = (i + 1) % NUM_PARTICLES;                // Xử lý khi i=29
            
            int lBest_idx = i; // Giả sử bản thân nó là tốt nhất trước

            // So sánh với hàng xóm Trái
            if (swarm[left].pBest_val < swarm[lBest_idx].pBest_val) {
                lBest_idx = left;
            }
            // So sánh với hàng xóm Phải
            if (swarm[right].pBest_val < swarm[lBest_idx].pBest_val) {
                lBest_idx = right;
            }

            // --> swarm[lBest_idx] chính là "thần tượng" cục bộ của hạt i lúc này

            for (int d = 0; d < DIM; d++) {
                double r1 = rand_01();
                double r2 = rand_01();

                // CÔNG THỨC CẬP NHẬT VẬN TỐC
                // Thay thế gBest bằng lBest (swarm[lBest_idx].pBest_pos)
                
                double cognitive = C1 * r1 * (swarm[i].pBest_pos[d] - swarm[i].position[d]);
                
                // Thành phần xã hội: Kéo về lBest chứ không phải gBest
                double social    = C2 * r2 * (swarm[lBest_idx].pBest_pos[d] - swarm[i].position[d]);

                swarm[i].velocity[d] = W * swarm[i].velocity[d] + cognitive + social;

                // Cập nhật vị trí
                swarm[i].position[d] += swarm[i].velocity[d];

                // Kiểm tra biên
                if (swarm[i].position[d] > MAX_X) swarm[i].position[d] = MAX_X;
                if (swarm[i].position[d] < MIN_X) swarm[i].position[d] = MIN_X;
            }

            // Đánh giá lại
            swarm[i].fitness = objective_function(swarm[i].position);

            // Cập nhật pBest
            if (swarm[i].fitness < swarm[i].pBest_val) {
                swarm[i].pBest_val = swarm[i].fitness;
                for(int d=0; d<DIM; d++) swarm[i].pBest_pos[d] = swarm[i].position[d];
            }

            // Cập nhật Global Best (Chỉ để theo dõi kết quả, không dùng tính toán)
            if (swarm[i].fitness < global_best_val) {
                global_best_val = swarm[i].fitness;
                for(int d=0; d<DIM; d++) global_best_pos[d] = swarm[i].position[d];
            }
        }

        if (iter % 10 == 0 || iter == MAX_ITER - 1) {
            printf("N-PSO Iter %d: Current Global Best = %.10f\n", iter, global_best_val);
        }
    }

    printf("\n--- KET QUA CUOI CUNG (N-PSO) ---\n");
    printf("Vi tri toi uu: [%.2f, %.2f]\n", global_best_pos[0], global_best_pos[1]);
    printf("Gia tri nho nhat: %.2f\n", global_best_val);

    return 0;
}