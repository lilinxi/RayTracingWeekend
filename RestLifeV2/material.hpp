#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "texture.hpp"
#include "hitable.hpp"
#include "onb.hpp"
#include "pdf.hpp"

struct ScatterRecord {
    Ray specularRay;
    bool isSpecular;        // 只是改变光的方向，而本身并没有颜色，也就没有散射光，不需要pdf
    Vector3d attenuation;   // 衰减
    PDF *pdf;               // 散射光的 pdf
};

class Material {
public:
    virtual bool scatter(const Ray &ray, const HitRecord &hitRecord, ScatterRecord &scatterRecord) const {
        return false;
    };

    virtual double scatterPDF(const Ray &ray, const HitRecord &hitRecord, Ray const &scattered) const {
        return 0;
    };

    virtual Vector3d emitted(const Ray &ray, const HitRecord &hitRecord, double u, double v, const Vector3d &p) const {
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

    virtual bool scatter(const Ray &ray, const HitRecord &hitRecord, ScatterRecord &scatterRecord) const {
        scatterRecord.isSpecular = false;
        scatterRecord.attenuation = this->_albedo->value(hitRecord.u, hitRecord.v, hitRecord.p);
        scatterRecord.pdf = new CosinePDF(hitRecord.normal);
        return true;
    };

//    均匀分布的概率pdf（先乘均匀分布的概率pdf，再除个性化采样的概率pdf）
    virtual double scatterPDF(const Ray &ray, const HitRecord &hitRecord, Ray const &scattered) const {
        double cosine = hitRecord.normal.dot(scattered.direction());
        cosine = cosine < 0 ? 0 : cosine;
        return cosine / M_PI;
    };
};

//out = in + 2B, B=dot(-in,n)*n=--dot(in,n)*n
Vector3d reflect(const Vector3d &v, const Vector3d &n) {
    return (v - 2 * v.dot(n) * n).normalized();
}

////金属模型
class Metal : public Material {
private:
    Vector3d _albedo; // 反射率
    double _fuzz = 0; // 模糊度参数（=反射偏移的球半径），最大为1，球体越大，反射的模糊度越高，这时并不再是镜面反射；默认无影响
public:
    explicit Metal(const Vector3d &albedo, double fuzz = 0) : _albedo(albedo) {
        if (fuzz < 1) {
            this->_fuzz = fuzz;
        } else {
            this->_fuzz = 1;
        }
    }

    virtual bool scatter(const Ray &ray, const HitRecord &record,  ScatterRecord &scatterRecord) const {
        Vector3d reflected = reflect(ray.direction(), record.normal);
        scatterRecord.specularRay = Ray(record.p, reflected + this->_fuzz * randomUnitSphere());
        scatterRecord.isSpecular = true;
        scatterRecord.attenuation = this->_albedo;
        scatterRecord.pdf = nullptr;
        return true;
    }
};

//ni*sin(theta_i)=nt*sin(theta_t)
bool refract(const Vector3d &v, const Vector3d &n, double niOverNt, Vector3d &refracted) {
    Vector3d uv = v.normalized();
    double dt = uv.dot(n); // -cos(theta_i)
    double discriminant = 1.0 - niOverNt * niOverNt * (1 - dt * dt);
//    sqrt(discriminant)=cos(theta_t)
    if (discriminant > 0) {
        refracted = niOverNt * (uv - n * dt) - n * sqrt(discriminant);
        refracted = refracted.normalized();
        return true;
    } else {
        return false;
    }
}

//菲涅尔反射
double schlick(double cosine, double refIdx) {
    double r0 = (1 - refIdx) / (1 + refIdx);
    r0 = r0 * r0;
//    lerp: pow((1 - cosine), 5) to 1
    return (1 - r0) * pow((1 - cosine), 5) + r0;
}

//电解质：折射
class Dielectric : public Material {
private:
    double _redIdx; // 折射率
public:
    Dielectric(double redIdx) : _redIdx(redIdx) {}

    virtual bool scatter(const Ray &ray, const HitRecord &record, ScatterRecord &scatterRecord) const {
        scatterRecord.isSpecular = true;
        scatterRecord.pdf = nullptr;
        scatterRecord.attenuation = Vector3d (1.0, 1.0, 1.0);
        Vector3d reflected = reflect(ray.direction(), record.normal);
        Vector3d outwardNormal;
        double niOverNt;
        double cosine;
        if (ray.direction().dot(record.normal) > 0) {
            outwardNormal = -record.normal;
            niOverNt = this->_redIdx;
            cosine = this->_redIdx * ray.direction().dot(record.normal);
        } else {
            outwardNormal = record.normal;
            niOverNt = 1.0 / this->_redIdx;
            cosine = -ray.direction().dot(record.normal);
        }
        Vector3d refracted;
        double reflectProb;
        if (refract(ray.direction(), outwardNormal, niOverNt, refracted)) {
            reflectProb = schlick(cosine, this->_redIdx);
        } else {
            reflectProb = 1.0; // 全反射
        }
        if (drand48() < reflectProb) {
            scatterRecord.specularRay = Ray(record.p, reflected);
        } else {
            scatterRecord.specularRay  = Ray(record.p, refracted);
        }
        return true;
    }
};

class DiffuseLight : public Material {
private:
    Texture *_emit;
public:
    DiffuseLight(Texture *emit) : _emit(emit) {}

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
