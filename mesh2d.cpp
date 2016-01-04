#include "mesh2d.h"

Mesh2D::Mesh2D(){
	nVtx_ = nTri_ = 0;
}

Mesh2D::~Mesh2D(){

}

void Mesh2D::addVector(const Vector2f& _vector){
    vtx_.push_back(_vector);
    nVtx_++;
}

void Mesh2D::addTriangle(const Triangle2D& _triangle){
    tri_.push_back(_triangle);
    nTri_++;
}

unsigned int Mesh2D::getNVtx() const{
    return nVtx_;
}

unsigned int Mesh2D::getNTri() const{
    return nTri_;
}

Vector2f Mesh2D::getVertex(unsigned int _index) const {
    return vtx_[_index];
}

Triangle2D Mesh2D::getTriangle(unsigned int _index) const {
    return tri_[_index];
}

