#ifndef BOX_HPP
#define BOX_HPP

#include "ray.hpp"

class AABB {
private:
    Vector3d _min, _max;
public:
    static AABB Union(AABB box0, AABB box1) {
        Vector3d min{
                std::min(box0._min.x(), box1._min.x()),
                std::min(box0._min.y(), box1._min.y()),
                std::min(box0._min.z(), box1._min.z())
        };
        Vector3d max{
                std::max(box0._max.x(), box1._max.x()),
                std::max(box0._max.y(), box1._max.y()),
                std::max(box0._max.z(), box1._max.z())
        };
        return AABB{min, max};
    }

    friend std::ostream &operator<<(std::ostream &os, const AABB &aabb) {
        os << "_min: " << aabb._min.transpose() << " _max: " << aabb._max.transpose();
        return os;
    }

public:
    AABB() {}

    AABB(const Vector3d &min, const Vector3d &max) : _min(min), _max(max) {}

    const Vector3d &min() const {
        return _min;
    }

    const Vector3d &max() const {
        return _max;
    }

    bool hit(const Ray &ray, double tMin, double tMax) const {
        for (int i = 0; i < 3; i++) {
            double invDir = 1.0 / ray.direction()[i];
            double t0 = (this->_min[i] - ray.origin()[i]) * invDir;
            double t1 = (this->_max[i] - ray.origin()[i]) * invDir;
            if (invDir < 0.0) {
                std::swap(t0, t1);
            }
            tMin = std::max(t0, tMin);
            tMax = std::min(t1, tMax);
            if (tMax <= tMin) {
                return false;
            }
        }
        return true;
    }
};

#endif //BOX_HPP
