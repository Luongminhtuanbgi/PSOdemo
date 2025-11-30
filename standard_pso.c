#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// --- CẤU HÌNH THAM SỐ ---
#define NUM_PARTICLES 30    // Số lượng hạt
#define MAX_ITER 100        // Số vòng lặp tối đa
#define DIM 2               // Số chiều (x, y)
#define W 0.7               // Trọng số quán tính (Hằng số)
#define C1 1.5              // Hệ số cá nhân
#define C2 1.5              // Hệ số xã hội
#define MIN_X -10.0         // Giới hạn không gian tìm kiếm
#define MAX_X 10.0

// Cấu trúc của một hạt
typedef struct {
    double position[DIM];
    double velocity[DIM];
    double pBest_pos[DIM];
    double pBest_val;
    double fitness;
} Particle;

// Hàm mục tiêu: f(x, y) = x^2 + y^2
double objective_function(double pos[]) {
    return pos[0] * pos[0] + pos[1] * pos[1];
}

// Hàm sinh số ngẫu nhiên [0, 1]
double random_01() {
    return (double)rand() / (double)RAND_MAX;
}

// Hàm sinh số ngẫu nhiên trong khoảng [min, max]
double random_range(double min, double max) {
    return min + random_01() * (max - min);
}

int main() {
    srand(time(NULL)); // Khởi tạo hạt giống ngẫu nhiên

    Particle swarm[NUM_PARTICLES];
    double gBest_pos[DIM];
    double gBest_val = 1e9; // Khởi tạo giá trị dương vô cùng lớn

    // --- 1. KHỞI TẠO ---
    for (int i = 0; i < NUM_PARTICLES; i++) {
        for (int d = 0; d < DIM; d++) {
            swarm[i].position[d] = random_range(MIN_X, MAX_X);
            swarm[i].velocity[d] = random_range(-1.0, 1.0);
            swarm[i].pBest_pos[d] = swarm[i].position[d];
        }
        swarm[i].fitness = objective_function(swarm[i].position);
        swarm[i].pBest_val = swarm[i].fitness;

        // Cập nhật gBest ban đầu
        if (swarm[i].fitness < gBest_val) {
            gBest_val = swarm[i].fitness;
            for (int d = 0; d < DIM; d++) gBest_pos[d] = swarm[i].position[d];
        }
    }

    // --- 2. VÒNG LẶP CHÍNH ---
    for (int iter = 0; iter < MAX_ITER; iter++) {
        for (int i = 0; i < NUM_PARTICLES; i++) {
            // Cập nhật Vận tốc và Vị trí
            for (int d = 0; d < DIM; d++) {
                double r1 = random_01();
                double r2 = random_01();

                // Công thức PSO Gốc
                swarm[i].velocity[d] = W * swarm[i].velocity[d] 
                                     + C1 * r1 * (swarm[i].pBest_pos[d] - swarm[i].position[d]) 
                                     + C2 * r2 * (gBest_pos[d] - swarm[i].position[d]);

                swarm[i].position[d] += swarm[i].velocity[d];

                // Kiểm tra biên (nếu cần thiết thì kẹp giá trị lại)
                if (swarm[i].position[d] < MIN_X) swarm[i].position[d] = MIN_X;
                if (swarm[i].position[d] > MAX_X) swarm[i].position[d] = MAX_X;
            }

            // Đánh giá lại hạt
            swarm[i].fitness = objective_function(swarm[i].position);

            // Cập nhật pBest
            if (swarm[i].fitness < swarm[i].pBest_val) {
                swarm[i].pBest_val = swarm[i].fitness;
                for (int d = 0; d < DIM; d++) swarm[i].pBest_pos[d] = swarm[i].position[d];
            }

            // Cập nhật gBest
            if (swarm[i].fitness < gBest_val) {
                gBest_val = swarm[i].fitness;
                for (int d = 0; d < DIM; d++) gBest_pos[d] = swarm[i].position[d];
            }
        }
        
    }

    printf("\n--- KET QUA PSO  ---\n");
    printf("Vi tri toi uu tim duoc: x = %.5f, y = %.5f\n", gBest_pos[0], gBest_pos[1]);
    printf("Gia tri nho nhat f(x,y) = %.10f\n", gBest_val);

    return 0;
}