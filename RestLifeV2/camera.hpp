#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "common.hpp"
#include "ray.hpp"

class Camera {
private:
    Vector3d _lower_left_corner{-2.0, -1.0, -1.0};
    Vector3d _horizontal{4.0, 0.0, 0.0};
    Vector3d _vertical{0.0, 2.0, 0.0};
    Vector3d _origin{0.0, 0.0, 0.0};
    Vector3d _u, _v, _w;
    double _lensRadius;
    double _time0, _time1;

public:
    /**
     *
     * @param lookFrom
     * @param lookAt
     * @param vUp
     * @param vfov
     * @param aspect
     * @param aperture  光圈（光圈越大越模糊）
     * @param focusDist 焦点到光圈的距离 之前默认是 1（聚焦平面在哪里）
     */
    Camera(Vector3d lookFrom, Vector3d lookAt, Vector3d vUp,
           double vfov, double aspect,
           double aperture, double focusDist,
           double time0, double time1) : _time0(time0), _time1(time1) {
        double theta = vfov / 180 * M_PI;
        double halfHeight = tan(theta / 2);
        double halfWidth = aspect * halfHeight;
        this->_origin = lookFrom;
        Vector3d u, v, w;
        w = (lookFrom - lookAt).normalized();
        u = (vUp.cross(w)).normalized();
        v = w.cross(u);
        this->_lower_left_corner =
                this->_origin - halfWidth * focusDist * u - halfHeight * focusDist * v - focusDist * w;
        this->_horizontal = 2 * halfWidth * focusDist * u;
        this->_vertical = 2 * halfHeight * focusDist * v;

        this->_u = u;
        this->_v = v;
        this->_w = w;
        this->_lensRadius = aperture / 2;
    }

//    返回的光线时间为快门开启时间段内的随机时间
    Ray getRay(double s, double t) {
//        Defocus Blur：散焦模糊，光圈随机
        Vector3d rd = this->_lensRadius * randomUnitDisk();
        Vector3d offset = this->_u * rd.x() + this->_v * rd.y();
        double time = this->_time0 + drand48() * (this->_time1 - this->_time0);
        return Ray(this->_origin + offset,
                   this->_lower_left_corner +
                   s * this->_horizontal +
                   t * this->_vertical -
                   this->_origin - offset,
                   time);
    }
};

#endif //CAMERA_HPP
