#ifndef PDF_HPP
#define PDF_HPP

#include "common.hpp"
#include "onb.hpp"
#include "random.hpp"

#define PDF_Epslion 0.0001

class PDF {
public:
//    计算生成的 pdf
    virtual double value(const Vector3d &direction) const = 0;

//    概率生成
    virtual Vector3d generate() const = 0;
};

class CosinePDF : public PDF {
private:
    ONB _uvw;
public:
    CosinePDF(const Vector3d &n) {
        this->_uvw = ONB(n);
    }

    virtual double value(const Vector3d &direction) const {
        double cosine = this->_uvw.w().dot(direction);
        if (cosine > 0) {
            return cosine / M_PI;
        } else {
            return 0;
        }
    }

    virtual Vector3d generate() const {
        Vector3d direction;
        do {
            direction = this->_uvw.local(randomCosineDirection());
        } while (this->value(direction) < PDF_Epslion);
        return direction;
    }
};

//可以传入Hitable光源
class HitablePDF : public PDF {
private:
    Hitable *_hitable;
    Vector3d _o;
public:
    HitablePDF(Hitable *hitable, const Vector3d &o) : _hitable(hitable), _o(o) {}

    virtual double value(const Vector3d &direction) const {
        return this->_hitable->pdfValue(this->_o, direction);
    }

    virtual Vector3d generate() const {
        return this->_hitable->random(this->_o);
    }
};

//混合cos pdf和light pdf
class MixturePDF : public PDF {
private:
    PDF *_pdf[2];
public:
    MixturePDF(PDF *p0, PDF *p1) {
        this->_pdf[0] = p0;
        this->_pdf[1] = p1;
    }

    virtual double value(const Vector3d &direction) const {
        return 0.5 * this->_pdf[0]->value(direction) +
               0.5 * this->_pdf[1]->value(direction);
    }

    virtual Vector3d generate() const {
        if (Random::GenUniformRandom(0, 1) < 0.5) {
            return this->_pdf[0]->generate();
        } else {
            return this->_pdf[1]->generate();
        }
    }
};

#endif //PDF_HPP
