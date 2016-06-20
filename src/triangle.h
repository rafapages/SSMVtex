/* 
 *  Copyright (c) 2015-2016  Rafael Pagés (rps (at) gti.ssr.upm.es)
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

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <eigen3/Eigen/Dense>
#include <iostream>

using namespace Eigen;

class Triangle{

 public:

    Triangle();
    Triangle(unsigned int _i0, unsigned int _i1, unsigned int _i2);
    virtual ~Triangle();

    // Data access
    inline const Vector3i& getIndices() const {
        return i_;
    }
    inline int getIndex(unsigned int _index) const{ // 0, 1 or 2
        if (_index > 2){
            std::cerr << "Wrong index!" << std::endl;
            exit(-1);
        }
        return i_(_index);
    }
    inline void setIndices(const Vector3i& _i){
        i_ = _i;
    }
    inline void setIndices(int _a, int _b, int _c){
        i_ = Vector3i(_a,_b,_c);
    }

    inline const Vector3d& getU() const{
        return u_;
    }
    inline const Vector3d& getV() const{
        return v_;
    }

    void setUV(const Vector3d& _u, const Vector3d& _v);


private:
    Vector3i i_;

    Vector3d u_,v_; //double u[3], v[3]; // Texture coords of vertices (default 0)
};

#endif // TRIANGLE_H
