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

#include "triangle.h"

Triangle::Triangle(){
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

const Vector3i& Triangle::getIndices() const {
    return i_;
}

int Triangle::getIndex(unsigned int _index) const {
    if (_index > 2){
        std::cerr << "Wrong index!" << std::endl;
        exit(-1);
    }

    return i_(_index);
}

void Triangle::setIndices(const Vector3i& _i){
    i_ = _i;
}

void Triangle::setIndices(int _a, int _b, int _c){
    i_ = Vector3i(_a,_b,_c);
}

void Triangle::setUV(const Vector3d& _u, const Vector3d& _v){
    u_ = _u;
    v_ = _v;
}

Vector3d Triangle::getU() const {
    return u_;
}

Vector3d Triangle::getV() const {
    return v_;
}


