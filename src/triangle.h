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
    Vector3i getIndices() const;
    int getIndex(unsigned int _index) const; // 0, 1 or 2
    void setIndices(const Vector3i& _i);
    void setIndices(int _a, int _b, int _c);
    void setUV(const Vector3d& _u, const Vector3d& _v);
    // void setCam(int _cam);
    Vector3d getU() const;
    Vector3d getV() const;

private:
    Vector3i i_;

    Vector3d u_,v_; //double u[3], v[3]; // Texture coords of vertices (default 0)
};

#endif // TRIANGLE_H
