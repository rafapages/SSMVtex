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

#ifndef MESH2D_H
#define MESH2D_H

#include <vector>

#include "triangle.h"

class Mesh2D {
    
public:
    Mesh2D();
    virtual ~Mesh2D();

    // Data access
    const Vector2f& getVertex(unsigned int _index) const;
    const Triangle& getTriangle(unsigned int _index) const;
    void addVector (const Vector2f& _vector);
    void addVector (const Vector2f& _vector, unsigned int _3dindex);
    void addTriangle(const Triangle& _triangle);
    void addTriangle(const Triangle& _triangle, unsigned int _3dindex);
    unsigned int getNVtx() const;
    unsigned int getNTri() const;
    float getTriArea() const;
    Vector2f getBBoxMin() const;
    Vector2f getBBoxMax() const;

    // Returns the area of the bounding box if vector _nv was added
    float getHypotheticalBBoxArea (const Vector2f& _nv) const;

    // Original 3D data info
    void setOrigVtx(unsigned int _2dindex, unsigned int _3dindex);
    void setOrigTri(unsigned int _2dindex, unsigned int _3dindex);
    int getOrigVtx(unsigned int _index) const;
    int getOrigTri(unsigned int _index) const;

    // Calculates the area of the triangle given its index
    float triangleArea(unsigned int _index) const;
    // Calculates the area of the triangle given its vertices indices
    float triangleArea(unsigned int _v0, unsigned int _v1, unsigned int _v2) const;

    // Displaces the whole mesh given a vector
    void displaceMesh(const Vector2f& _v);
    // Rotates the whole mesh 90º anti-clockwise
    void rotateMesh();
    // Add offset to the bounding box
    void addOffset2BoundingBox(float _offset);

private:


	std::vector<Vector2f> vtx_;
	std::vector<Triangle> tri_;
	unsigned int nVtx_, nTri_;

    // Correspondance of each vertex with its 3D version
    std::vector<int> origVtx_;
    // Correspondance of each triangle with its 3D version
    std::vector<int> origTri_;

    // Total area of the triangles of the mesh
    float triarea_;

    // Vector defining the current bounding box of the mesh
    Vector2f bBoxMin_, bBoxMax_;

};

#endif // MESH2D_H
