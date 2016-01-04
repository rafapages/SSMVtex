#include "mesh2d.h"

Mesh2D::Mesh2D(){
	nVtx_ = nTri_ = 0;
}

Mesh2D::~Mesh2D(){

}

void Mesh2D::addVector(const Vector2f& _vector){
    vtx_.push_back(_vector);
    origVtx_.push_back(-1);
    nVtx_++;
}

void Mesh2D::addVector(const Vector2f& _vector, unsigned int _3dindex){
    vtx_.push_back(_vector);
    origVtx_.push_back((int)_3dindex);
    nVtx_++;
}

void Mesh2D::addTriangle(const Triangle& _triangle){
    tri_.push_back(_triangle);
    origTri_.push_back(-1);
    nTri_++;
}

void Mesh2D::addTriangle(const Triangle& _triangle, unsigned int _3dindex){
    tri_.push_back(_triangle);
    origTri_.push_back(_3dindex);
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

Triangle Mesh2D::getTriangle(unsigned int _index) const {
    return tri_[_index];
}

void Mesh2D::setOrigVtx(unsigned int _2dindex, unsigned int _3dindex){
	origVtx_[_2dindex] = (int)_3dindex;
}

void Mesh2D::setOrigTri(unsigned int _2dindex, unsigned int _3dindex){
	origTri_[_2dindex] = (int)_3dindex;
}

int Mesh2D::getOrigVtx(unsigned int _index) const {
	return origVtx_[_index];
}

int Mesh2D::getOrigTri(unsigned int _index) const {
	return origTri_[_index];
}

