#include "../random.hpp"
#include "../common.hpp"
#include "../debug.hpp"

/**
 * 生成均匀分布的随机方向
 * 验证：cos（theta）^3在半球的积分=PI/2
 */

int main() {
    int N = 1000000;
    double sum = 0;
    for (int i = 0; i < N; i++) {
        double r1 = Random::GenUniformRandom(0, 1);
        double r2 = Random::GenUniformRandom(0, 1);
        double x = cos(2 * M_PI * r1) * 2 * sqrt(r2 * (1 - r2));
        double y = sin(2 * M_PI * r1) * 2 * sqrt(r2 * (1 - r2));
        double z = 1 - 2 * r2; // z = cos（theta）
        Vector3d v = Vector3d{x, y, z};
//        cout << v.transpose() << " : " << v.norm() << endl;
        z = 1 - r2; // 按新的验证分布采样
//        double osum = sum;
        sum += z * z * z / (1.0 / (2.0 * M_PI)); // pdf = 1.0 / (2.0 * M_PI)
//        cout << osum << "+= " << z * z * z / (1.0 / (2.0 * M_PI)) << " = " << sum << endl;
    }
    cout << "PI/2 = " << M_PI / 2 << endl;
    cout << "I = " << sum / N << endl;
}