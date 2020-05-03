#ifndef RECT_HPP
#define RECT_HPP

#include "hitable.hpp"
#include "material.hpp"

#define Delta 0.0001

enum class RectNormal {
    FixedPositive,
    FixedNegative,
    Auto
};

class XYRect : public Hitable {
public:
    double _x0, _x1, _y0, _y1, _k;
    Material *_material;
    RectNormal _fixedNormal;
public:
    XYRect() {}

    XYRect(double x0, double x1, double y0, double y1, double k, Material *material,
           RectNormal fixedNormal = RectNormal::Auto) :
            _x0(x0), _x1(x1), _y0(y0), _y1(y1), _k(k), _material(material), _fixedNormal(fixedNormal) {}

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
        switch (this->_fixedNormal) {
            case RectNormal::Auto:
                record.normal = ray.direction().dot(Vector3d{0, 0, 1}) < 0 ?
                                Vector3d{0, 0, 1} : Vector3d{0, 0, -1};
                break;
            case RectNormal::FixedPositive:
                record.normal = Vector3d{0, 0, 1};
                break;
            case RectNormal::FixedNegative:
                record.normal = Vector3d{0, 0, -1};
                break;
        }
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
    RectNormal _fixedNormal;
public:
    XZRect() {}

    XZRect(double x0, double x1, double z0, double z1, double k, Material *material,
           RectNormal fixedNormal = RectNormal::Auto) :
            _x0(x0), _x1(x1), _z0(z0), _z1(z1), _k(k), _material(material), _fixedNormal(fixedNormal) {}

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
        switch (this->_fixedNormal) {
            case RectNormal::Auto:
                record.normal = ray.direction().dot(Vector3d{0, 1, 0}) < 0 ?
                                Vector3d{0, 1, 0} : Vector3d{0, -1, 0};
                break;
            case RectNormal::FixedPositive:
                record.normal = Vector3d{0, 1, 0};
                break;
            case RectNormal::FixedNegative:
                record.normal = Vector3d{0, -1, 0};
                break;
        }
        record.u = (x - this->_x0) / (this->_x1 - this->_x0);
        record.v = (z - this->_z0) / (this->_z1 - this->_z0);
        record.material = this->_material;
        return true;
    };

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        box = AABB({this->_x0, this->_k - Delta, this->_z0}, {this->_x1, this->_k + Delta, this->_z1});
        return true;
    };

    virtual double pdfValue(const Vector3d &o, const Vector3d &direction) const {
        HitRecord record;
        if (this->hit(Ray(o, direction), 0.001, MAXFLOAT, record)) {
            double area = (this->_x1 - this->_x0) * (this->_z1 - this->_z0);
            double distanceSquared = record.t * record.t * direction.squaredNorm();
            double cosine = abs(direction.dot(record.normal) / direction.norm());
            return distanceSquared / (cosine * area);
        } else {
            return 0;
        }
    }

    virtual Vector3d random(const Vector3d &o) {
        Vector3d randomPoint = Vector3d{
                Random::GenUniformRandom(this->_x0, this->_x1),
                this->_k,
                Random::GenUniformRandom(this->_z0, this->_z1)
        };
        return randomPoint - o;
    }
};

class YZRect : public Hitable {
public:
    double _y0, _y1, _z0, _z1, _k;
    Material *_material;
    RectNormal _fixedNormal;
public:
    YZRect() {}

    YZRect(double y0, double y1, double z0, double z1, double k, Material *material,
           RectNormal fixedNormal = RectNormal::Auto) :
            _y0(y0), _y1(y1), _z0(z0), _z1(z1), _k(k), _material(material), _fixedNormal(fixedNormal) {}

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
        switch (this->_fixedNormal) {
            case RectNormal::Auto:
                record.normal = ray.direction().dot(Vector3d{1, 0, 0}) < 0 ?
                                Vector3d{1, 0, 0} : Vector3d{-1, 0, 0};
                break;
            case RectNormal::FixedPositive:
                record.normal = Vector3d{1, 0, 0};
                break;
            case RectNormal::FixedNegative:
                record.normal = Vector3d{-1, 0, 0};
                break;
        }
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
