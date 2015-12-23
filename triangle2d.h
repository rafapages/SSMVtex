#ifndef TRIANGLE2D_H
#define TRIANGLE2D_H

#include "triangle3d.h"

class Triangle2D
{
public:
    Triangle2D();
    Triangle2D(int _i0, int _i1, int _i2, int _orig3D);
    ~Triangle2D();

    // Data acces
    Vector3i getIndices() const;
    int getIndex(unsigned int _index) const; // 0, 1 or 2;
    int getOrig3D() const;

private:
    Vector3i i_;
    int orig3D_;
};

#endif // TRIANGLE2D_H
