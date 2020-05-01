#include <iostream>
#include "../random.hpp"

using namespace std;

/**
 * 蒙特卡洛方法估计圆周率
 * 通过对样本进行分层来减轻这种递减收益，此法通常称为抖动
 */
int main() {
    int sqrtN = 10000;
    int insideCircleRegular = 0;
    int insideCircleStratified = 0;
    for (int i = 0; i < sqrtN; i++) {
        for (int j = 0; j < sqrtN; j++) {
            double x = Random::GenUniformRandom(-1, 1);
            double y = Random::GenUniformRandom(-1, 1);
            if (x * x + y * y < 1) {
                insideCircleRegular++;
            }
            x = Random::GenUniformRandom(double(i) / sqrtN, double(i + 1) / sqrtN);
            y = Random::GenUniformRandom(double(j) / sqrtN, double(j + 1) / sqrtN);
            if (x * x + y * y < 1) {
                insideCircleStratified++;
            }
        }
    }
    cout << "Regular Method Pi: " << 4 * double(insideCircleRegular) / (sqrtN * sqrtN) << endl;
    cout << "Stratified Method Pi: " << 4 * double(insideCircleStratified) / (sqrtN * sqrtN) << endl;
    return 0;
}