#include "../random.hpp"
#include "../common.hpp"
#include "../debug.hpp"

/**
 * 生成均匀分布的随机方向
 * 验证：cos（theta）^3在半球的积分=PI/2
 */

Vector3d RandomCosineDirection() {
    double r1 = Random::GenUniformRandom(0, 1);
    double r2 = Random::GenUniformRandom(0, 1);
    double theta = sqrt(1 - r2);
    double phi = 2 * M_PI * r1;
    double x = cos(phi) * 2 * sqrt(r2);
    double y = sin(phi) * 2 * sqrt(r2);
    double z = theta;
    return {x, y, z};
}

int main() {
    int N = 1000000;
    double sum = 0;
    for (int i = 0; i < N; i++) {
        Vector3d v = RandomCosineDirection();
        if (v.z() == 0) {
            i--;
            continue;
        }
        sum += v.z() * v.z() * v.z() / (v.z() / M_PI);
    }
    cout << "PI/2 = " << M_PI / 2 << endl;
    cout << "I = " << sum / N << endl;
}