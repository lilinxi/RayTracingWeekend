#ifndef TRANSLATE_HPP
#define TRANSLATE_HPP

#include "hitable.hpp"
#include "debug.hpp"

class Translate : public Hitable {
private:
    Hitable *_hitable;
    Vector3d _offset;
public:
    Translate(Hitable *hitable, const Vector3d &offset) : _hitable(hitable), _offset(offset) {}

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        Ray translateRay(ray.origin() - this->_offset, ray.direction(), ray.time());
        if (!this->_hitable->hit(translateRay, tMin, tMax, record)) {
            return false;
        }
        record.p += this->_offset;
        return true;
    };

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        if (!this->_hitable->boundingBox(time0, time1, box)) {
            return false;
        }
        box = AABB(box.min() + this->_offset, box.max() + this->_offset);
        return true;
    };
};

class RotateY : public Hitable {
private:
    double _sinTheta;
    double _cosTheta;
    Hitable *_hitable;
public:
    RotateY(Hitable *hitable, double angle) : _hitable(hitable) {
        auto radians = degrees_to_radians(angle);
        this->_sinTheta = sin(radians);
        this->_cosTheta = cos(radians);
    }


    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        Vector3d rotatedOrigin = ray.origin();
        Vector3d rotatedDirection = ray.direction();

        rotatedOrigin[0] = this->_cosTheta * ray.origin()[0] - this->_sinTheta * ray.origin()[2];
        rotatedOrigin[2] = this->_sinTheta * ray.origin()[0] + this->_cosTheta * ray.origin()[2];

        rotatedDirection[0] = this->_cosTheta * ray.direction()[0] - this->_sinTheta * ray.direction()[2];
        rotatedDirection[2] = this->_sinTheta * ray.direction()[0] + this->_cosTheta * ray.direction()[2];

        Ray rotatedRay(rotatedOrigin, rotatedDirection, ray.time());

        if (!this->_hitable->hit(rotatedRay, tMin, tMax, record)) {
            return false;
        }

        Vector3d rawP = record.p;
        Vector3d rawNormal = record.normal;

        rawP[0] = this->_cosTheta * record.p[0] + this->_sinTheta * record.p[2];
        rawP[2] = -this->_sinTheta * record.p[0] + this->_cosTheta * record.p[2];

        rawNormal[0] = this->_cosTheta * record.normal[0] + this->_sinTheta * record.normal[2];
        rawNormal[2] = -this->_sinTheta * record.normal[0] + this->_cosTheta * record.normal[2];

        record.p = rawP;
        record.normal = rawNormal;

        return true;
    };

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        if (!this->_hitable->boundingBox(time0, time1, box)) {
            return false;
        }
        Vector3d min(Infinity, Infinity, Infinity);
        Vector3d max(-Infinity, -Infinity, -Infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i * box.max().x() + (1 - i) * box.min().x();
                    auto y = j * box.max().y() + (1 - j) * box.min().y();
                    auto z = k * box.max().z() + (1 - k) * box.min().z();

                    auto newX = this->_cosTheta * x + this->_sinTheta * z;
                    auto newZ = -this->_sinTheta * x + this->_cosTheta * z;

                    Vector3d tester(newX, y, newZ);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::min(min[c], tester[c]);
                        max[c] = std::max(max[c], tester[c]);
                    }
                }
            }
        }

        box = AABB(min, max);
        return true;
    };
};

#endif //TRANSLATE_HPP
