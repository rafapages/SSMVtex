#include "mesh3d.h"

Mesh3D::Mesh3D(){
    nVtx_ = nTri_ = 0;
}

Mesh3D::Mesh3D(const std::string _fileName){

    nVtx_ = nTri_ = 0;
    this->readOBJ(_fileName);

}

Mesh3D::Mesh3D(const std::vector<Vector3f> &_vtx, const std::vector<Triangle3D> _tri){
    vtx_ = _vtx;
    tri_ = _tri;
    nVtx_ = _vtx.size();
    nTri_ = _tri.size();
}

Mesh3D::~Mesh3D(){

}

void Mesh3D::readOBJ(const std::string& _fileName){

    std::ifstream meshFile(_fileName.c_str());

    if (meshFile.is_open()){

        std::string line;
        std::string initline;

        // First avoid comments
        do {
            std::getline(meshFile, line);
            initline = line.substr(0,2);
        } while ((initline.compare("v ") != 0) && (initline.compare("f ") != 0));

        while (!meshFile.eof()){
            if (initline.compare("v ") == 0){
                float a, b, c;
                sscanf(line.c_str(), "v %f %f %f", &a, &b, &c);
                this->addVector(Vector3f(a,b,c));
            } else if (initline.compare("f ") == 0) {
                unsigned int a, b, c;
                sscanf(line.c_str(), "f %d %d %d", &a, &b, &c);
                this->addTriangle(Triangle3D(a-1,b-1,c-1)); // OBJ indices start at 1
            } else {
                // Do nothing by now...
            }
            std::getline(meshFile, line);
            initline = line.substr(0,2);
        }

    } else {
        std::cerr << "Unable to read " << _fileName << " file!" << std::endl;
        exit(-1);
    }

    std::cerr << "3D Mesh file " << _fileName << " read with " << nVtx_ << " vertices and " << nTri_ << " triangles." << std::endl;
    meshFile.close();
}   

void Mesh3D::writeOBJ(const std::string& _fileName){

    std::ofstream outMesh(_fileName.c_str());

    // I should first print a header...

    // Vertices
    for (unsigned int i = 0; i < nVtx_; i++){
        const Vector3f current = vtx_[i];
        outMesh << "v";
        for (unsigned int j = 0; j < 3; j++){
            outMesh << " " << current(j);
        }
        outMesh << "\n";
    }

    for (unsigned int i = 0; i < nTri_; i++){
        const Vector3i current = tri_[i].getIndices();
        outMesh << "f";
        for (unsigned int j = 0; j < 3; j++){
            outMesh << " " << current(j)+1; // OBJ indices start at 1
        }
        outMesh << "\n";
    }

    std::cerr << "3D mesh file " << _fileName << " exported with " << nVtx_ << " vertices and " << nTri_ << " triangles." << std::endl;

    outMesh.close();
}

void Mesh3D::addVector(const Vector3f& _vector){
    vtx_.push_back(_vector);
    nVtx_++;
}

void Mesh3D::addTriangle(const Triangle3D& _triangle){
    tri_.push_back(_triangle);
    nTri_++;
}

unsigned int Mesh3D::getNVtx() const{
    return nVtx_;
}

unsigned int Mesh3D::getNTri() const{
    return nTri_;
}

Vector3f Mesh3D::getVertex(unsigned int _index) const {
    return vtx_[_index];
}

Triangle3D Mesh3D::getTriangle(unsigned int _index) const {
    return tri_[_index];
}

Vector3f Mesh3D::getTriangleNormal(const Vector3f &_a, const Vector3f &_b, const Vector3f &_c) const{

    Vector3f cp = (_b - _a).cross(_c - _a);
    return cp.normalized();

}

Vector3f Mesh3D::getTriangleNormal(unsigned int _index) const{

    Vector3f vertices[3];
    for (unsigned int i = 0; i < 3; i++){
        vertices[i] = vtx_[tri_[_index].getIndex(i)];
    }

    return getTriangleNormal(vertices[0], vertices[1], vertices[2]);
}


