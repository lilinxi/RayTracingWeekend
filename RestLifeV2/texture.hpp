#ifndef TEXTURE_HPP
#define TEXTURE_HPP

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

#endif //TEXTURE_HPP
