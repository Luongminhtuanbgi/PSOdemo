#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// --- CẤU HÌNH THAM SỐ CHUNG ---
#define NUM_PARTICLES 30    // Số lượng hạt
#define MAX_ITER 100        // Số vòng lặp tối đa
#define DIM 2               // Số chiều (x, y)
#define MIN_X -100.0        // Giới hạn tìm kiếm
#define MAX_X 100.0

// --- THAM SỐ ĐẶC TRƯNG CỦA X-PSO (Constriction Factor) ---
// Theo chuẩn của Clerc & Kennedy (2002):
// c1 + c2 phải lớn hơn 4. Thường chọn phi = 4.1
#define C1 2.05
#define C2 2.05

// Cấu trúc hạt
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
    for (int i = 0; i < DIM; i++) {
        sum += pos[i] * pos[i];
    }
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

    // 1. TÍNH TOÁN HỆ SỐ NÉN (CHI) TRƯỚC KHI CHẠY
    // Công thức: Chi = 2 / |2 - phi - sqrt(phi^2 - 4*phi)|
    double phi = C1 + C2; // 4.1
    double chi = 2.0 / fabs(2.0 - phi - sqrt(phi * phi - 4.0 * phi));
    
    // Với C1=C2=2.05, giá trị chi sẽ xấp xỉ 0.7298
    printf("--- KHOI TAO X-PSO ---\n");
    printf("He so phi: %.2f\n", phi);
    printf("He so nen (Chi) da tinh duoc: %.5f\n", chi);

    Particle swarm[NUM_PARTICLES];
    double gBest_pos[DIM];
    double gBest_val = 1e15;

    // 2. KHỞI TẠO QUẦN THỂ
    for (int i = 0; i < NUM_PARTICLES; i++) {
        for (int d = 0; d < DIM; d++) {
            swarm[i].position[d] = rand_range(MIN_X, MAX_X);
            swarm[i].velocity[d] = rand_range(-1.0, 1.0);
            swarm[i].pBest_pos[d] = swarm[i].position[d];
        }
        
        swarm[i].fitness = objective_function(swarm[i].position);
        swarm[i].pBest_val = swarm[i].fitness;

        if (swarm[i].fitness < gBest_val) {
            gBest_val = swarm[i].fitness;
            for (int d = 0; d < DIM; d++) gBest_pos[d] = swarm[i].position[d];
        }
    }

    // 3. VÒNG LẶP CHÍNH
    for (int iter = 0; iter < MAX_ITER; iter++) {
        
        for (int i = 0; i < NUM_PARTICLES; i++) {
            for (int d = 0; d < DIM; d++) {
                double r1 = rand_01();
                double r2 = rand_01();

                //  CÔNG THỨC CẬP NHẬT VẬN TỐC CỦA X-PSO
                
                double cognitive = C1 * r1 * (swarm[i].pBest_pos[d] - swarm[i].position[d]);
                double social    = C2 * r2 * (gBest_pos[d] - swarm[i].position[d]);
                
                swarm[i].velocity[d] = chi * (swarm[i].velocity[d] + cognitive + social);

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

            // Cập nhật gBest
            if (swarm[i].fitness < gBest_val) {
                gBest_val = swarm[i].fitness;
                for(int d=0; d<DIM; d++) gBest_pos[d] = swarm[i].position[d];
            }
        }

        if (iter % 10 == 0 || iter == MAX_ITER - 1) {
            printf("X-PSO Iter %d: gBest Fitness = %.10f\n", iter, gBest_val);
        }
    }

    printf("\n--- KET QUA CUOI CUNG (X-PSO) ---\n");
    printf("Vi tri toi uu: [%.2f, %.2f]\n", gBest_pos[0], gBest_pos[1]);
    printf("Gia tri nho nhat: %.2f\n", gBest_val);

    return 0;
}