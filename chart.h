#ifndef CHART_H
#define CHART_H

#include <list>

#include "mesh2D.h"

struct Edge{

	// Indices to vertices
	int a, b; // 3D mesh
    int pa, pb; // 2D mesh

    // These are to correctly arrange the edges
    int Candidate;
    int Present;
    int birth;
};


class Chart{

public:

	Chart();

	void addOffset2BoundingBox(float _offset);


private:

	std::list<Edge> perimeter_;
	Mesh2D m_;

	int nEdgePos;
	int order;



};


#endif