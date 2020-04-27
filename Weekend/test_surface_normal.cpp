#include <iostream>
#include <fstream>
#include "ray.hpp"

using namespace std;

//t*t*B*B+2*t*B*(A-C)+(A-C)*(A-C)-R*R=0
double hit_sphere(const Vector3d &center, double radius, const Ray &ray) {
    Vector3d oc = ray.origin() - center;
    double a = ray.direction().dot(ray.direction());
    double b = 2.0 * oc.dot(ray.direction());
    double c = oc.dot(oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1;
    } else {
//        cout << "--------" << endl;
//        cout << (-b - sqrt(discriminant)) / (2.0 * a) << endl;
//        cout << (+b - sqrt(discriminant)) / (2.0 * a) << endl;
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

Vector3d lerp(double t, Vector3d start, Vector3d end) {
    return (1.0 - t) * start + t * end;
}

Vector3d color(const Ray &ray) {
    double t = hit_sphere({0, 0, -1}, 0.5, ray);
    if (t > 0.0) {
        Vector3d N = (ray(t) - Vector3d{0, 0, -1}).normalized();
//    (-1,1)->(0,1)
        return (N + Vector3d{1, 1, 1}) * 0.5;
    }
//    (-1,1)->(0,1)
    t = 0.5 * (ray.direction().y() + 1.0);
    return lerp(t, {1.0, 1.0, 1.0}, {0.5, 0.7, 1.0});
}

int main() {
    int nx = 200;
    int ny = 100;
    ofstream fout("test_surface_normal.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    Vector3d lower_left_corner{-2.0, -1.0, -1.0};
    Vector3d horizontal{4.0, 0.0, 0.0};
    Vector3d vertical{0.0, 2.0, 0.0};
    Vector3d origin{0.0, 0.0, 0.0};
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            double u = double(i) / double(nx);
            double v = double(j) / double(ny);
            Ray ray(origin, lower_left_corner + u * horizontal + v * vertical);
            Vector3d col = color(ray);
            int ir = int(255.99 * col.x());
            int ig = int(255.99 * col.y());
            int ib = int(255.99 * col.z());
            fout << ir << " " << ig << " " << ib << endl;
        }
    }
    fout.close();
}