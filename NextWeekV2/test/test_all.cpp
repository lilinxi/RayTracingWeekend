#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "../camera.hpp"
#include "../ray.hpp"
#include "../hitable.hpp"
#include "../bvh.hpp"
#include "../material.hpp"
#include "../rect.hpp"
#include "../cube.hpp"
#include "../translate.hpp"
#include "../constant_medium.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

using namespace std;

Vector3d color(const Ray &ray, Hitable *world, int depth) {
    HitRecord record;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, record)) {
        Ray scattered;
        Vector3d attenuation;
        Vector3d emit = record.material->emitted(record.u, record.v, record.p);
        if (depth < 50 && record.material->scatter(ray, record, attenuation, scattered)) {
            return emit.array() + attenuation.array() * color(scattered, world, depth + 1).array();
        } else {
            return emit;
        }
    } else {
        return {0, 0, 0};
    }
}

Hitable *cornellBox() {
    vector<Hitable *> list;
//    灯
    Material *light = new DiffuseLight(new ConstantTexture(Vector3d(7, 7, 7)));
    list.push_back(new XZRect(123, 423, 147, 412, 554, light));
//    地面
    Material *ground = new Lambertian(new ConstantTexture(Vector3d(0.48, 0.83, 0.53)));
    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = Random::GenUniformRandom(1, 101);
            auto z1 = z0 + w;
            list.push_back(new Cube(Vector3d{x0, y0, z0}, Vector3d{x1, y1, z1}, ground));
        }
    }
//    移动的球
    Vector3d center1 = Vector3d(400, 400, 200);
    Vector3d center2 = center1 + Vector3d(30, 0, 0);
    Material *movingSphereMaterial = new Lambertian(new ConstantTexture(Vector3d(0.7, 0.3, 0.1)));
    list.push_back(new MovingSphere(center1, center2, 0, 1, 50, movingSphereMaterial));
//    玻璃球
    list.push_back(new Sphere(Vector3d(260, 150, 45), 50, new Dielectric(1.5)));
//    金属球
    list.push_back(new Sphere(Vector3d(0, 150, 145), 50, new Metal(Vector3d(0.8, 0.8, 0.9), 10.0)));
//    烟雾效果出来的水晶球？
    Hitable *boundary = new Sphere(Vector3d(360, 150, 145), 70, new Dielectric(1.5));

    list.push_back(new ConstantMedium(
            boundary, new ConstantTexture(Vector3d(0.2, 0.4, 0.9)), 0.2
    ));
//    整个空间烟雾效果
    boundary = new Sphere(Vector3d(0, 0, 0), 5000, new Dielectric(1.5));
    list.push_back(new ConstantMedium(boundary, new ConstantTexture(Vector3d(1, 1, 1)), 0.0001));
//    地球
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("../../img/earthmap.jpg", &nx, &ny, &nn, 0);
    Material *emat = new Lambertian(new ImageTexture(tex_data, nx, ny));
    list.push_back(new Sphere(Vector3d(400, 200, 400), 100, emat));
//    大理石球
    Texture *pertext = new TurbNoiseTexture2(0.1);
    list.push_back(new Sphere(Vector3d(220, 280, 300), 80, new Lambertian(pertext)));
//    球球组成的盒子
    vector<Hitable *> boxes;
    Material *white = new Lambertian(new ConstantTexture(Vector3d(0.73, 0.73, 0.73)));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes.push_back(new Sphere(Vector3d(
                Random::GenUniformRandom(0, 165),
                Random::GenUniformRandom(0, 165),
                Random::GenUniformRandom(0, 165)
        ), 10, white));
    }
    list.push_back(new Translate(new RotateY(new BVHNode(boxes, 0, 1), 15), Vector3d(-100, 270, 395)));

    auto start = std::chrono::system_clock::now();
    Hitable *bvh = new BVHNode(list, 0, 1);
    auto stop = std::chrono::system_clock::now();
    std::cout << "build bvh cost: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
              << " seconds\n";

//    return new HitableList(list);
    return bvh;
}

int main() {
    auto start = std::chrono::system_clock::now();
    int nx = 400;
    int ny = 200;
    int ns = 100;
//    nx = 1080;
//    ny = 720;
//    ns = 100;
    nx = 4000;
    ny = 4000;
    ns = 200;
    ofstream fout("test_all.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    Hitable *world = cornellBox();
    Vector3d lookFrom{478, 278, -600};
    Vector3d lookAt{278, 278, 0};
    double distToFocus = 10.0;
    double aperture = 0.0;
    Camera camera(lookFrom, lookAt, {0, 1, 0}, 40, double(nx) / double(ny), aperture, distToFocus, 0.0, 1.0);
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            Vector3d col{0, 0, 0};
            for (int s = 0; s < ns; s++) {
                double u = double(i + drand48()) / double(nx);
                double v = double(j + drand48()) / double(ny);
                Ray ray = camera.getRay(u, v);
                col += color(ray, world, 0);
            }
            col /= double(ns);
//            gamma correction
            col = Vector3d{sqrt(col.x()), sqrt(col.y()), sqrt(col.z())};
            int ir = int(255.99 * col.x());
            int ig = int(255.99 * col.y());
            int ib = int(255.99 * col.z());
//            不能大于255
            ir = ir > 255 ? 255 : ir;
            ig = ig > 255 ? 255 : ig;
            ib = ib > 255 ? 255 : ib;
            fout << ir << " " << ig << " " << ib << endl;
            UpdateProgress(double(ny - j) / ny);
        }
    }
    fout.close();
    UpdateProgress(1.0);
    auto stop = std::chrono::system_clock::now();
    std::cout << "cost: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
              << " seconds\n";
}