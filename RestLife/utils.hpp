#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include "hitable.hpp"
#include "bvh.hpp"
#include "material.hpp"
#include "rect.hpp"
#include "cube.hpp"
#include "translate.hpp"
#include "pdf.hpp"

void CreateCornellBox(Hitable **scene, Camera **camera, double aspect) {
//    build scene
    std::vector<Hitable *> list;
    Material *red = new Lambertian(new ConstantTexture({0.65, 0.05, 0.05}));
    Material *white = new Lambertian(new ConstantTexture({0.73, 0.73, 0.73}));
    Material *green = new Lambertian(new ConstantTexture({0.12, 0.45, 0.15}));
    Material *light = new DiffuseLight(new ConstantTexture({15, 15, 15}));
    list.push_back(new YZRect(0, 555, 0, 555, 555, green));         // 左墙
    list.push_back(new YZRect(0, 555, 0, 555, 0, red));             // 右墙
    list.push_back(new XZRect(213, 343, 227, 332, 554, light, RectNormal::FixedNegative));     // 顶灯
    list.push_back(new XZRect(0, 555, 0, 555, 555, white));         // 顶板
    list.push_back(new XZRect(0, 555, 0, 555, 0, white));           // 地板
    list.push_back(new XYRect(0, 555, 0, 555, 555, white));         // 背墙
    list.push_back(new Translate(
            new RotateY(
                    new Cube(
                            {0, 0, 0}, {165, 165, 165}, white
                    ), -18
            ), {130, 0, 65}
    ));
    list.push_back(new Translate(
            new RotateY(
                    new Cube(
                            {0, 0, 0}, {165, 330, 165}, white
                    ), 15
            ), {265, 0, 295}
    ));
    *scene = new BVHNode(list, 0, 1);

//    build camera
    Vector3d lookFrom{278, 278, -800};
    Vector3d lookAt{278, 278, 0};
    double distToFocus = 10.0;
    double aperture = 0.0;
    double vfov = 40.0;
    *camera = new Camera(lookFrom, lookAt, {0, 1, 0}, vfov, aspect, aperture, distToFocus, 0.0, 1.0);
}

Vector3d Color(const Ray &ray, Hitable *world, int depth) {
    HitRecord record;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, record)) {
        Vector3d attenuation;
        Ray scattered;
        double pdf;
        Vector3d emit = record.material->emitted(record.u, record.v, record.p);
        if (depth < 50 && record.material->scatter(ray, record, attenuation, scattered, pdf)) {
            return emit.array() +
                   attenuation.array() *
                   record.material->scatterPDF(ray, record, scattered) *
                   Color(scattered, world, depth + 1).array() /
                   pdf;
        } else {
            return emit;
        }
    } else {
        return {0, 0, 0};
    }
}

Vector3d ColorSampleLight(const Ray &ray, Hitable *world, int depth) {
    HitRecord record;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, record)) {
        Vector3d attenuation;
        Ray scattered;
        double pdf;
        Vector3d emit = record.material->emitted(record.u, record.v, record.p);
        if (depth < 50 && record.material->scatter(ray, record, attenuation, scattered, pdf)) {
            Vector3d sampleOnLight = Vector3d{
                    213 + Random::GenUniformRandom(0, 1) * (343 - 213),
                    554,
                    227 + Random::GenUniformRandom(0, 1) * (332 - 227)
            };
            Vector3d directionToLight = sampleOnLight - record.p;
            double distanceSquared = directionToLight.squaredNorm();
            directionToLight.normalize();
//            光源照不到
            if (directionToLight.dot(record.normal) < 0) {
                return emit;
            }
            double lightArea = (343 - 213) * (332 - 227);
            double lightCosine = fabs(directionToLight.y());
//            太小了不行
            if (lightCosine < PDF_Epslion) {
                return emit;
            }
            pdf = distanceSquared / (lightCosine * lightArea);
            scattered = Ray(record.p, directionToLight, ray.time());
            return emit.array() +
                   attenuation.array() *
                   record.material->scatterPDF(ray, record, scattered) *
                   ColorSampleLight(scattered, world, depth + 1).array() /
                   pdf;
        } else {
            return emit;
        }
    } else {
        return {0, 0, 0};
    }
}

