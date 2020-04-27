#ifndef HITABLE_HPP
#define HITABLE_HPP

#include <Eigen/Core>
#include <vector>

using Eigen::Vector3d;

class Material;

struct HitRecord {
    double t;
    Vector3d p;
    Vector3d normal;
    Material *material;
};

class Hitable {
public:
    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const = 0;
};

class Sphere : public Hitable {
private:
    Vector3d _center;
    double _radius;
    Material *_material;
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
                return true;
            }
            t = (-b + sqrt(discriminant)) / (2.0 * a);
            if (t < tMax && t > tMin) {
                record.t = t;
                record.p = ray(t);
                record.normal = (record.p - this->_center) / this->_radius;
                record.material = this->_material;
                return true;
            }
        }
        return false;
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
};

#endif //HITABLE_HPP
