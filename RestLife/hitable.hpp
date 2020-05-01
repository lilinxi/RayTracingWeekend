#ifndef HITABLE_HPP
#define HITABLE_HPP

#include <vector>
#include "box.hpp"

class Material;

struct HitRecord {
    double t;               // 光线的t
    Vector3d p;             // 交点
    Vector3d normal;        // 交点法线
    double u, v;            // 交点纹理坐标
    Material *material;     // 交点材质

    friend std::ostream &operator<<(std::ostream &os, const HitRecord &record) {
        os << "t: " << record.t << " p: " << record.p.transpose() << " normal: " << record.normal.transpose();
        return os;
    }
};

class Hitable {
public:
    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const = 0;

    virtual bool boundingBox(double time0, double time1, AABB &box) const = 0;
};

class Sphere : public Hitable {
private:
    Vector3d _center;
    double _radius;
    Material *_material;

    static void GetSphereUV(const Vector3d &p, double &u, double &v) {
        double phi = atan2(p.z(), p.x());
        double theta = asin(p.y());
        u = 1 - (phi + M_PI) / (2 * M_PI);
        v = (theta + M_PI / 2) / M_PI;
    }

public:
    Sphere() {}

    Sphere(const Vector3d &center, double radius) : _center(center), _radius(radius) {}

    Sphere(const Vector3d &center, double radius, Material *material) :
            _center(center), _radius(radius), _material(material) {}

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        //t*t*B*B+2*t*B*(A-C)+(A-C)*(A-C)-R*R=0
        Vector3d oc = ray.origin() - this->_center;
        double a = ray.direction().dot(ray.direction());
        double b = 2.0 * oc.dot(ray.direction());
        double c = oc.dot(oc) - this->_radius * this->_radius;
        double discriminant = b * b - 4 * a * c;
        if (discriminant > 0) {
            double t = (-b - sqrt(discriminant)) / (2.0 * a);
            if (t < tMax && t > tMin) {
                record.t = t;
                record.p = ray(t);
                record.normal = (record.p - this->_center) / this->_radius;
                record.material = this->_material;
                Sphere::GetSphereUV((record.p - this->_center) / this->_radius, record.u, record.v);
                return true;
            }
            t = (-b + sqrt(discriminant)) / (2.0 * a);
            if (t < tMax && t > tMin) {
                record.t = t;
                record.p = ray(t);
                record.normal = (record.p - this->_center) / this->_radius;
                record.material = this->_material;
                Sphere::GetSphereUV((record.p - this->_center) / this->_radius, record.u, record.v);
                return true;
            }
        }
        return false;
    }

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        box = AABB(this->_center - Vector3d{this->_radius, this->_radius, this->_radius},
                   this->_center + Vector3d{this->_radius, this->_radius, this->_radius});
        return true;
    }
};

class MovingSphere : public Hitable {
private:
    Vector3d _center0, _center1;
    double _time0, _time1;
    double _radius;
    Material *_material;
public:
    MovingSphere() {}

    MovingSphere(const Vector3d &center0, const Vector3d &center1,
                 double time0, double time1,
                 double radius, Material *material) :
            _center0(center0), _center1(center1),
            _time0(time0), _time1(time1),
            _radius(radius), _material(material) {}

    Vector3d center(double time) const {
        return this->_center0 +
               (time - this->_time0) / (this->_time1 - this->_time0) * (this->_center1 - this->_center0);
    }

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        //t*t*B*B+2*t*B*(A-C)+(A-C)*(A-C)-R*R=0
        Vector3d center = this->center(ray.time());
        Vector3d oc = ray.origin() - center;
        double a = ray.direction().dot(ray.direction());
        double b = 2.0 * oc.dot(ray.direction());
        double c = oc.dot(oc) - this->_radius * this->_radius;
        double discriminant = b * b - 4 * a * c;
        if (discriminant > 0) {
            double t = (-b - sqrt(discriminant)) / (2.0 * a);
            if (t < tMax && t > tMin) {
                record.t = t;
                record.p = ray(t);
                record.normal = (record.p - center) / this->_radius;
                record.material = this->_material;
                return true;
            }
            t = (-b + sqrt(discriminant)) / (2.0 * a);
            if (t < tMax && t > tMin) {
                record.t = t;
                record.p = ray(t);
                record.normal = (record.p - center) / this->_radius;
                record.material = this->_material;
                return true;
            }
        }
        return false;
    }

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        AABB box0(
                this->center(time0) - Vector3d{this->_radius, this->_radius, this->_radius},
                this->center(time0) + Vector3d{this->_radius, this->_radius, this->_radius});
        AABB box1(
                this->center(time1) - Vector3d{this->_radius, this->_radius, this->_radius},
                this->center(time1) + Vector3d{this->_radius, this->_radius, this->_radius});
        box = AABB::Union(box0, box1);
        return true;
    }
};

class HitableList : public Hitable {
private:
    std::vector<Hitable *> _list;
    int _size;
public:
    HitableList() {}

    HitableList(const std::vector<Hitable *> &list) : _list(list) {
        this->_size = list.size();
    }

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        HitRecord temp;
        bool hit = false;
        double closest = tMax;
        for (const Hitable *hitable:this->_list) {
            if (hitable->hit(ray, tMin, tMax, temp)) {
                hit = true;
                if (closest > temp.t) {
                    closest = temp.t;
                    record = temp;
                }
            }
        }
        return hit;
    }

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        if (this->_size < 1) {
            return false;
        }
        AABB tempBox;
        if (!this->_list[0]->boundingBox(time0, time1, tempBox)) {
            return false;
        }
        box = tempBox;
        for (int i = 0; i < this->_size; i++) {
            if (this->_list[i]->boundingBox(time0, time1, tempBox)) {
                box = AABB::Union(box, tempBox);
            } else {
                return false;
            }
        }
        return true;
    }
};


#endif //HITABLE_HPP
