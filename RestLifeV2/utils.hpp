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

void CreateCornellBox(Hitable **scene, Hitable **light, Camera **camera, double aspect) {
//    build scene
    std::vector<Hitable *> list;
    Material *red = new Lambertian(new ConstantTexture({0.65, 0.05, 0.05}));
    Material *white = new Lambertian(new ConstantTexture({0.73, 0.73, 0.73}));
    Material *green = new Lambertian(new ConstantTexture({0.12, 0.45, 0.15}));
    Material *lightMaterial = new DiffuseLight(new ConstantTexture({15, 15, 15}));
    list.push_back(new YZRect(0, 555, 0, 555, 555, green));         // 左墙
    list.push_back(new YZRect(0, 555, 0, 555, 0, red));             // 右墙
//    build light
    *light = new XZRect(213, 343, 227, 332, 554, lightMaterial, RectNormal::FixedNegative);
    list.push_back(*light);     // 顶灯
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

void CreateCornellBoxWithSpecularFace(Hitable **scene, Hitable **light, Camera **camera, double aspect) {
//    build scene
    std::vector<Hitable *> list;
    Material *red = new Lambertian(new ConstantTexture({0.65, 0.05, 0.05}));
    Material *white = new Lambertian(new ConstantTexture({0.73, 0.73, 0.73}));
    Material *green = new Lambertian(new ConstantTexture({0.12, 0.45, 0.15}));
    Material *lightMaterial = new DiffuseLight(new ConstantTexture({15, 15, 15}));
    list.push_back(new YZRect(0, 555, 0, 555, 555, green));         // 左墙
    list.push_back(new YZRect(0, 555, 0, 555, 0, red));             // 右墙
//    build light
    *light = new XZRect(213, 343, 227, 332, 554, lightMaterial, RectNormal::FixedNegative);
    list.push_back(*light);     // 顶灯
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
    Material *aluminum = new Metal(Vector3d{0.8, 0.85, 0.88}, 0.0);
    list.push_back(new Translate(
            new RotateY(
                    new Cube(
                            {0, 0, 0}, {165, 330, 165}, aluminum
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

void CreateCornellBoxWithSpecularSphere(Hitable **scene, Hitable **sampleHitable, Camera **camera, double aspect) {
//    build scene
    std::vector<Hitable *> list;
    Material *red = new Lambertian(new ConstantTexture({0.65, 0.05, 0.05}));
    Material *white = new Lambertian(new ConstantTexture({0.73, 0.73, 0.73}));
    Material *green = new Lambertian(new ConstantTexture({0.12, 0.45, 0.15}));
    Material *lightMaterial = new DiffuseLight(new ConstantTexture({15, 15, 15}));
    list.push_back(new YZRect(0, 555, 0, 555, 555, green));         // 左墙
    list.push_back(new YZRect(0, 555, 0, 555, 0, red));             // 右墙
    list.push_back(new XZRect(213, 343, 227, 332, 554, lightMaterial, RectNormal::FixedNegative));     // 顶灯
    list.push_back(new XZRect(0, 555, 0, 555, 555, white));         // 顶板
    list.push_back(new XZRect(0, 555, 0, 555, 0, white));           // 地板
    list.push_back(new XYRect(0, 555, 0, 555, 555, white));         // 背墙
    Material *glass = new Dielectric(1.5);
    *sampleHitable = new Sphere(Vector3d{190, 90, 190}, 90, glass);
//    build sampleHitable
    list.push_back(*sampleHitable);
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

void
CreateCornellBoxWithSpecularSphereSampleBoth(Hitable **scene, Hitable **sampleHitable, Camera **camera, double aspect) {
//    build scene
    std::vector<Hitable *> list;
    Material *red = new Lambertian(new ConstantTexture({0.65, 0.05, 0.05}));
    Material *white = new Lambertian(new ConstantTexture({0.73, 0.73, 0.73}));
    Material *green = new Lambertian(new ConstantTexture({0.12, 0.45, 0.15}));
    Material *lightMaterial = new DiffuseLight(new ConstantTexture({15, 15, 15}));
    list.push_back(new YZRect(0, 555, 0, 555, 555, green));         // 左墙
    list.push_back(new YZRect(0, 555, 0, 555, 0, red));             // 右墙
    Hitable *light = new XZRect(213, 343, 227, 332, 554, lightMaterial, RectNormal::FixedNegative);
    list.push_back(light);     // 顶灯
    list.push_back(new XZRect(0, 555, 0, 555, 555, white));         // 顶板
    list.push_back(new XZRect(0, 555, 0, 555, 0, white));           // 地板
    list.push_back(new XYRect(0, 555, 0, 555, 555, white));         // 背墙
    Material *glass = new Dielectric(1.5);
    Hitable *glassSphere = new Sphere(Vector3d{190, 90, 190}, 90, glass);
    list.push_back(glassSphere);
//    build sampleHitable
    *sampleHitable = new HitableList({light, glassSphere});
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

Vector3d Color(const Ray &ray, Hitable *world, Hitable *light, int depth) {
    HitRecord hitRecord;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, hitRecord)) {
        ScatterRecord scatterRecord;
        Vector3d emit = hitRecord.material->emitted(ray, hitRecord, hitRecord.u, hitRecord.v, hitRecord.p);
        if (depth < 50 && hitRecord.material->scatter(ray, hitRecord, scatterRecord)) {
            if (scatterRecord.isSpecular) {
                return scatterRecord.attenuation.array() *
                       Color(scatterRecord.specularRay, world, light, depth + 1).array();
            }
            HitablePDF lightPDF(light, hitRecord.p);
            MixturePDF mixturePdf(&lightPDF, scatterRecord.pdf);
            Ray scattered;
            double pdfValue;
            do {
                scattered = Ray(hitRecord.p, mixturePdf.generate(), ray.time());
                pdfValue = mixturePdf.value(scattered.direction());
            } while (pdfValue < PDF_Epslion);
            delete scatterRecord.pdf;
            return emit.array() +
                   scatterRecord.attenuation.array() *
                   hitRecord.material->scatterPDF(ray, hitRecord, scattered) *
                   Color(scattered, world, light, depth + 1).array() /
                   pdfValue;
        } else {
            return emit;
        }
    } else {
        return {0, 0, 0};
    }
}

#endif //UTILS_HPP
