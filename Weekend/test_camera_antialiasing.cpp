#include <iostream>
#include <fstream>
#include <vector>
#include "ray.hpp"
#include "hitable.hpp"
#include "camera.hpp"

using namespace std;

Vector3d lerp(double t, Vector3d start, Vector3d end) {
    return (1.0 - t) * start + t * end;
}

Vector3d color(const Ray &ray, Hitable *world) {
    HitRecord record;
    if (world->hit(ray, 0, MAXFLOAT, record)) {
        return (record.normal + Vector3d{1, 1, 1}) * 0.5;
    } else {
        double t = 0.5 * (ray.direction().y() + 1.0);
        return lerp(t, {1.0, 1.0, 1.0}, {0.5, 0.7, 1.0});

    }
}

int main() {
    int nx = 200;
    int ny = 100;
    int ns = 100;
    ofstream fout("test_camera_antialiasing.ppm");
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
}