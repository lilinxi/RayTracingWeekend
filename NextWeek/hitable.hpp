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
    double u;
    double v;
    Material *material;

    };

class AABB;

class Hitable {
public:
    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const = 0;

    virtual bool boundingBox(double t0, double t1, AABB &box) const = 0;
};

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

    virtual bool boundingBox(double t0, double t1, AABB &box) const {
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

    virtual bool boundingBox(double t0, double t1, AABB &box) const {
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

    virtual bool boundingBox(double t0, double t1, AABB &box) const {
        if (this->_size < 1) {
            return false;
        }
        AABB tempBox;
        if (!this->_list[0]->boundingBox(t0, t1, tempBox)) {
            return false;
        }
        box = tempBox;
        for (int i = 0; i < this->_size; i++) {
            if (this->_list[i]->boundingBox(t0, t1, tempBox)) {
                box = AABB::Union(box, tempBox);
            } else {
                return false;
            }
        }
        return true;
    }
};

class BVHNode : public Hitable {
private:
    AABB _box;
    Hitable *_left;
    Hitable *_right;
public:
    BVHNode() {}

    BVHNode(std::vector<Hitable *> list, double time0, double time1) {
        if (list.size() == 1) {
            this->_left = list[0];
            this->_right = list[0];
        } else if (list.size() == 2) {
            this->_left = list[0];
            this->_right = list[1];
        } else {
            int axis = int(3 * drand48());
            std::sort(list.begin(), list.end(), [axis](Hitable *h1, Hitable *h2) {
                AABB box1, box2;
                bool ret1 = h1->boundingBox(0, 0, box1);
                bool ret2 = h2->boundingBox(0, 0, box2);
                if (!ret1 || !ret2) {
                    std::runtime_error("hitable no bound box");
                }
                return box1.min()[axis] < box2.min()[axis];
            });

            auto beginning = list.begin();
            auto middling = list.begin() + (list.size() / 2);
            auto ending = list.end();

            auto leftList = std::vector<Hitable *>(beginning, middling);
            auto rightList = std::vector<Hitable *>(middling, ending);

            this->_left = new BVHNode(leftList, time0, time1);
            this->_right = new BVHNode(rightList, time0, time1);
        }
        AABB box1, box2;
        bool ret1 = this->_left->boundingBox(0, 0, box1);
        bool ret2 = this->_right->boundingBox(0, 0, box2);
        if (!ret1 || !ret2) {
            std::runtime_error("hitable no bound box");
        }
        this->_box = AABB::Union(box1, box2);
    }

    virtual bool hit(const Ray &ray, double tMin, double tMax, HitRecord &record) const {
        if (this->_box.hit(ray, tMin, tMax)) {
            HitRecord leftRec, rightRec;
            bool hitLeft = this->_left->hit(ray, tMin, tMax, leftRec);
            bool hitRight = this->_right->hit(ray, tMin, tMax, rightRec);
            if (hitLeft && hitRight) {
                record = leftRec.t < rightRec.t ? leftRec : rightRec;
                return true;
            } else if (hitLeft) {
                record = leftRec;
                return true;
            } else if (hitRight) {
                record = rightRec;
                return true;
            }
        }
        return false;
    }

    virtual bool boundingBox(double t0, double t1, AABB &box) const {
        AABB box1, box2;
        bool ret1 = this->_left->boundingBox(t0, t1, box1);
        bool ret2 = this->_right->boundingBox(t0, t1, box2);
        if (!ret1 || !ret2) {
            std::runtime_error("hitable no bound box");
        }
        box = AABB::Union(box1, box2);
        return true;
    };
};

#endif //HITABLE_HPP
