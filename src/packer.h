#ifndef PACKER_H
#define PACKER_H

#include "chart.h"

struct Contour{
	float y, x1, x2;
};

class Packer {

public:

	static void pack(std::vector<Chart>& _charts, float& _width, float& _height);

};


#endif // PACKER_H
