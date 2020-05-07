#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "../camera.hpp"
#include "../ray.hpp"
#include "../utils.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

using namespace std;

int main() {
    auto start = std::chrono::system_clock::now();
    int nx = 400;
    int ny = 200;
    int ns = 100;
    nx = 1080;
    ny = 720;
    ns = 1000;
//    nx = 4000;
//    ny = 4000;
//    ns = 100;
    ofstream fout("test_new_pdf_interface_4_4000_4000_200.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    Hitable *world;
    Camera *camera;
    Hitable *sampleHitable;
//    以光源和玻璃球采样
    CreateCornellBoxWithSpecularSphereSampleBoth(&world, &sampleHitable, &camera, double(nx) / double(ny));
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            Vector3d col{0, 0, 0};
            for (int s = 0; s < ns; s++) {
                double u = double(i + drand48()) / double(nx);
                double v = double(j + drand48()) / double(ny);
                Ray ray = camera->getRay(u, v);
                col += Color(ray, world, sampleHitable, 0);
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