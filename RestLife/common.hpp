#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <limits>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "random.hpp"

using Eigen::Vector3d;

const double Infinity = std::numeric_limits<double>::infinity();

Vector3d lerp(double t, Vector3d start, Vector3d end) {
    return (1.0 - t) * start + t * end;
}

void UpdateProgress(double progress) {
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
//    \r：回车不换行
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

Vector3d randomUnitSphere() {
    Vector3d p;
    do {
//        (0,1)->(-1,1)
        p = 2.0 * Vector3d{drand48(), drand48(), drand48()} - Vector3d{1, 1, 1};
    } while (p.norm() >= 1.0);
    return p;
}

Vector3d randomUnitDisk() {
    Vector3d p;
    do {
//        (0,1)->(-1,1)
        p = 2.0 * Vector3d{drand48(), drand48(), 0} - Vector3d{1, 1, 0};
    } while (p.dot(p) >= 1.0);
    return p;
}

Vector3d randomCosineDirection() {
    double r1 = Random::GenUniformRandom(0, 1);
    double r2 = Random::GenUniformRandom(0, 1);
    double theta = sqrt(1 - r2);
    double phi = 2 * M_PI * r1;
    double x = cos(phi) * 2 * sqrt(r2);
    double y = sin(phi) * 2 * sqrt(r2);
    double z = theta;
    return {x, y, z};
}

inline double degrees_to_radians(double degrees) {
    return degrees * M_PI / 180.0;
}

#endif //COMMON_HPP
