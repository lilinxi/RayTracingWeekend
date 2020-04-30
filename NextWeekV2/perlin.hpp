#ifndef PERLIN_HPP
#define PERLIN_HPP

#include "common.hpp"
#include "random.hpp"

/**
 * 柏林噪声算法
 * https://www.cnblogs.com/leoin2012/p/7218033.html
 */
class Perlin {
private:
    static const int PointCount = 256;
    Vector3d *_randomList;
    int *_permX;
    int *_permY;
    int *_permZ;
public:
    Perlin() {
        this->_randomList = new Vector3d[Perlin::PointCount];
        for (int i = 0; i < Perlin::PointCount; ++i) {
            this->_randomList[i] = Vector3d{
                    Random::GenUniformRandom(-1, 1),
                    Random::GenUniformRandom(-1, 1),
                    Random::GenUniformRandom(-1, 1)};
        }

        this->_permX = Perlin::PerlinGeneratePerm();
        this->_permY = Perlin::PerlinGeneratePerm();
        this->_permZ = Perlin::PerlinGeneratePerm();
    }

    ~Perlin() {
        delete[] this->_randomList;
        delete[] this->_permX;
        delete[] this->_permY;
        delete[] this->_permZ;
    }

    double noise(const Vector3d &p) const {
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());
        auto i = static_cast<int>(floor(p.x()));
        auto j = static_cast<int>(floor(p.y()));
        auto k = static_cast<int>(floor(p.z()));
        Vector3d c[2][2][2];

        for (int di = 0; di < 2; di++) {
            for (int dj = 0; dj < 2; dj++) {
                for (int dk = 0; dk < 2; dk++) {
                    c[di][dj][dk] = this->_randomList[
                            this->_permX[(i + di) & 255] ^
                            this->_permY[(j + dj) & 255] ^
                            this->_permZ[(k + dk) & 255]
                    ];
                }
            }
        }

        return Perlin::PerlinInterp(c, u, v, w);
    }

    double turb(const Vector3d &p, int depth = 7) const {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++) {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);
    }

private:
    static int *PerlinGeneratePerm() {
        auto p = new int[Perlin::PointCount];
        for (int i = 0; i < Perlin::PointCount; i++) {
            p[i] = i;
        }
        Perlin::Permute(p, Perlin::PointCount);
        return p;
    }

    static void Permute(int *p, int n) {
        for (int i = n - 1; i > 0; i--) {
            int target = Random::GenUniformRandomi(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    inline static double PerlinInterp(Vector3d c[2][2][2], double u, double v, double w) {
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accum = 0.0;

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    Vector3d weight_v(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu)) *
                             (j * vv + (1 - j) * (1 - vv)) *
                             (k * ww + (1 - k) * (1 - ww)) * c[i][j][k].dot(weight_v);
                }
            }
        }

        return accum;
    }
};

#endif //PERLIN_HPP
