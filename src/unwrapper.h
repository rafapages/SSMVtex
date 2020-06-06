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

#ifndef UNWRAPPER_H
#define UNWRAPPER_H

#include "mesh3d.h"
#include "chart.h"

class Unwrapper {

public:

	// Transforms the 3D mesh into a group of 2D charts
	// input: 3D mesh
	// output: vector of Charts
	static void unwrapMesh(const Mesh3D& _mesh, std::vector<Chart>& _charts);

	// Transforms the 3D splats into individual 2D charts that can get packed later
	// input: 3D mesh
	// output: vector of Charts
	static void unwrapSplats(const Mesh3D& _mesh, std::vector<Chart>& _charts);


private:

	// Searchs for the neighbors of each triangle
	// filling in two given vectors:
	// input: 3D mesh
	// output:
	// _adj_count: number of neighbours each triangle has
	// _triNeighbor: contains the index of each triangle neighbors
	static void findTriangleNeighbors(const Mesh3D& _mesh, std::vector<unsigned int>& _adj_count, std::vector<int>& _triNeighbor);

	// Checks if 2D vector v1 (from v1a to v1b) intersects with v2 or not
	static bool vectorIntersec(const Vector2f& _v1a, const Vector2f& _v1b, const Vector2f& _v2a, const Vector2f& _v2b);

};



#endif // UNWRAPPER_H
