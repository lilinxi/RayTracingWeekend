#ifndef RAY_HPP
#define RAY_HPP

#include <Eigen/Core>

using Eigen::Vector3d;

class Ray {
private:
    Vector3d _A, _B;
public:
    Ray() {}

    Ray(const Vector3d &a, const Vector3d &b) : _A(a), _B(b.normalized()) {}

    Vector3d origin() const {
        return this->_A;
    }

    Vector3d direction() const {
        return this->_B;
    }

    Vector3d operator()(double t) const{
        return this->_A + t * this->_B;
    }
};

#endif //RAY_HPP
