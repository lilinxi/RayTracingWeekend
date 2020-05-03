#ifndef BVH_HPP
#define BVH_HPP

#include "hitable.hpp"

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
//            随机一个维度排序
            int axis = int(3 * drand48());
            std::sort(list.begin(), list.end(), [axis,time0,time1](Hitable *h1, Hitable *h2) {
                AABB box1, box2;
                bool ret1 = h1->boundingBox(time0, time1, box1);
                bool ret2 = h2->boundingBox(time0, time1, box2);
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
        bool ret1 = this->_left->boundingBox(time0, time1, box1);
        bool ret2 = this->_right->boundingBox(time0, time1, box2);
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

    virtual bool boundingBox(double time0, double time1, AABB &box) const {
        box = this->_box;
        return true;
    };
};

#endif //BVH_HPP
