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

void Chart::displace(const Vector2f& _v){
	m_.displaceMesh(_v);
}

void Chart::rotate(){
	m_.rotateMesh();
}

void Chart::addOffset2BoundingBox(float _offset){
	m_.addOffset2BoundingBox(_offset);
}

void Chart::testExportOBJ(){

	std::ofstream outMesh("test.obj");

	std::cerr << "Tiene " << m_.getNVtx() << " vertices y " << m_.getNTri() << " triangulos" << std::endl;

    // Vertices
    for (unsigned int i = 0; i < m_.getNVtx(); i++){
        const Vector2f current = m_.getVertex(i);
        outMesh << "v";
        for (unsigned int j = 0; j < 2; j++){
            outMesh << " " << current(j);
        }
        outMesh << " 0 \n";
    }

    for (unsigned int i = 0; i < m_.getNTri(); i++){
        const Vector3i current = m_.getTriangle(i).getIndices();
        outMesh << "f";
        for (unsigned int j = 0; j < 3; j++){
            outMesh << " " << current(j)+1; // OBJ indices start at 1
        }
        outMesh << "\n";
    }

    outMesh.close();
}

