#ifndef MESH2D_H
#define MESH2D_H

#include <vector>

#include "triangle2d.h"

class Mesh2D
{
public:
    Mesh2D();
    ~Mesh2D();

    // Data access
    Vector2f getVertex(unsigned int _index) const;
    Triangle2D getTriangle(unsigned int _index) const;
    void addVector (const Vector2f& _vector);
    void addTriangle(const Triangle2D& _triangle);
    unsigned int getNVtx() const;
    unsigned int getNTri() const;

private:

	std::vector<Vector2f> vtx_;
	std::vector<Triangle2D> tri_;
	unsigned int nVtx_, nTri_;

};

#endif // MESH2D_H
