#ifndef TRIANGLE3D_H
#define TRIANGLE3D_H

#include <eigen3/Eigen/Dense>
#include <iostream>

using namespace Eigen;

class Triangle3D{

 public:

    Triangle3D();
    Triangle3D(unsigned int _i0, unsigned int _i1, unsigned int _i2);
    ~Triangle3D();

    // Data access
    Vector3i getIndices() const;
    int getIndex(unsigned int _index) const; // 0, 1 or 2

private:
    Vector3i i_;
    // IMPORTANT!!! CHECK IF cam_ IS STILL USED AT ALL
    int cam_; // -1 : Not assigned (default); -2 : new texture; 0+ : old texture
    Vector3d u_,v_; //double u[3], v[3]; // Texture coords of vertices (default 0)

};

#endif // TRIANGLE3D_H
