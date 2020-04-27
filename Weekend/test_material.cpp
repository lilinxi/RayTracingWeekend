#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "ray.hpp"
#include "hitable.hpp"
#include "camera.hpp"
#include "material.hpp"

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

int main() {
    auto start = std::chrono::system_clock::now();
    int nx = 400;
    int ny = 200;
    int ns = 100;
    ofstream fout("test_material.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    vector<Hitable *> list;
    list.push_back(new Sphere({0, 0, -1}, 0.5, new Lambertian({0.8, 0.3, 0.3})));
    list.push_back(new Sphere({0, -100.5, -1}, 100, new Lambertian({0.8, 0.8, 0.0})));
    list.push_back(new Sphere({1, 0, -1}, 0.5, new Metal({0.8, 0.6, 0.2}, 0.5)));
    list.push_back(new Sphere({-1, 0, -1}, 0.5, new Metal({0.8, 0.8, 0.8}, 0.5)));
    Hitable *world = new HitableList(list);
    Camera camera;
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
        }
    }
    fout.close();
    auto stop = std::chrono::system_clock::now();
    std::cout << "cost: " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
              << " seconds\n";
}