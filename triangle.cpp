#include "triangle.h"

Triangle::Triangle(){
    cam_ = -1;
    i_(0) = -1;
    i_(1) = -1;
    i_(2) = -1;
}

Triangle::Triangle(unsigned int _i0, unsigned int _i1, unsigned int _i2){
    i_(0) = _i0;
    i_(1) = _i1;
    i_(2) = _i2;
}

Triangle::~Triangle(){

}

Vector3i Triangle::getIndices() const {
    return i_;
}

int Triangle::getIndex(unsigned int _index) const {
    if (_index > 2){
        std::cerr << "Wrong index!" << std::endl;
        exit(-1);
    }

    return i_(_index);
}

void Triangle::setUV(const Vector3d& _u, const Vector3d& _v){
    u_ = _u;
    v_ = _v;
}

void Triangle::setCam(int _cam){
    cam_ = _cam;
}

Vector3d Triangle::getU() const {
    return u_;
}

Vector3d Triangle::getV() const {
    return v_;
}


