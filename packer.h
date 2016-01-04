#ifndef PACKER_H
#define PACKER_H

#include "chart.h"

class Packer{

public:

	Packer();
	~Packer();

	void setInputCharts(const std::vector<Chart>& _charts);
	void getCharts(std::vector<Chart>& _charts); // OJO CON EL TEMA REFERNCIAS

	void pack();

private:

	std::vector<Chart> charts_;

};


#endif