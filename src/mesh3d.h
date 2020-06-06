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

#ifndef MESH3D_H
#define MESH3D_H

#include <stdio.h>
#include <fstream>
#include <vector>

#include "triangle.h"
#include "color.h"

class Mesh3D {

 public:
    Mesh3D();
    Mesh3D(const std::string _fileName); // Currently only reads OBJs
    Mesh3D(const std::vector<Vector3f>& _vtx, const std::vector<Triangle> _tri);
    virtual ~Mesh3D();

    // I/O
    void readOBJ(const std::string& _fileName);
    void writeOBJ(const std::string& _fileName);
    void writeOBJ(const std::string& _fileName, const std::string& _textureFile);
    void writeColorPerVertexOBJ(const std::string& _fileName, const std::vector<Color> _colours);
    void writeVRML(const std::string& _fileName, const std::string& _textureFile);
    void writePLY(const std::string& _fileName, const std::string& _textureFile);


    // Data access
    // Vector3f getVertex(unsigned int _index) const;
    inline const Vector3f& getVertex(unsigned int _index) const{
        return vtx_[_index];
    }
    //Triangle getTriangle(unsigned int _index) const;
    inline const Triangle& getTriangle(unsigned int _index) const{
        return tri_[_index];
    }
    inline unsigned int getNVtx() const {
        return nVtx_;
    }
    inline unsigned int getNTri() const{
        return nTri_;
    }

    void addVector(const Vector3f& _vector);
    void addTriangle(const Triangle& _triangle);

    // set new list of Triangles
    void replaceTriangles(const std::vector<Triangle>& _newTriangles);

    void setTriangleUV(unsigned int _index, const Vector3d& _u, const Vector3d& _v);

    // Triangle normal, normalized
    Vector3f getTriangleNormal(unsigned int _index) const;
    Vector3f getTriangleNormal(const Vector3f& _a, const Vector3f& _b, const Vector3f& _c) const;

    // Calculates the area of the triangle given its index
    float triangleArea(unsigned int _index) const;
    // Calculates the area of the triangle given its vertices indices
    float triangleArea(unsigned int _v0, unsigned int _v1, unsigned int _v2) const;
    
 private:

    void writeOBJheader(std::ofstream& _outFile);
    void writeMTLfile(const std::string& _fileName, const std::string& _textureFile);

    std::vector<Vector3f> vtx_;
    std::vector<Triangle> tri_;
    unsigned int nVtx_, nTri_;

};

#endif // MESH3D_H
