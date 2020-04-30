//
// Created by limengfan on 2020/4/30.
//

#ifndef CONSTANT_MEDIUM_HPP
#define CONSTANT_MEDIUM_HPP

#include "hitable.hpp"

/**
 * 烟雾模拟
 */
class ConstantMedium : public Hitable {
private:
    Hitable *_hitable;          // 烟雾边界
    Material *_material;        // 相位函数
    double _negInvDensity;      // 强度
public:
    ConstantMedium(Hitable *hitable, Texture *texture, double density) :
            _hitable(hitable), _negInvDensity(-1 / density) {
        this->_material = new Isotropic(texture);
    }


    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {

        HitRecord record1, record2;

        if (!this->_hitable->hit(ray, -Infinity, Infinity, record1)) {
            return false;
        }

        if (!this->_hitable->hit(ray, record1.t + 0.0001, Infinity, record2)) {
            return false;
        }

        if (record1.t < tMin) {
            record1.t = tMin;
        }
        if (record2.t > tMax) {
            record2.t = tMax;
        }

        if (record1.t >= record2.t) {
            return false;
        }

        if (record1.t < 0) {
            record1.t = 0;
        }

        const auto ray_length = ray.direction().norm();
        const auto distance_inside_boundary = (record2.t - record1.t) * ray_length;
        const auto hit_distance = this->_negInvDensity * log(Random::GenUniformRandom(0, 1));

        if (hit_distance > distance_inside_boundary) {
            return false;
        }

        record.t = record1.t + hit_distance / ray_length;
        record.p = ray(record.t);
        record.normal = Vector3d(1, 0, 0);  // arbitrary
        record.material = this->_material;

        return true;
    }

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        return this->_hitable->boundingBox(time0, time1, box);
    }
};

#endif //CONSTANT_MEDIUM_HPP
