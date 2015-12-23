#include "triangle2d.h"

Triangle2D::Triangle2D(){

}

Triangle2D::Triangle2D(int _i0, int _i1, int _i2, int _orig3D){
    i_(0) = _i0;
    i_(1) = _i1;
    i_(2) = _i2;
    orig3D_ = _orig3D;
}

Triangle2D::~Triangle2D(){

}

Vector3i Triangle2D::getIndices() const {
    return i_;
}

int Triangle2D::getIndex(unsigned int _index) const {
    if (_index > 2){
        std::cerr << "Wrong 2D triangle index!" << std::endl;
        return -1;
    }
    return i_(_index);
}

int Triangle2D::getOrig3D() const {
    return orig3D_;
}
