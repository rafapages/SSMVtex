#include "triangle3d.h"

Triangle3D::Triangle3D(){
    cam_ = -1;
    i_(0) = -1;
    i_(1) = -1;
    i_(2) = -1;
}

Triangle3D::Triangle3D(unsigned int _i0, unsigned int _i1, unsigned int _i2){
    i_(0) = _i0;
    i_(1) = _i1;
    i_(2) = _i2;
}

Triangle3D::~Triangle3D(){

}

Vector3i Triangle3D::getIndices() const {
    return i_;
}

int Triangle3D::getIndex(unsigned int _index) const {
    if (_index > 2){
        std::cerr << "Wrong index!" << std::endl;
        exit(-1);
    }

    return i_(_index);
}


