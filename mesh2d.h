#ifndef MESH2D_H
#define MESH2D_H

#include <vector>

#include "triangle.h"

class Mesh2D
{
public:
    Mesh2D();
    ~Mesh2D();

    // Data access
    Vector2f getVertex(unsigned int _index) const;
    Triangle getTriangle(unsigned int _index) const;
    void addVector (const Vector2f& _vector);
    void addVector (const Vector2f& _vector, unsigned int _3dindex);
    void addTriangle(const Triangle& _triangle);
    void addTriangle(const Triangle& _triangle, unsigned int _3dindex);
    unsigned int getNVtx() const;
    unsigned int getNTri() const;

    void setOrigVtx(unsigned int _2dindex, unsigned int _3dindex);
    void setOrigTri(unsigned int _2dindex, unsigned int _3dindex);
    int getOrigVtx(unsigned int _index) const;
    int getOrigTri(unsigned int _index) const;


private:

	std::vector<Vector2f> vtx_;
	std::vector<Triangle> tri_;
	unsigned int nVtx_, nTri_;

    // Correspondance of each vertex with its 3D version
    std::vector<int> origVtx_;
    // Correspondance of each triangle with its 3D version
    std::vector<int> origTri_;

};

#endif // MESH2D_H