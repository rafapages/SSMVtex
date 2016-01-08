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
	~Chart();

	// Data access
	void setNEdgePos(int _value);
	void increaseNEdgePos();
	void decreaseNEdgePos();
	void setOrder(int _value);
	void increaseOrder();
	int getNEdgePos() const;
	int getOrder() const;

	// Displace chart
	void displace(const Vector2f& _v);
	// Rotate chart 90º anti-clockwise
	void rotate();

	// Chart dimensions
	float getHeight() const;
	float getWidth() const;

	void addOffset2BoundingBox(float _offset);

	void testExportOBJ();
	
	std::list<Edge> perimeter_;
	Mesh2D m_;


private:


	int nEdgePos_;
	int order_;

};


#endif