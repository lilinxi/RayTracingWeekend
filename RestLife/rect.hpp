#ifndef RECT_HPP
#define RECT_HPP

#include "hitable.hpp"
#include "material.hpp"

#define Delta 0.0001

class XYRect : public Hitable {
public:
    double _x0, _x1, _y0, _y1, _k;
    Material *_material;
public:
    XYRect() {}

    XYRect(double x0, double x1, double y0, double y1, double k, Material *material) :
            _x0(x0), _x1(x1), _y0(y0), _y1(y1), _k(k), _material(material) {}

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        auto t = (this->_k - ray.origin().z()) / ray.direction().z();
        if (t < tMin || t > tMax) {
            return false;
        }
        auto x = ray.origin().x() + t * ray.direction().x();
        auto y = ray.origin().y() + t * ray.direction().y();
        if (x < this->_x0 || x > this->_x1 || y < this->_y0 || y > this->_y1) {
            return false;
        }

        record.t = t;
        record.p = ray(t);
        record.normal = ray.direction().dot(Vector3d{0, 0, 1}) < 0 ?
                        Vector3d{0, 0, 1} : Vector3d{0, 0, -1};
        record.u = (x - this->_x0) / (this->_x1 - this->_x0);
        record.v = (y - this->_y0) / (this->_y1 - this->_y0);
        record.material = this->_material;
        return true;
    };

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        box = AABB({this->_x0, this->_y0, this->_k - Delta}, {this->_x1, this->_y1, this->_k + Delta});
        return true;
    };
};

class XZRect : public Hitable {
public:
    double _x0, _x1, _z0, _z1, _k;
    Material *_material;
public:
    XZRect() {}

    XZRect(double x0, double x1, double z0, double z1, double k, Material *material) :
            _x0(x0), _x1(x1), _z0(z0), _z1(z1), _k(k), _material(material) {}

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        auto t = (this->_k - ray.origin().y()) / ray.direction().y();
        if (t < tMin || t > tMax) {
            return false;
        }

        auto x = ray.origin().x() + t * ray.direction().x();
        auto z = ray.origin().z() + t * ray.direction().z();
        if (x < this->_x0 || x > this->_x1 || z < this->_z0 || z > this->_z1) {
            return false;
        }

        record.t = t;
        record.p = ray(t);
        record.normal = ray.direction().dot(Vector3d{0, 1, 0}) < 0 ?
                        Vector3d{0, 1, 0} : Vector3d{0, -1, 0};
        record.u = (x - this->_x0) / (this->_x1 - this->_x0);
        record.v = (z - this->_z0) / (this->_z1 - this->_z0);
        record.material = this->_material;
        return true;
    };

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        box = AABB({this->_x0, this->_k - Delta, this->_z0}, {this->_x1, this->_k + Delta, this->_z1});
        return true;
    };
};

class YZRect : public Hitable {
public:
    double _y0, _y1, _z0, _z1, _k;
    Material *_material;
public:
    YZRect() {}

    YZRect(double y0, double y1, double z0, double z1, double k, Material *material) :
            _y0(y0), _y1(y1), _z0(z0), _z1(z1), _k(k), _material(material) {}

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        auto t = (this->_k - ray.origin().x()) / ray.direction().x();
        if (t < tMin || t > tMax) {
            return false;
        }

        auto y = ray.origin().y() + t * ray.direction().y();
        auto z = ray.origin().z() + t * ray.direction().z();
        if (y < this->_y0 || y > this->_y1 || z < this->_z0 || z > this->_z1) {
            return false;
        }

        record.t = t;
        record.p = ray(t);
        record.normal = ray.direction().dot(Vector3d{1, 0, 0}) < 0 ?
                        Vector3d{1, 0, 0} : Vector3d{-1, 0, 0};
        record.u = (y - this->_y0) / (this->_y1 - this->_y0);
        record.v = (z - this->_z0) / (this->_z1 - this->_z0);
        record.material = this->_material;
        return true;
    };

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        box = AABB({this->_k - Delta, this->_y0, this->_z0}, {this->_k + Delta, this->_y1, this->_z1});
        return true;
    };
};

#endif //RECT_HPP
