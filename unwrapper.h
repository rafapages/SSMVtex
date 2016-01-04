#ifndef UNWRAPPER_H
#define UNWRAPPER_H

#include "mesh3d.h"
#include "chart.h"

class Unwrapper{

public:

	Unwrapper();
	~Unwrapper();

	// I/O
	void setInputMesh(const Mesh3D& _mesh);
	void getCharts(std::vector<Chart>& _charts); // OJO CON EL TEMA REFERNCIAS

	// Transforms the 3D mesh into a group
	// of 2D charts
	void unwrapMesh();

	// Transforms the 3D splats into individual
	// 2D charts that can get packed later
	void unwrapSplats();


private:

	// Searchs for the neighbors of each triangle
	// filling in two given vectors:
	// _adj_count: number of neighbours each triangle has
	// _triNeighbor: contains the index of each triangle neighbors
	void findTriangleNeighbors(std::vector<unsigned int>& _adj_count, std::vector<int>& _triNeighbor);



	Mesh3D mesh_;
	std::vector<Chart> charts_;



};



#endif