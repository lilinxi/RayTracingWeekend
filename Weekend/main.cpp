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

Hitable *randomScene() {
    vector<Hitable *> list;
    list.push_back(new Sphere({0, -1000, 0}, 1000, new Lambertian({0.5, 0.5, 0.5})));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double chooseMat = drand48();
            Vector3d center{a + 0.9 * drand48(), 0.2, b + 0.9 * drand48()};
            if ((center - Vector3d{4, 0.2, 0}).norm() > 0.9) {
                if (chooseMat < 0.8) { // diffuse
                    list.push_back(new Sphere(center, 0.2, new Lambertian(
                            {drand48() * drand48(), drand48() * drand48(), drand48() * drand48()})));
                } else if (chooseMat < 0.95) { //metal
                    list.push_back(new Sphere(center, 0.2, new Metal(
                            {0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())}, 0.5 * drand48())));
                } else { // glass
                    list.push_back(new Sphere(center, 0.2, new Dielectric(1.5)));
                }
            }
        }
    }
    list.push_back(new Sphere(Vector3d{0, 1, 0}, 1.0, new Dielectric(1.5)));
    list.push_back(new Sphere(Vector3d{-4, 1, 0}, 1.0, new Lambertian(Vector3d{0.4, 0.2, 0.1})));
    list.push_back(new Sphere(Vector3d{4, 1, 0}, 1.0, new Metal(Vector3d{0.7, 0.6, 0.5})));

    return new HitableList(list);
}

int main() {
    auto start = std::chrono::system_clock::now();
    int nx = 400;
    int ny = 200;
    int ns = 1;
    ofstream fout("random_scene.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    Hitable *world = randomScene();
    Vector3d lookFrom{3, 3, 2};
    Vector3d lookAt{0, 0, -1};
    double distToFocus = (lookFrom - lookAt).norm();
    double aperture = 2.0;
    Camera camera(lookFrom, lookAt, {0, 1, 0}, 20, double(nx) / double(ny), aperture, distToFocus);
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