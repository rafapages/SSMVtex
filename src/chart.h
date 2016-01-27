/* 
 *  Copyright (c) 2015  Rafael Pagés (rps (at) gti.ssr.upm.es)
 *    and Universidad Politécnica de Madrid
 *
 *  This file is part of Multitex
 *
 *  Multitex is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Multitex is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

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


class Chart {

public:

	Chart();
	virtual ~Chart();

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
	float getArea() const;

	void addOffset2BoundingBox(float _offset);
	
	std::list<Edge> perimeter_;
	Mesh2D m_;


private:


	int nEdgePos_;
	int order_;

};


#endif // CHART_H
