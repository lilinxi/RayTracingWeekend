#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <iostream>

using namespace std;

#include <Eigen/Core>

using Eigen::Vector3d;

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
    virtual bool scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered) const = 0;
};

class Texture {
public:
    virtual Vector3d value(double u, double v, const Vector3d &p) const = 0;
};

Vector3d randomUnitSphere() {
    Vector3d p;
    do {
//        (0,1)->(-1,1)
        p = 2 * Vector3d{drand48(), drand48(), drand48()} - Vector3d{1, 1, 1};
    } while (p.norm() >= 1);
    return p;
}

//Lambertian 反射：理想散射
class Lambertian : public Material {
private:
//    Vector3d _albedo; // 反射率
    Texture *_albedo;
public:
    explicit Lambertian(Texture *albedo) : _albedo(albedo) {}

    virtual bool scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered) const {
        Vector3d target = record.p + record.normal + randomUnitSphere();
        scattered = Ray(record.p, target - record.p, ray.time());
        attenuation = this->_albedo->value(record.u, record.v, record.p);
        return true;
    }
};

//out = in + 2B, B=dot(-in,n)*n=--dot(in,n)*n
Vector3d reflect(const Vector3d &v, const Vector3d &n) {
    return (v - 2 * v.dot(n) * n).normalized();
}

//金属模型
class Metal : public Material {
private:
    Vector3d _albedo; // 反射率
    double _fuzz = 0; // 模糊度参数（=球半径），最大为1，球体越大，反射的模糊度越高；默认无影响
public:
    explicit Metal(const Vector3d &albedo, double fuzz = 1) : _albedo(albedo) {
        if (fuzz < 1) {
            this->_fuzz = fuzz;
        } else {
            this->_fuzz = 1;
        }
    }

    virtual bool scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered) const {
        Vector3d reflected = reflect(ray.direction(), record.normal);
        scattered = Ray(record.p, reflected + this->_fuzz * randomUnitSphere());
        attenuation = this->_albedo;
//        cout << scattered.direction().dot(record.normal) << endl;
        return scattered.direction().dot(record.normal) > 0;
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

    virtual bool scatter(const Ray &ray, const HitRecord &record, Vector3d &attenuation, Ray &scattered) const {
        Vector3d reflected = reflect(ray.direction(), record.normal);
        attenuation = Vector3d{1.0, 1.0, 1.0};
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
            scattered = Ray(record.p, reflected);
        } else {
            scattered = Ray(record.p, refracted);
        }
        return true;
    }
};

class ConstantTexture : public Texture {
private:
    Vector3d _color;
public:
    ConstantTexture() {}

    ConstantTexture(const Vector3d &color) : _color(color) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        return this->_color;
    }
};

class CheckerTexture : public Texture {
private:
    Texture *_odd;
    Texture *_even;
public:
    CheckerTexture() {}

    CheckerTexture(Texture *odd, Texture *even) : _odd(odd), _even(even) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        double sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0) {
            return this->_odd->value(u, v, p);
        } else {
            return this->_even->value(u, v, p);
        }
    }
};

class Perlin {
private:
    static double *RanDouble;
    static int *PermX;
    static int *PermY;
    static int *PermZ;

    static double *PerlinGenerate() {
        double *p = new double[256];
        for (int i = 0; i < 256; i++) {
            p[i] = drand48();
        }
        return p;
    }

    static void Permute(int *p, int n) {
        for (int i = n - 1; i > 0; i--) {
            int target = int(drand48() * (i + 1));
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    static int *PerlinGeneratePerm() {
        int *p = new int[256];
        for (int i = 0; i < 256; i++) {
            p[i] = i;
        }
        Perlin::Permute(p, 256);
        return p;
    }

    static double TrilinearInterp(double c[2][2][2], double u, double v, double w) {
        double accum = 0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    accum += (i * u + (1 - i) * (1 - u)) *
                             (j * v + (1 - j) * (1 - v)) *
                             (k * w + (1 - k) * (1 - w)) * c[i][j][k];
                }
            }
        }
        return accum;
    }

public:
    static double Noise(const Vector3d &p) {
        double u = p.x() - floor(p.x());
        double v = p.y() - floor(p.y());
        double w = p.z() - floor(p.z());
//        4: 越大格子越小
        int i = int(4 * p.x()) & 255;
        int j = int(4 * p.y()) & 255;
        int k = int(4 * p.z()) & 255;
        return Perlin::RanDouble[Perlin::PermX[i] ^ Perlin::PermY[j] ^ Perlin::PermZ[k]];
    }

    static double SmoothNoise(const Vector3d &p) {
        double u = p.x() - floor(p.x());
        double v = p.y() - floor(p.y());
        double w = p.z() - floor(p.z());
        u = u * u * ((3 - 2 * u));
        v = v * v * ((3 - 2 * v));
        w = w * w * ((3 - 2 * w));
        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());
        double c[2][2][2];
        for (int di = 0; di < 2; di++) {
            for (int dj = 0; dj < 2; dj++) {
                for (int dk = 0; dk < 2; dk++) {
                    c[di][dj][dk] = Perlin::RanDouble[
                            Perlin::PermX[(i + di) & 255] ^
                            Perlin::PermY[(j + dj) & 255] ^
                            Perlin::PermZ[(k + dk) & 255]];
                }
            }
        }
        return Perlin::TrilinearInterp(c, u, v, w);
    }
};

double *Perlin::RanDouble = Perlin::PerlinGenerate();
int *Perlin::PermX = Perlin::PerlinGeneratePerm();
int *Perlin::PermY = Perlin::PerlinGeneratePerm();
int *Perlin::PermZ = Perlin::PerlinGeneratePerm();

class NoiseTexture : public Texture {
public:
    NoiseTexture() {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        return Vector3d{1, 1, 1} * Perlin::Noise(p);
    }
};

class SmoothNoiseTexture : public Texture {
private:
    double _scale;
public:
    SmoothNoiseTexture(double scale = 1) : _scale(scale) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        return Vector3d{1, 1, 1} * Perlin::SmoothNoise(this->_scale * p);
    }
};

class ImageTexture : public Texture {
private:
    unsigned char *_data;
    int _nx, _ny;
public:
    ImageTexture() {}

    ImageTexture(unsigned char *data, int nx, int ny) : _data(data), _nx(nx), _ny(ny) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        int i = u * this->_nx;
        int j = (1 - v) * this->_ny - 0.001;
        if (i < 0) {
            i = 0;
        }
        if (j < 0) {
            j = 0;
        }
        if (i > this->_nx - 1) {
            i = this->_nx - 1;
        }
        if (j > this->_ny - 1) {
            j = this->_ny - 1;
        }
        double r = int(this->_data[3 * i + 3 * this->_nx * j]) / 255.0;
        double g = int(this->_data[3 * i + 3 * this->_nx * j + 1]) / 255.0;
        double b = int(this->_data[3 * i + 3 * this->_nx * j + 2]) / 255.0;
        return Vector3d{r, g, b};
    }
};

#endif //MATERIAL_HPP
