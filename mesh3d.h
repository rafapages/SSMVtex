#ifndef MESH3D_H
#define MESH3D_H

#include <stdio.h>
#include <fstream>
#include <vector>

#include "triangle.h"

class Mesh3D {

 public:
    Mesh3D();
    Mesh3D(const std::string _fileName); // Currently only reads OBJs
    Mesh3D(const std::vector<Vector3f>& _vtx, const std::vector<Triangle> _tri);
    virtual ~Mesh3D();

    // I/O
    void readOBJ(const std::string& _fileName);
    void writeOBJ(const std::string& _fileName);

    // Data access
    Vector3f getVertex(unsigned int _index) const;
    Triangle getTriangle(unsigned int _index) const;
    void addVector(const Vector3f& _vector);
    void addTriangle(const Triangle& _triangle);
    unsigned int getNVtx() const;
    unsigned int getNTri() const;

    // Triangle normal, normalized
    Vector3f getTriangleNormal(unsigned int _index) const;
    Vector3f getTriangleNormal(const Vector3f& _a, const Vector3f& _b, const Vector3f& _c) const;

 private:

    std::vector<Vector3f> vtx_;
    std::vector<Triangle> tri_;
    unsigned int nVtx_, nTri_;

};

#endif // MESH3D_H
