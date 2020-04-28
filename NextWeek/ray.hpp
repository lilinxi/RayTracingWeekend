#ifndef RAY_HPP
#define RAY_HPP

#include <Eigen/Core>

using Eigen::Vector3d;

class Ray {
private:
    Vector3d _A, _B;
    double _time; // 照相机的快门开关有一定的时间
public:
    Ray() {}

    Ray(const Vector3d &a, const Vector3d &b, double time = 0.0) : _A(a), _B(b.normalized()), _time(time) {}

    Vector3d origin() const {
        return this->_A;
    }

    Vector3d direction() const {
        return this->_B;
    }

    double time() const {
        return this->_time;
    }

    Vector3d operator()(double t) const {
        return this->_A + t * this->_B;
    }
};

#endif //RAY_HPP
