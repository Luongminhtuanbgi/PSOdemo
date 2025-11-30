#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// --- CẤU HÌNH THAM SỐ LDIW-PSO ---
#define NUM_PARTICLES 30    // Số lượng hạt
#define MAX_ITER 100        // Số vòng lặp tối đa
#define DIM 2               // Số chiều (x, y)
#define MIN_X -100.0        // Giới hạn không gian tìm kiếm
#define MAX_X 100.0

// Tham số đặc trưng của LDIW
#define W_MAX 0.9           // Quán tính lớn nhất (đầu trận)
#define W_MIN 0.4           // Quán tính nhỏ nhất (cuối trận)
#define C1 2.0              // Hệ số học tập cá nhân
#define C2 2.0              // Hệ số học tập xã hội

// Cấu trúc đại diện cho một hạt
typedef struct {
    double position[DIM];   // Vị trí hiện tại (x, y)
    double velocity[DIM];   // Vận tốc hiện tại (vx, vy)
    double pBest_pos[DIM];  // Vị trí tốt nhất cá nhân từng đi qua
    double pBest_val;       // Giá trị hàm mục tiêu tại pBest
    double fitness;         // Giá trị hàm mục tiêu hiện tại
} Particle;

// Hàm mục tiêu: f(x) = x^2 + y^2 (Tìm cực tiểu về 0)
double objective_function(double pos[]) {
    double sum = 0;
    for (int i = 0; i < DIM; i++) {
        sum += pos[i] * pos[i];
    }
    return sum;
}

// Hàm sinh số ngẫu nhiên trong khoảng [min, max]
double rand_range(double min, double max) {
    return min + (double)rand() / RAND_MAX * (max - min);
}

// Hàm sinh số ngẫu nhiên [0, 1]
double rand_01() {
    return (double)rand() / RAND_MAX;
}

int main() {
    // 1. Khởi tạo ngẫu nhiên
    srand(time(NULL));

    Particle swarm[NUM_PARTICLES];
    double gBest_pos[DIM];
    double gBest_val = 1e15; // Khởi tạo giá trị dương vô cùng lớn

    // Khởi tạo quần thể ban đầu
    for (int i = 0; i < NUM_PARTICLES; i++) {
        for (int d = 0; d < DIM; d++) {
            swarm[i].position[d] = rand_range(MIN_X, MAX_X);
            swarm[i].velocity[d] = rand_range(-1.0, 1.0);
            swarm[i].pBest_pos[d] = swarm[i].position[d];
        }
        
        // Đánh giá lần đầu
        swarm[i].fitness = objective_function(swarm[i].position);
        swarm[i].pBest_val = swarm[i].fitness;

        // Cập nhật gBest toàn đàn
        if (swarm[i].fitness < gBest_val) {
            gBest_val = swarm[i].fitness;
            for (int d = 0; d < DIM; d++) {
                gBest_pos[d] = swarm[i].position[d];
            }
        }
    }

    printf("Bat dau thuat toan LDIW-PSO...\n");
    printf("Trang thai ban dau: gBest = %.10f\n", gBest_val);

    // 2. VÒNG LẶP CHÍNH (MAIN LOOP)
    for (int iter = 0; iter < MAX_ITER; iter++) {
        
        // Tính trọng số quán tính W giảm tuyến tính theo thời gian
        // Tại iter=0, w = 0.9. Tại iter=100, w = 0.4.
        double w_current = W_MAX - ((W_MAX - W_MIN) * iter / MAX_ITER);

        for (int i = 0; i < NUM_PARTICLES; i++) {
            for (int d = 0; d < DIM; d++) {
                double r1 = rand_01();
                double r2 = rand_01();

                // Cập nhật VẬN TỐC (Sử dụng w_current)
                swarm[i].velocity[d] = w_current * swarm[i].velocity[d] 
                                     + C1 * r1 * (swarm[i].pBest_pos[d] - swarm[i].position[d]) 
                                     + C2 * r2 * (gBest_pos[d] - swarm[i].position[d]);

                // Cập nhật VỊ TRÍ
                swarm[i].position[d] += swarm[i].velocity[d];

                // Xử lý biên (Nếu bay ra ngoài thì kéo về biên)
                if (swarm[i].position[d] > MAX_X) swarm[i].position[d] = MAX_X;
                if (swarm[i].position[d] < MIN_X) swarm[i].position[d] = MIN_X;
            }

            // Đánh giá lại hạt tại vị trí mới
            swarm[i].fitness = objective_function(swarm[i].position);

            // Cập nhật pBest (Tốt nhất cá nhân)
            if (swarm[i].fitness < swarm[i].pBest_val) {
                swarm[i].pBest_val = swarm[i].fitness;
                for (int d = 0; d < DIM; d++) {
                    swarm[i].pBest_pos[d] = swarm[i].position[d];
                }
            }

            // Cập nhật gBest (Tốt nhất toàn đàn)
            if (swarm[i].fitness < gBest_val) {
                gBest_val = swarm[i].fitness;
                for (int d = 0; d < DIM; d++) {
                    gBest_pos[d] = swarm[i].position[d];
                }
            }
        }

        // In kết quả mỗi 10 vòng để theo dõi tiến độ
        if (iter % 10 == 0 || iter == MAX_ITER - 1) {
            printf("Iter %d (w = %.4f): gBest Fitness = %.10f\n", iter, w_current, gBest_val);
        }
    }

    // 3. Kết quả cuối cùng
    printf("\n--- KET QUA PSO ---\n");
    printf("Vi tri toi uu tim duoc (x, y): [%.2f, %.2f]\n", gBest_pos[0], gBest_pos[1]);
    printf("Gia tri nho nhat f(x,y): %.2f\n", gBest_val);

    return 0;
}

