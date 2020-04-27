#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "ray.hpp"
#include "hitable.hpp"
#include "camera.hpp"

using namespace std;

Vector3d randomUnitSphere() {
    Vector3d p;
    do {
//        (0,1)->(-1,1)
        p = 2 * Vector3d{drand48(), drand48(), drand48()} - Vector3d{1, 1, 1};
    } while (p.norm() >= 1);
    return p;
}

Vector3d lerp(double t, Vector3d start, Vector3d end) {
    return (1.0 - t) * start + t * end;
}

Vector3d color(const Ray &ray, Hitable *world) {
    HitRecord record;
    if (world->hit(ray, 0, MAXFLOAT, record)) {
//        center:P+N, dir:rand
        Vector3d target = record.p + record.normal + randomUnitSphere();
//        absorb 50%
        return 0.5 * color(Ray(record.p, target - record.p), world);
    } else {
        double t = 0.5 * (ray.direction().y() + 1.0);
        return lerp(t, {1.0, 1.0, 1.0}, {0.5, 0.7, 1.0});
    }
}

int main() {
    auto start = std::chrono::system_clock::now();
    int nx = 400;
    int ny = 200;
    int ns = 100;
    ofstream fout("test_diffuse_1.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    vector<Hitable *> list;
    list.push_back(new Sphere({0, 0, -1}, 0.5));
    list.push_back(new Sphere({0, -100.5, -1}, 100));
    Hitable *world = new HitableList(list);
    Camera camera;
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            Vector3d col{0, 0, 0};
            for (int s = 0; s < ns; s++) {
                double u = double(i + drand48()) / double(nx);
                double v = double(j + drand48()) / double(ny);
                Ray ray = camera.getRay(u, v);
                col += color(ray, world);
            }
            col /= double(ns);
            int ir = int(255.99 * col.x());
            int ig = int(255.99 * col.y());
            int ib = int(255.99 * col.z());
            fout << ir << " " << ig << " " << ib << endl;
        }
    }
    fout.close();
    auto stop = std::chrono::system_clock::now();
    std::cout << "cost: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
              << " seconds\n";
}