/* 
 *  Copyright (c) 2015-2017  Rafael Pagés (rafael.pages (at) scss.tcd.ie)
 *
 *  This file is part of SSMVtex
 *
 *  SSMVtex is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SSMVtex is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
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
