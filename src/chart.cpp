/* 
 *  Copyright (c) 2015-2017  Rafael Pagés (rafael.pages (at) scss.tcd.ie)
 *
 *  This file is part of SSMVtex
 *
 *  SSMVtex is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SSMVtex is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "chart.h"
#include <fstream>

Chart::Chart(){

}

Chart::~Chart(){

}

float Chart::getWidth() const {
	const Vector2f bboxmin = m_.getBBoxMin();
	const Vector2f bboxmax = m_.getBBoxMax();
	return fabs(bboxmax(0) - bboxmin(0));
}

float Chart::getHeight() const {
	const Vector2f bboxmin = m_.getBBoxMin();
	const Vector2f bboxmax = m_.getBBoxMax();
	return fabs(bboxmax(1) - bboxmin(1));
}

float Chart::getArea() const {
	return getHeight() * getWidth();
}

void Chart::displace(const Vector2f& _v){
	m_.displaceMesh(_v);
}

void Chart::rotate(){
	m_.rotateMesh();
}

void Chart::addOffset2BoundingBox(float _offset){
	m_.addOffset2BoundingBox(_offset);
}

