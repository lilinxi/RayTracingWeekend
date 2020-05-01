#include <iostream>
#include "../random.hpp"

using namespace std;

/**
 * 蒙特卡洛方法估计x^2积分（0~2）
 * 三种不同的重要性采样：以三个不同函数的累积概率密度函数生成随机变量，最后除以概率
 * 其中pdf3，以原始函数作为概率函数进行的重要性采样效果最好
 */
double pdf1(double x) {
    return 0.5 * x;
}

double pdf2(double x) {
    return 0.5;
}

double pdf3(double x) {
    return 3 * x * x / 8;
}

#define Epsilon 0.00001

int main() {
    int N = 10000;
    double sum1 = 0;
    double sum2 = 0;
    double sum3 = 0;
    for (int i = 0; i < N; i++) {
        double x = sqrt(Random::GenUniformRandom(0, 4));
        double pdfx1 = pdf1(x);
        if (pdfx1 < Epsilon) {
            N--;
            continue;
        }
        double fx1 = x * x;

        x = Random::GenUniformRandom(0, 2);
        double pdfx2 = pdf2(x);
        if (pdfx2 < Epsilon) {
            N--;
            continue;
        }
        double fx2 = x * x;

        x = pow(Random::GenUniformRandom(0, 8), 1. / 3.);
        double pdfx3 = pdf3(x);
        if (pdfx3 < Epsilon) {
            N--;
            continue;
        }
        double fx3 = x * x;

        sum1 += fx1 / pdfx1;
        sum2 += fx2 / pdfx2;
        sum3 += fx3 / pdfx3;
    }
    cout << "Real I: " << 8. / 3. << endl;
    cout << "Stratified Method I(1): " << sum1 / N << " distance: " << fabs(sum1 / N - 8. / 3.) << endl;
    cout << "Stratified Method I(2): " << sum2 / N << " distance: " << fabs(sum2 / N - 8. / 3.) << endl;
    cout << "Stratified Method I(3): " << sum3 / N << " distance: " << fabs(sum3 / N - 8. / 3.) << endl;
    return 0;
}