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

#ifndef MESH2D_H
#define MESH2D_H

#include <vector>

#include "triangle.h"

class Mesh2D {
    
public:
    Mesh2D();
    virtual ~Mesh2D();

    // Data access
    inline const Vector2f& getVertex(unsigned int _index) const{
        return vtx_[_index];
    }
    inline const Triangle& getTriangle(unsigned int _index) const{
        return tri_[_index];
    }
    inline unsigned int getNVtx() const{
        return nVtx_;
    }
    inline unsigned int getNTri() const{
        return nTri_;
    }
    inline float getTriArea() const{
        return triarea_;
    }
    inline Vector2f getBBoxMin() const{
        return bBoxMin_;
    }
    inline Vector2f getBBoxMax() const{
        return bBoxMax_;
    }

    void addVector (const Vector2f& _vector);
    void addVector (const Vector2f& _vector, unsigned int _3dindex);
    void addTriangle(const Triangle& _triangle);
    void addTriangle(const Triangle& _triangle, unsigned int _3dindex);

    // set new list of Triangles
    void replaceTriangles(const std::vector<Triangle>& _newTriangles);


    // Returns the area of the bounding box if vector _nv was added
    float getHypotheticalBBoxArea (const Vector2f& _nv) const;

    // Original 3D data info
    void setOrigVtx(unsigned int _2dindex, unsigned int _3dindex);
    void setOrigTri(unsigned int _2dindex, unsigned int _3dindex);
    int getOrigVtx(unsigned int _index) const;
    int getOrigTri(unsigned int _index) const;
    void replaceOrigTri(const std::vector<int>& _newOrigTri);

    // Calculates the area of the triangle given its index
    float triangleArea(unsigned int _index) const;
    // Calculates the area of the triangle given its vertices indices
    float triangleArea(unsigned int _v0, unsigned int _v1, unsigned int _v2) const;
    // Calculates the area of the triangle given its vertices
    static float triangleArea(const Vector2f& _v0, const Vector2f& _v1, const Vector2f& _v2);

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
