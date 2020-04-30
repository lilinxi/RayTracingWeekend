#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "ray.hpp"
#include "hitable.hpp"
#include "camera.hpp"
#include "material.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

using namespace std;

Vector3d lerp(double t, Vector3d start, Vector3d end) {
    return (1.0 - t) * start + t * end;
}

Vector3d color(const Ray &ray, Hitable *world, int depth) {
    HitRecord record;
//    ignore hit when t is near zero
    if (world->hit(ray, 0.001, MAXFLOAT, record)) {
        Ray scattered;
        Vector3d attenuation;
        if (depth < 50 && record.material->scatter(ray, record, attenuation, scattered)) {
            return attenuation.array() * color(scattered, world, depth + 1).array();
        } else {
            return Vector3d{0, 0, 0};
        }
    } else {
        double t = 0.5 * (ray.direction().y() + 1.0);
        return lerp(t, {1.0, 1.0, 1.0}, {0.5, 0.7, 1.0});
    }
}

void UpdateProgress(double progress) {
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
//    \r：回车不换行
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

Hitable *twoSpheres() {
    vector<Hitable *> list;
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("../../img/earthmap.jpg", &nx, &ny, &nn, 0);
    Texture *image = new ImageTexture(tex_data, nx, ny);
    list.push_back(new Sphere({0, -10, 0}, 10, new Lambertian(image)));
    list.push_back(new Sphere({0, 10, 0}, 10, new Lambertian(image)));

    auto start = std::chrono::system_clock::now();
    Hitable *bvh = new BVHNode(list, 0, 0);
    auto stop = std::chrono::system_clock::now();
    std::cout << "build bvh cost: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
              << " seconds\n";

    return bvh;
}

int main() {
    auto start = std::chrono::system_clock::now();
    int nx = 400;
    int ny = 200;
    int ns = 10;
    ofstream fout("test_image.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    Hitable *world = twoSpheres();
    Vector3d lookFrom{13, 2, 3};
    Vector3d lookAt{0, 0, 0};
    double distToFocus = 10.0;
    double aperture = 0.0;
    Camera camera(lookFrom, lookAt, {0, 1, 0}, 20, double(nx) / double(ny), aperture, distToFocus, 0.0, 1.0);
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