#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "texture.hpp"
#include "hitable.hpp"
#include "onb.hpp"

#define PDF_Epslion 0.0001

class Material {
public:
    /**
     * 散射
     * @param ray           入射光
     * @param record        相交点
     * @param attenuation   衰减
     * @param scattered     散射光
     * @return
     */
    virtual bool
    scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered, double &pdf) const {
        return false;
    };

    virtual double scatterPDF(const Ray &ray, const HitRecord &record, Ray const &scattered) const {
        return 0;
    };

    virtual Vector3d emitted(double u, double v, const Vector3d &p) const {
        return Vector3d{0, 0, 0};
    }

    virtual Vector3d emitted(const Ray &ray, const HitRecord &record, double u, double v, const Vector3d &p) const {
        return Vector3d{0, 0, 0};
    }
};

//Lambertian 反射：理想散射
class Lambertian : public Material {
private:
//    Vector3d _albedo; // 反射率
    Texture *_albedo;
public:
    explicit Lambertian(Texture *albedo) : _albedo(albedo) {}

    virtual bool
    scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered, double &pdf) const {
//        Origin
/*
        attenuation = this->_albedo->value(record.u, record.v, record.p);
        Vector3d target = record.p + record.normal + randomUnitSphere();
        scattered = Ray(record.p, target - record.p, ray.time());
        pdf = record.normal.dot(scattered.direction()) / M_PI;
        return true;
         */
//        只采样上半球
/*
        attenuation = this->_albedo->value(record.u, record.v, record.p);
        Vector3d direction;
        do {
            direction = randomUnitSphere();
        } while (record.normal.dot(direction) < 0);
        scattered = Ray(record.p, direction, ray.time());
        pdf = 0.5 / M_PI;
        */
        attenuation = this->_albedo->value(record.u, record.v, record.p);
        ONB uvw(record.normal);
        do {
            Vector3d direction = uvw.local(randomCosineDirection());
            scattered = Ray(record.p, direction, ray.time());
            pdf = uvw.w().dot(scattered.direction()) / M_PI;
        } while (abs(pdf) < PDF_Epslion);
        return true;
    }

    virtual double scatterPDF(const Ray &ray, const HitRecord &record, Ray const &scattered) const {
        double cosine = record.normal.dot(scattered.direction());
        cosine = cosine < 0 ? 0 : cosine;
        return cosine / M_PI;
    };
};

//out = in + 2B, B=dot(-in,n)*n=--dot(in,n)*n
Vector3d reflect(const Vector3d &v, const Vector3d &n) {
    return (v - 2 * v.dot(n) * n).normalized();
}

////金属模型
//class Metal : public Material {
//private:
//    Vector3d _albedo; // 反射率
//    double _fuzz = 0; // 模糊度参数（=球半径），最大为1，球体越大，反射的模糊度越高；默认无影响
//public:
//    explicit Metal(const Vector3d &albedo, double fuzz = 1) : _albedo(albedo) {
//        if (fuzz < 1) {
//            this->_fuzz = fuzz;
//        } else {
//            this->_fuzz = 1;
//        }
//    }
//
//    virtual bool scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered) const {
//        Vector3d reflected = reflect(ray.direction(), record.normal);
//        scattered = Ray(record.p, reflected + this->_fuzz * randomUnitSphere());
//        attenuation = this->_albedo;
////        cout << scattered.direction().dot(record.normal) << endl;
//        return scattered.direction().dot(record.normal) > 0;
//    }
//};
//
////ni*sin(theta_i)=nt*sin(theta_t)
//bool refract(const Vector3d &v, const Vector3d &n, double niOverNt, Vector3d &refracted) {
//    Vector3d uv = v.normalized();
//    double dt = uv.dot(n); // -cos(theta_i)
//    double discriminant = 1.0 - niOverNt * niOverNt * (1 - dt * dt);
////    sqrt(discriminant)=cos(theta_t)
//    if (discriminant > 0) {
//        refracted = niOverNt * (uv - n * dt) - n * sqrt(discriminant);
//        refracted = refracted.normalized();
//        return true;
//    } else {
//        return false;
//    }
//}
//
////菲涅尔反射
//double schlick(double cosine, double refIdx) {
//    double r0 = (1 - refIdx) / (1 + refIdx);
//    r0 = r0 * r0;
////    lerp: pow((1 - cosine), 5) to 1
//    return (1 - r0) * pow((1 - cosine), 5) + r0;
//}
//
////电解质：折射
//class Dielectric : public Material {
//private:
//    double _redIdx; // 折射率
//public:
//    Dielectric(double redIdx) : _redIdx(redIdx) {}
//
//    virtual bool scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered) const {
//        Vector3d reflected = reflect(ray.direction(), record.normal);
//        attenuation = Vector3d{1.0, 1.0, 1.0};
//        Vector3d outwardNormal;
//        double niOverNt;
//        double cosine;
//        if (ray.direction().dot(record.normal) > 0) {
//            outwardNormal = -record.normal;
//            niOverNt = this->_redIdx;
//            cosine = this->_redIdx * ray.direction().dot(record.normal);
//        } else {
//            outwardNormal = record.normal;
//            niOverNt = 1.0 / this->_redIdx;
//            cosine = -ray.direction().dot(record.normal);
//        }
//        Vector3d refracted;
//        double reflectProb;
//        if (refract(ray.direction(), outwardNormal, niOverNt, refracted)) {
//            reflectProb = schlick(cosine, this->_redIdx);
//        } else {
//            reflectProb = 1.0; // 全反射
//        }
//        if (drand48() < reflectProb) {
//            scattered = Ray(record.p, reflected);
//        } else {
//            scattered = Ray(record.p, refracted);
//        }
//        return true;
//    }
//};

class DiffuseLight : public Material {
private:
    Texture *_emit;
public:
    DiffuseLight(Texture *emit) : _emit(emit) {}

    virtual bool scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered) const {
        return false;
    };

    virtual Vector3d emitted(double u, double v, const Vector3d &p) const {
        return {0, 0, 0};
        return this->_emit->value(u, v, p);
    }

    virtual Vector3d emitted(const Ray &ray, const HitRecord &record, double u, double v, const Vector3d &p) const {
//        天花板周围的灯有噪声，因为灯是双面的，这里删除一个面，只要向下的光
        if (record.normal.dot(ray.direction()) < 0.0) {
            return this->_emit->value(u, v, p);
        } else {
            return {0, 0, 0};
        }
    }
};

#endif //MATERIAL_HPP
