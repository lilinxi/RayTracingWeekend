#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "perlin.hpp"

class Texture {
public:
    /**
     *
     * @param u 纹理坐标
     * @param v 纹理坐标
     * @param p 点坐标
     * @return
     */
    virtual Vector3d value(double u, double v, const Vector3d &p) const = 0;
};

class ConstantTexture : public Texture {
private:
    Vector3d _color;
public:
    ConstantTexture() {}

    ConstantTexture(const Vector3d &color) : _color(color) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        return this->_color;
    }
};

class CheckerTexture : public Texture {
private:
    Texture *_odd;
    Texture *_even;
public:
    CheckerTexture() {}

    CheckerTexture(Texture *odd, Texture *even) : _odd(odd), _even(even) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        double sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0) {
            return this->_odd->value(u, v, p);
        } else {
            return this->_even->value(u, v, p);
        }
    }
};

class SimpleNoiseTexture : public Texture {
private:
    double _scale;
    Perlin _noise;
public:
    SimpleNoiseTexture(double scale = 1) : _scale(scale) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        return Vector3d{1, 1, 1} * this->_noise.noise(this->_scale * p);
    }
};

class TurbNoiseTexture0 : public Texture {
private:
    double _scale;
    Perlin _noise;
public:
    TurbNoiseTexture0(double scale = 1) : _scale(scale) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        return Vector3d{1, 1, 1} * 0.5 * (1 + this->_noise.turb(this->_scale * p));
    }
};

class TurbNoiseTexture1 : public Texture {
private:
    double _scale;
    Perlin _noise;
public:
    TurbNoiseTexture1(double scale = 1) : _scale(scale) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        return Vector3d{1, 1, 1} * this->_noise.turb(this->_scale * p);
    }
};

class TurbNoiseTexture2 : public Texture {
private:
    double _scale;
    Perlin _noise;
public:
    TurbNoiseTexture2(double scale = 1) : _scale(scale) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        return Vector3d{1, 1, 1} * 0.5 * (1 + sin(this->_scale * p.z() + 10 * this->_noise.turb(p)));
    }
};

class ImageTexture : public Texture {
private:
    unsigned char *_data;
    int _nx, _ny;
public:
    ImageTexture() {}

    ImageTexture(unsigned char *data, int nx, int ny) : _data(data), _nx(nx), _ny(ny) {}

    virtual Vector3d value(double u, double v, const Vector3d &p) const {
        int i = u * this->_nx;
        int j = (1 - v) * this->_ny - 0.001;
        if (i < 0) {
            i = 0;
        }
        if (j < 0) {
            j = 0;
        }
        if (i > this->_nx - 1) {
            i = this->_nx - 1;
        }
        if (j > this->_ny - 1) {
            j = this->_ny - 1;
        }
        double r = int(this->_data[3 * i + 3 * this->_nx * j]) / 255.0;
        double g = int(this->_data[3 * i + 3 * this->_nx * j + 1]) / 255.0;
        double b = int(this->_data[3 * i + 3 * this->_nx * j + 2]) / 255.0;
        return Vector3d{r, g, b};
    }
};

#endif //TEXTURE_HPP
