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

#include "mesh3d.h"

Mesh3D::Mesh3D(){
    nVtx_ = nTri_ = 0;
}

Mesh3D::Mesh3D(const std::string _fileName){

    nVtx_ = nTri_ = 0;
    this->readOBJ(_fileName);

}

Mesh3D::Mesh3D(const std::vector<Vector3f> &_vtx, const std::vector<Triangle> _tri){
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
                this->addTriangle(Triangle(a-1,b-1,c-1)); // OBJ indices start at 1
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

    std::cerr << "Writing to output OBJ file " << nTri_ << " triangles and " << nVtx_ << " vertices... " << std::endl;


    writeOBJheader(outMesh);

    // Vertices
    for (unsigned int i = 0; i < nVtx_; i++){
        const Vector3f current = vtx_[i];
        outMesh << "v";
        for (unsigned int j = 0; j < 3; j++){
            outMesh << " " << current(j);
        }
        outMesh << "\n";
    }

    // Triangles
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

void Mesh3D::writeOBJ(const std::string& _fileName, const std::string& _textureFile){

    std::ofstream outMesh(_fileName.c_str());

    writeOBJheader(outMesh);
    writeMTLfile(_fileName + ".mtl", _textureFile);

    outMesh << "mtllib ./" << _fileName << ".mtl" << std::endl;

    // Vertices
    for (unsigned int i = 0; i < nVtx_; i++){
        const Vector3f current = vtx_[i];
        outMesh << "v";
        for (unsigned int j = 0; j < 3; j++){
            outMesh << " " << current(j);
        }
        outMesh << "\n";
    }

    outMesh << "\nusemtl material_0" << std::endl;

    // Triangles and texture coordinates
    // Note: this is a bit inneficient yet...
    unsigned int texIndex = 1;
    for (unsigned int i = 0; i < nTri_; i++){
        const Vector3i current = tri_[i].getIndices();
        const Vector3d u = tri_[i].getU();
        const Vector3d v = tri_[i].getV();

        for (unsigned int j = 0; j < 3; j++){
            outMesh << "vt " << u(j) << " " << v(j) << std::endl;
        }

        outMesh << "f";
        for (unsigned int j = 0; j < 3; j++){
            outMesh << " " << current(j)+1 << "/" << j+texIndex; // OBJ indices start at 1
        }
        texIndex += 3;
        outMesh << "\n";
    }
}

void Mesh3D::writeOBJheader(std::ofstream& _outFile){

    if (_outFile.is_open()){

        _outFile << "# " << nVtx_ << " vertices and " << nTri_ << " triangles." << std::endl;

    } else {
        std::cerr << "Unable to write OBJ header." << std::endl;
    }
}

void Mesh3D::writeMTLfile(const std::string& _fileName, const std::string& _textureFile){

    std::ofstream mtlFile(_fileName.c_str());

    mtlFile << "newmtl material_0\n";
    mtlFile << "Ka 0.200000 0.200000 0.200000\n";
    mtlFile << "Kd 1.000000 1.000000 1.000000\n";
    mtlFile << "Ks 1.000000 1.000000 1.000000\n";
    mtlFile << "Tr 1.000000\n";
    mtlFile << "illum 2\n";
    mtlFile << "Ns 0.000000\n";
    mtlFile << "map_Kd " << _textureFile << std::endl;
    
    mtlFile.close();

}

void Mesh3D::writeVRML(const std::string& _fileName, const std::string& _textureFile){

    std::ofstream fOut;
    std::vector<int> LUT (nVtx_, -1);   // Look Up Table for vertex indices

    fOut.open(_fileName.c_str());
    if (!fOut.is_open()){
        std::cerr << "ExportVRML: unable to write output file: " << _fileName << std::endl;
        return;
    }

    std::cerr << "Writing to output VRML file " << nTri_ << " triangles and " << nVtx_ << " vertices... " << std::endl;

    std::string header[] = {
        "#VRML V2.0 utf8",
        "",
        "WorldInfo { info \"Created with Multitex, (c) 2016 "
        "by Rafael Pagés, Francisco Morán + Daniel Berjón + Sergio Arnaldo"
        "(GTI-UPM)\" }",
        "",
        "Background { skyColor 1 1 1 }",
        "",
    };

    for (unsigned int i=0; i < sizeof(header)/sizeof(header[0]); i++){
        fOut << header[i] << std::endl;
    }

    fOut << "DEF Object Group { children [\n";
    fOut << "  DEF Seen Shape {\n";
    fOut << "    appearance Appearance {\n";
    fOut << "      texture ImageTexture { url \"" << _textureFile << "\" }\n";
    fOut << "      material Material { diffuseColor 1.0 1.0 1.0 }\n"; 

    fOut << "    }\n";

    // IndexedFaceSet:
    // coord

    fOut << "    geometry IndexedFaceSet {\n";
    fOut << "      coord Coordinate { point [\n";

    // Find triangles this camera was assigned to
    std::vector<Triangle>::iterator triPtr;
    unsigned int nv = 0;
    for (triPtr = tri_.begin(); triPtr != tri_.end(); ++triPtr) {
        // Find vertices not already printed
        for (unsigned int k = 0; k < 3; k++) {
            if (LUT[triPtr->getIndex(k)] == -1) {
                // Print vertex coords
                const Vector3f thisV = vtx_[triPtr->getIndex(k)];
                fOut << "\t" <<  thisV(0) << " " << thisV(1) << " " << thisV(2) << "\n";
                LUT[triPtr->getIndex(k)] = nv++;
            }
        }
    }

    fOut << "      ] } # point, Coordinate\n";

    // texCoord

    fOut << "      texCoord TextureCoordinate { point [\n";

    for (triPtr = tri_.begin(); triPtr != tri_.end(); ++triPtr) {
        const Vector3d u = triPtr->getU();
        const Vector3d v = triPtr->getV();

        for (unsigned int k = 0; k < 3; k++) {
            fOut << "\t" << u(k) << " " << v(k)<< "\n"; /// 1-v ponía!
        }
    }
        
    fOut << "      ] } # point, TextureCoordinate\n";

    // coordIndex

    unsigned int nTriThisCam = 0;
    fOut << "      coordIndex [\n";
    // Find triangles this camera was assigned to
    triPtr = tri_.begin();
    for (unsigned int j = 0; j < nTri_; j++, ++triPtr){
        nTriThisCam++;
        // Print translated indices of vertices
        fOut << "\t" << LUT[triPtr->getIndex(0)] << " " << LUT[triPtr->getIndex(1)] << " " << LUT[triPtr->getIndex(2)] << " -1 \n";
    }
    fOut << "      ] # coordIndex\n";


    // texCoordIndex
    
    fOut << "      texCoordIndex [\n";
    int s = 0;
    for (unsigned int j = 0; j < nTriThisCam; j++) {
        fOut << "\t" << s << " " << s+1 << " "<< s+2 << " -1\n";
        s = s+3;
    }
    fOut << "      ] # texCoordIndex\n";


    // IFS & Shape ending

    fOut << "    } # IndexedFaceSet\n";
    fOut << "  } # " << "Seen" << "\n";
    fOut << "] } # Object\n\n";

    std::cerr << "done!" << std::endl;

    fOut.close();
}

void Mesh3D::writePLY(const std::string& _fileName, const std::string& _textureFile){

    std::ofstream outMesh(_fileName.c_str());

    std::cerr << "Writing to output PLY file " << nTri_ << " triangles and " << nVtx_ << " vertices... " << std::endl;


    // Intro stuff
    outMesh << "ply\n";
    outMesh << "format ascii 1.0\n";
    outMesh << "comment Model created using Multitex\n";
    outMesh << "comment TextureFile " << _textureFile << std::endl;

    // Vertex header stuff
    outMesh << "element vertex " << nVtx_ << std::endl;
    outMesh << "property float x\n";
    outMesh << "property float y\n";
    outMesh << "property float z\n";

    // Face header stuff
    outMesh << "element face " << nTri_ << std::endl;
    outMesh << "property list uchar int vertex_indices\n";
    outMesh << "property list uchar float texcoord\n";
    outMesh << "end_header\n";

    // Vertices
    for (unsigned int i = 0; i < nVtx_; i++){
        const Vector3f& current = getVertex(i);
        for (unsigned int j = 0; j < 3; j++){
            outMesh << current[j] << " ";
        }
        outMesh << "\n";
    }

    // Triangles & texture coordinates
    for (unsigned int i = 0; i < nTri_; i++){
        const Triangle& currentTri = getTriangle(i);
        const Vector3i& current = currentTri.getIndices();
        const Vector3d u = currentTri.getU();
        const Vector3d v = currentTri.getV();

        outMesh << "3 ";
        for (unsigned int j = 0; j < 3; j++){
            outMesh << current[j] << " ";
        }

        outMesh << "6 ";
        for (unsigned int j = 0; j < 3; j++){
            outMesh << u(j) << " " << v(j) << " ";
        }
        outMesh << "\n";
    }

    outMesh.close();

}



void Mesh3D::addVector(const Vector3f& _vector){
    vtx_.push_back(_vector);
    nVtx_++;
}

void Mesh3D::addTriangle(const Triangle& _triangle){
    tri_.push_back(_triangle);
    nTri_++;
}

// unsigned int Mesh3D::getNVtx() const{
//     return nVtx_;
// }

// unsigned int Mesh3D::getNTri() const{
//     return nTri_;
// }

// const Vector3f& Mesh3D::getVertex(unsigned int _index) const {
//     return vtx_[_index];
// }

// const Triangle& Mesh3D::getTriangle(unsigned int _index) const {
//     return tri_[_index];
// }

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

float Mesh3D::triangleArea(unsigned int _v0, unsigned int _v1, unsigned int _v2) const{

    Vector3f v1v0 = vtx_[_v1] - vtx_[_v0];
    Vector3f v2v0 = vtx_[_v2] - vtx_[_v0];

    //vtx_t[vl.getIndex(0)]-vtx_t[vl.getIndex(2)]) * (vtx_s[vl.getIndex(1)]-vtx_s[vl.getIndex(2)]) - (vtx_s[vl.getIndex(0)]-vtx_s[vl.getIndex(2)]) * (vtx_t[vl.getIndex(1)]-vtx_t[vl.getIndex(2)]
    const Vector3f n = v1v0.cross(v2v0);

    return 0.5 * n.norm();

}

float Mesh3D::triangleArea(unsigned int _index) const {

    return triangleArea(tri_[_index].getIndex(0), tri_[_index].getIndex(1), tri_[_index].getIndex(2));

}

void Mesh3D::setTriangleUV(unsigned int _index, const Vector3d& _u, const Vector3d& _v){
    tri_[_index].setUV(_u, _v);
}


