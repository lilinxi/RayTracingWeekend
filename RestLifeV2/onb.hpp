#ifndef ONB_HPP
#define ONB_HPP

#include "common.hpp"

/**
 * ONB： Ortho-normal Bases 标准正交基
 */
class ONB {
private:
    Vector3d _axis[3];
public:
    ONB() {}

//    以n为一个方向建立直角坐标系
    ONB(const Vector3d &n) {
        this->_axis[2] = n.normalized();
        Vector3d a; // 选取一个与n不平行的a，这里选择坐标轴
        if (abs(this->w().x()) > 0.9) {
            a = Vector3d{0, 1, 0};
        } else {
            a = Vector3d{1, 0, 0};
        }
        this->_axis[1] = this->w().cross(a);
        this->_axis[0] = this->w().cross(this->v());
    }

    inline Vector3d operator[](int i) const {
        return this->_axis[i];
    }

    Vector3d u() const {
        return this->_axis[0];
    }

    Vector3d v() const {
        return this->_axis[1];
    }

    Vector3d w() const {
        return this->_axis[2];
    }

    Vector3d local(double a, double b, double c) const {
        return a * this->u() + b * this->v() + c * this->w();
    }

    Vector3d local(Vector3d a) const{
        return this->local(a.x(), a.y(), a.z());
    }
};


#endif //ONB_HPP
