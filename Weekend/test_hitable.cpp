#include <iostream>
#include <fstream>
#include <vector>
#include "ray.hpp"
#include "hitable.hpp"

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
    ofstream fout("test_hitable.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    Vector3d lower_left_corner{-2.0, -1.0, -1.0};
    Vector3d horizontal{4.0, 0.0, 0.0};
    Vector3d vertical{0.0, 2.0, 0.0};
    Vector3d origin{0.0, 0.0, 0.0};
    vector<Hitable *> list;
    list.push_back(new Sphere({0, 0, -1}, 0.5));
    list.push_back(new Sphere({0, -100.5, -1}, 100));
    Hitable *world = new HitableList(list);
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            double u = double(i) / double(nx);
            double v = double(j) / double(ny);
            Ray ray(origin, lower_left_corner + u * horizontal + v * vertical);
            Vector3d col = color(ray,world);
            int ir = int(255.99 * col.x());
            int ig = int(255.99 * col.y());
            int ib = int(255.99 * col.z());
            fout << ir << " " << ig << " " << ib << endl;
        }
    }
    fout.close();
}