#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <Eigen/Core>
#include <Eigen/Dense>
#include "ray.hpp"

using Eigen::Vector3d;

//Defocus Blur：散焦模糊，光圈随机
Vector3d randomInUnitDisk() {
    Vector3d p;
    do {
//        (0,1)->(-1,1)
        p = 2.0 * Vector3d{drand48(), drand48(), 0} - Vector3d{1, 1, 0};
    } while (p.dot(p) >= 1.0);
    return p;
}

class Camera {
private:
    Vector3d _lower_left_corner{-2.0, -1.0, -1.0};
    Vector3d _horizontal{4.0, 0.0, 0.0};
    Vector3d _vertical{0.0, 2.0, 0.0};
    Vector3d _origin{0.0, 0.0, 0.0};
    Vector3d _u, _v, _w;
    double _lensRadius;

public:
    Camera() {}

    /**
     * 成像平面默认在z=-1上
     * @param vfov      top to bottom in degrees
     * @param aspect    weight / height
     */
    Camera(double vfov, double aspect) {
        double theta = vfov / 180 * M_PI;
        double halfHeight = tan(theta / 2);
        double halfWidth = aspect * halfHeight;
        this->_lower_left_corner = Vector3d{-halfWidth, -halfHeight, -1.0};
        this->_horizontal = Vector3d{2 * halfWidth, 0.0, 0.0};
        this->_vertical = Vector3d{0.0, 2 * halfHeight, 0.0};
        this->_origin = Vector3d{0.0, 0.0, 0.0};
    }

    Camera(Vector3d lookFrom, Vector3d lookAt, Vector3d vUp, double vfov, double aspect) {
        double theta = vfov / 180 * M_PI;
        double halfHeight = tan(theta / 2);
        double halfWidth = aspect * halfHeight;
        this->_origin = lookFrom;
        Vector3d u, v, w;
        w = (lookFrom - lookAt).normalized();
        u = (vUp.cross(w)).normalized();
        v = w.cross(u);
        this->_lower_left_corner = this->_origin - halfWidth * u - halfHeight * v - w;
        this->_horizontal = 2 * halfWidth * u;
        this->_vertical = 2 * halfHeight * v;

        this->_u = u;
        this->_v = v;
        this->_w = w;
    }

    /**
     *
     * @param lookFrom
     * @param lookAt
     * @param vUp
     * @param vfov
     * @param aspect
     * @param aperture  光圈
     * @param focusDist 焦点到光圈的距离 之前默认是 1
     */
    Camera(Vector3d lookFrom, Vector3d lookAt, Vector3d vUp,
           double vfov, double aspect,
           double aperture, double focusDist) {
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

    Ray getRay(double u, double v) {
        return Ray(this->_origin,
                   this->_lower_left_corner +
                   u * this->_horizontal +
                   v * this->_vertical -
                   this->_origin);
    }

    Ray getRayDefocusBlur(double s, double t) {
        Vector3d rd = this->_lensRadius * randomInUnitDisk();
        Vector3d offset = this->_u * rd.x() + this->_v * rd.y();
        return Ray(this->_origin + offset,
                   this->_lower_left_corner +
                   s * this->_horizontal +
                   t * this->_vertical -
                   this->_origin - offset);
    }
};

#endif //CAMERA_HPP
