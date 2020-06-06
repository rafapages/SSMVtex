/* 
 *  Copyright (c) 2017  Rafael Pagés
 *
 *  This file is part of SSMVtex
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef CHART_H
#define CHART_H

#include <list>

#include "mesh2d.h"

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
	inline void setNEdgePos(int _value){
		nEdgePos_ = _value;	
	}
	inline void increaseNEdgePos(){
		nEdgePos_++;
	}
	inline void decreaseNEdgePos(){
		nEdgePos_--;
	}
	inline void setOrder(int _value){
		order_ = _value;
	}
	inline void increaseOrder(){
		order_++;
	}
	inline int getNEdgePos() const {
		return nEdgePos_;
	}
	inline int getOrder() const {
		return order_;
	}

	inline void setMesh(const Mesh2D& _mesh){
		m_ = _mesh;
	}

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
