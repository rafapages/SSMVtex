#ifndef PACKER_H
#define PACKER_H

#include "chart.h"

struct Contour{
	float y, x1, x2;
};

class Packer{

public:

	Packer();
	~Packer();

	// Data access
	void setInputCharts(const std::vector<Chart>& _charts);
	void getCharts(std::vector<Chart>& _charts); // OJO CON EL TEMA REFERNCIAS
	float getWidth() const;
	float getHeight() const;


	void pack();

private:

	std::vector<Chart> charts_;

	// Width and height of the resulting group
	// of packed charts
	float width_, height_;

};


#endif