Vector3d ColorSampleOneFaceLight(const Ray &ray, Hitable *world, int depth) {
    HitRecord record;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, record)) {
        Vector3d attenuation;
        Ray scattered;
        double pdf;
        Vector3d emit = record.material->emitted(ray, record, record.u, record.v, record.p);
        if (depth < 50 && record.material->scatter(ray, record, attenuation, scattered, pdf)) {
            Vector3d sampleOnLight = Vector3d{
                    213 + Random::GenUniformRandom(0, 1) * (343 - 213),
                    554,
                    227 + Random::GenUniformRandom(0, 1) * (332 - 227)
            };
            Vector3d directionToLight = sampleOnLight - record.p;
            double distanceSquared = directionToLight.squaredNorm();
            directionToLight.normalize();
//            光源照不到
            if (directionToLight.dot(record.normal) < 0) {
                return emit;
            }
            double lightArea = (343 - 213) * (332 - 227);
            double lightCosine = fabs(directionToLight.y());
//            太小了不行
            if (lightCosine < PDF_Epslion) {
                return emit;
            }
            pdf = distanceSquared / (lightCosine * lightArea);
            scattered = Ray(record.p, directionToLight, ray.time());
            return emit.array() +
                   attenuation.array() *
                   record.material->scatterPDF(ray, record, scattered) *
                   ColorSampleOneFaceLight(scattered, world, depth + 1).array() /
                   pdf;
        } else {
            return emit;
        }
    } else {
        return {0, 0, 0};
    }
}

Vector3d ColorUseCosinePDF(const Ray &ray, Hitable *world, int depth) {
    HitRecord record;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, record)) {
        Vector3d attenuation;
        Ray scattered;
        double pdf;
        Vector3d emit = record.material->emitted(ray, record, record.u, record.v, record.p);
        if (depth < 50 && record.material->scatter(ray, record, attenuation, scattered, pdf)) {
            CosinePDF cosinePdf(record.normal);
            scattered = Ray(record.p, cosinePdf.generate(), ray.time());
            pdf = cosinePdf.value(scattered.direction());
            return emit.array() +
                   attenuation.array() *
                   record.material->scatterPDF(ray, record, scattered) *
                   ColorUseCosinePDF(scattered, world, depth + 1).array() /
                   pdf;
        } else {
            return emit;
        }
    } else {
        return {0, 0, 0};
    }
}

Vector3d ColorUseHitablePDF(const Ray &ray, Hitable *world, int depth) {
    HitRecord record;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, record)) {
        Vector3d attenuation;
        Ray scattered;
        double pdf;
        Vector3d emit = record.material->emitted(ray, record, record.u, record.v, record.p);
        if (depth < 50 && record.material->scatter(ray, record, attenuation, scattered, pdf)) {
            Hitable *light = new XZRect(213, 343, 227, 332, 554, 0);
            HitablePDF lightPdf(light, record.p);
            scattered = Ray(record.p, lightPdf.generate(), ray.time());
            pdf = lightPdf.value(scattered.direction());
            return emit.array() +
                   attenuation.array() *
                   record.material->scatterPDF(ray, record, scattered) *
                   ColorUseHitablePDF(scattered, world, depth + 1).array() /
                   pdf;
        } else {
            return emit;
        }
    } else {
        return {0, 0, 0};
    }
}

Vector3d ColorUseMixturePDF(const Ray &ray, Hitable *world, int depth) {
    HitRecord record;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, record)) {
        Vector3d attenuation;
        Ray scattered;
        double pdf;
        Vector3d emit = record.material->emitted(ray, record, record.u, record.v, record.p);
        if (depth < 50 && record.material->scatter(ray, record, attenuation, scattered, pdf)) {
            Hitable *light = new XZRect(213, 343, 227, 332, 554, 0);
            HitablePDF lightPdf(light, record.p);
            CosinePDF cosinePdf(record.normal);
            MixturePDF mixturePdf(&lightPdf, &cosinePdf);
            scattered = Ray(record.p, mixturePdf.generate(), ray.time());
            pdf = mixturePdf.value(scattered.direction());
            return emit.array() +
                   attenuation.array() *
                   record.material->scatterPDF(ray, record, scattered) *
                   ColorUseMixturePDF(scattered, world, depth + 1).array() /
                   pdf;
        } else {
            return emit;
        }
    } else {
        return {0, 0, 0};
    }
}

#endif //UTILS_HPP
