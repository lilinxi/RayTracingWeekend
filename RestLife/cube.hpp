#ifndef CURB_HPP
#define CURB_HPP

#include "rect.hpp"

class Cube : public Hitable {
private:
    Vector3d _min, _max;
    HitableList *_sides;
public:
    Cube() {}

    Cube(const Vector3d &min, const Vector3d &max, Material *material) : _min(min), _max(max) {
        std::vector<Hitable *> list;
        list.push_back(new XYRect(this->_min.x(), this->_max.x(),
                                  this->_min.y(), this->_max.y(),
                                  this->_min.z(), material));
        list.push_back(new XYRect(this->_min.x(), this->_max.x(),
                                  this->_min.y(), this->_max.y(),
                                  this->_max.z(), material));

        list.push_back(new XZRect(this->_min.x(), this->_max.x(),
                                  this->_min.z(), this->_max.z(),
                                  this->_min.y(), material));
        list.push_back(new XZRect(this->_min.x(), this->_max.x(),
                                  this->_min.z(), this->_max.z(),
                                  this->_max.y(), material));

        list.push_back(new YZRect(this->_min.y(), this->_max.y(),
                                  this->_min.z(), this->_max.z(),
                                  this->_min.x(), material));
        list.push_back(new YZRect(this->_min.y(), this->_max.y(),
                                  this->_min.z(), this->_max.z(),
                                  this->_max.x(), material));

        this->_sides = new HitableList(list);
    }

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        return this->_sides->hit(ray, tMin, tMax, record);
    };

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        box = AABB(this->_min, this->_max);
        return true;
    };
};

#endif //CURB_HPP
