/* 
 *  Copyright (c) 2015  Rafael Pagés (rps (at) gti.ssr.upm.es)
 *    and Universidad Politécnica de Madrid
 *
 *  This file is part of Multitex
 *
 *  Multitex is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Multitex is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "mesh2d.h"

Mesh2D::Mesh2D(){
	nVtx_ = nTri_ = 0;
	triarea_ = 0.0;
	bBoxMax_ = bBoxMin_ = Vector2f(0.0,0.0);
}

Mesh2D::~Mesh2D(){

}

void Mesh2D::addVector(const Vector2f& _vector){
    vtx_.push_back(_vector);
    origVtx_.push_back(-1);
    nVtx_++;

    // Update the bounding box
    bBoxMin_(0) = bBoxMin_(0) < _vector(0) ? bBoxMin_(0) : _vector(0);
    bBoxMin_(1) = bBoxMin_(1) < _vector(1) ? bBoxMin_(1) : _vector(1);
    bBoxMax_(0) = bBoxMax_(0) > _vector(0) ? bBoxMax_(0) : _vector(0);
    bBoxMax_(1) = bBoxMax_(1) > _vector(1) ? bBoxMax_(1) : _vector(1);
}

void Mesh2D::addVector(const Vector2f& _vector, unsigned int _3dindex){
    vtx_.push_back(_vector);
    origVtx_.push_back((int)_3dindex);
    nVtx_++;

    // Update the bounding box
    bBoxMin_(0) = bBoxMin_(0) < _vector(0) ? bBoxMin_(0) : _vector(0);
    bBoxMin_(1) = bBoxMin_(1) < _vector(1) ? bBoxMin_(1) : _vector(1);
    bBoxMax_(0) = bBoxMax_(0) > _vector(0) ? bBoxMax_(0) : _vector(0);
    bBoxMax_(1) = bBoxMax_(1) > _vector(1) ? bBoxMax_(1) : _vector(1);
}

void Mesh2D::addTriangle(const Triangle& _triangle){
    tri_.push_back(_triangle);
    origTri_.push_back(-1);
    nTri_++;

    const float area = triangleArea(tri_.size()-1);
    triarea_ += area;
}

void Mesh2D::addTriangle(const Triangle& _triangle, unsigned int _3dindex){
    tri_.push_back(_triangle);
    origTri_.push_back(_3dindex);
    nTri_++;

    const float area = triangleArea(tri_.size()-1);
    triarea_ += area;
}

float Mesh2D::getHypotheticalBBoxArea (const Vector2f& _nv) const {
    const float minx = bBoxMin_(0) < _nv(0) ? bBoxMin_(0) : _nv(0);
    const float maxx = bBoxMax_(0) > _nv(0) ? bBoxMax_(0) : _nv(0);
    const float miny = bBoxMin_(1) < _nv(1) ? bBoxMin_(1) : _nv(1);
    const float maxy = bBoxMax_(1) > _nv(1) ? bBoxMax_(1) : _nv(1);
    return (maxx - minx) * (maxy - miny);
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

float Mesh2D::triangleArea(unsigned int _v0, unsigned int _v1, unsigned int _v2) const{

	Vector2f v1v0 = vtx_[_v1] - vtx_[_v0];
	Vector2f v2v0 = vtx_[_v2] - vtx_[_v0];

	return (v1v0(0) * v2v0(1) - v1v0(1) * v2v0(0)) * 0.5;

}

float Mesh2D::triangleArea(unsigned int _index) const {

	return triangleArea(tri_[_index].getIndex(0), tri_[_index].getIndex(1), tri_[_index].getIndex(2));

}

void Mesh2D::displaceMesh(const Vector2f& _v){

	std::vector<Vector2f>::iterator it;
	for (it = vtx_.begin(); it != vtx_.end(); ++it){
		(*it) = (*it) + _v;
	}
	bBoxMax_ = bBoxMax_ + _v;
	bBoxMin_ = bBoxMin_ + _v;
}

void Mesh2D::rotateMesh(){
	std::vector<Vector2f>::iterator it;
	for (it = vtx_.begin(); it != vtx_.end(); ++it){
		const Vector2f thisv = (*it);
		Vector2f rot;
		// x' = -y ; y' = x
		rot(0) = (-1) * thisv(1);
		rot(1) = thisv(0);
		(*it) = rot;
	}

	// The new min and max values are the opposite
	// corners of the bounding box
	const Vector2f b1 (bBoxMin_(0), bBoxMax_(1));
	const Vector2f b2 (bBoxMax_(0), bBoxMin_(1));
	// And now we rotate them
	bBoxMin_(0) = (-1) * b1(1);
	bBoxMin_(1) = b1(0);
	bBoxMax_(0) = (-1) * b2(1);
	bBoxMax_(1) = b2(0);

	// Vertices are displaced so the mesh is aligned with the origin
	const Vector2f origin(0.0,0.0);
	displaceMesh(origin - bBoxMin_);
}

void Mesh2D::addOffset2BoundingBox(float _offset){

	const Vector2f off(_offset, _offset);
	const Vector2f aux = bBoxMax_ + off;
	bBoxMax_ = aux;

	std::vector<Vector2f>::iterator it;
	for (it = vtx_.begin(); it != vtx_.end(); ++it){
		(*it) = (*it) + off * 0.5;
	}
}

