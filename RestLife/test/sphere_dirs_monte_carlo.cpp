#include <iostream>
#include "../random.hpp"
#include "../common.hpp"

using namespace std;

/**
 * 蒙特卡洛方法估计theta^2积分，theta∈（0，2pi），​theta​ is the angle with the ​z​ axis
 */
Vector3d RandomUnitSphere() {
    Vector3d p;
    do {
        p = Vector3d{
                Random::GenUniformRandom(0, 1),
                Random::GenUniformRandom(0, 1),
                Random::GenUniformRandom(0, 1)
        };
    } while (p.norm() >= 1.0); // 在空间单位立方体内均匀采样，并去除超过单位球的部分
    return p.normalized();
}

double pdf(Vector3d p) {
    return 1 / (4 * M_PI);
}

int main() {
    int N = 10000;
    double sum = 0;
    for (int i = 0; i < N; i++) {
        Vector3d d = RandomUnitSphere();
        sum += d.z() * d.z() / pdf(d);
    }
    cout << "Real I: " << 4. / 3. * M_PI << endl;
    cout << "Stratified Method I(1): " << sum / N << " distance: " << fabs(sum / N - 4. / 3. * M_PI) << endl;
    return 0;
}