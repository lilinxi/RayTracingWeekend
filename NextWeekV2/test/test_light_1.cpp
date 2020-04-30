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

Hitable *lightWithSphere() {
    vector<Hitable *> list;
    Texture *text0 = new TurbNoiseTexture2(4);
    list.push_back(new Sphere({0, -1000, 0}, 1000, new Lambertian(text0)));
    list.push_back(new Sphere({-4, 2, 0}, 2.0, new Lambertian(text0)));
    list.push_back(new Sphere({0, 2, 0}, 2.0, new Lambertian(text0)));
    list.push_back(new Sphere({0, 7, 0}, 2.0, new DiffuseLight(new ConstantTexture({10,10,10}))));
    list.push_back(new XYRect(3, 5, 1, 3, -2, new DiffuseLight(new ConstantTexture({10, 10, 10}))));

    auto start = std::chrono::system_clock::now();
    Hitable *bvh = new BVHNode(list, 0, 0);
    auto stop = std::chrono::system_clock::now();
    std::cout << "build bvh cost: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
              << " seconds\n";

//    return new HitableList(list)
    return bvh;
}

int main() {
    auto start = std::chrono::system_clock::now();
    int nx = 400;
    int ny = 200;
    int ns = 200;
//    nx = 1080;
//    ny = 720;
//    ns = 100;
    ofstream fout("test_light_1.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    Hitable *world = lightWithSphere();
    Vector3d lookFrom{15,15,15};
    Vector3d lookAt{0, 0, 0};
    double distToFocus = 10.0;
    double aperture = 0.0;
    Camera camera(lookFrom, lookAt, {0, 1, 0}, 50, double(nx) / double(ny), aperture, distToFocus, 0.0, 1.0);
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