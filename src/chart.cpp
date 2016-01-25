#include "chart.h"
#include <fstream>

Chart::Chart(){

}

Chart::~Chart(){

}

void Chart::setNEdgePos(int _value){
	nEdgePos_ = _value;
}

void Chart::increaseNEdgePos(){
	nEdgePos_++;
}

void Chart::decreaseNEdgePos(){
	nEdgePos_--;
}

void Chart::setOrder(int _value){
	order_ = _value;
}

void Chart::increaseOrder(){
	order_++;
}

int Chart::getNEdgePos() const {
	return nEdgePos_;
}

int Chart::getOrder() const {
	return order_;
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

