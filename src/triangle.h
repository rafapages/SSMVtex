/* 
 *  Copyright (c) 2017  Rafael Pagés
 *
 *  This file is part of SSMVtex
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
