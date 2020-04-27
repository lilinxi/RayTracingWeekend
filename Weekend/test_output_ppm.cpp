#include <iostream>
#include <fstream>

using namespace std;

int main() {
    int nx = 200;
    int ny = 200;
    ofstream fout("test_output_ppm.ppm");
    fout << "P3" << endl << nx << " " << ny << endl << 255 << endl;
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            double r = double(i) / double(nx);
            double g = double(j) / double(ny);
            double b = 0.2;
            int ir = int(255.99 * r);
            int ig = int(255.99 * g);
            int ib = int(255.99 * b);
            fout << ir << " " << ig << " " << ib << endl;
        }
    }
    fout.close();
}