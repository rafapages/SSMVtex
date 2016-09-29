/* 
 *  Copyright (c) 2015-2016  Rafael Pagés (rps (at) gti.ssr.upm.es)
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

#include <iomanip>
#include <algorithm>

#include <opencv2/photo/photo.hpp>

#include "multitexturer.h"

Multitexturer::Multitexturer(){
    ca_mode_ = AREA_OCCL;
    m_mode_ = TEXTURE;
    in_mode_ = MESH;
    out_extension_ = VRML;
    num_cam_mix_ = 2;
    alpha_ = 0.5;
    beta_ = 1.0;
    dimension_ = 10000000;
    imageCacheSize_ = 75;
    highlightOcclusions_ = false;
    powerOfTwoImSize_ = false;
    photoconsistency_ = false;

    nCam_ = nVtx_ = nTri_ = 0;

    realWidth_ = realHeight_ = 0.0;
    imWidth_ = imHeight_ = 0;
}

Multitexturer::~Multitexturer(){
}

void Multitexturer::parseCommandLine(int argc, char *argv[]){

    std::vector<std::string> opts;
    int index = 1;

    // We first read options
    for (; index < argc ; ++index){

        char* opt = argv[index];
        char c;
        if (opt[0] == '-'){ // options starting with '-' or '--'
            if (!isdigit(opt[1])){

                switch (c = opt[1]) {
                    case 'n':	ca_mode_ = NORMAL_VERTEX; 		opts.push_back(std::string(1,c)); break;
                    case 'b':	ca_mode_ = NORMAL_BARICENTER;	opts.push_back(std::string(1,c)); break;
                    case 'a':	ca_mode_ = AREA; photoconsistency_ = true;               opts.push_back(std::string(1,c)); break;
                    case 'l':	ca_mode_ = AREA_OCCL;           opts.push_back(std::string(1,c)); break;
                    case 'p':   ca_mode_ = AREA_OCCL; photoconsistency_ = true;   opts.push_back(std::string(1,c)); break;

                    case 't':	m_mode_ = TEXTURE;  opts.push_back(std::string(1,c)); break;
                    case 'v':   m_mode_ = VERTEX;   opts.push_back(std::string(1,c)); break;
                    case 'f':   m_mode_ = FLAT;     opts.push_back(std::string(1,c)); break;

                    case 'm':   in_mode_ = MESH;    opts.push_back(std::string(1,c)); break;
                    case 's':   in_mode_ = SPLAT;   opts.push_back(std::string(1,c)); break;

                    case 'o':   highlightOcclusions_ = true; break;

            //case 'p':   powerOfTwoImSize_ = true; opts.push_back(c); break;

                    case 'h':
                    printHelp();
                    break;

                    case '-': {
                        std::string optionValue;
                        for (unsigned int i = 2; (opt[i] != '=') && (opt[i] != '\0'); i++){
                            optionValue += opt[i];
                        }

                        int intValue;
                        float floatValue;
                        std::string stringValue;

                        if (optionValue.compare("faceCam") == 0){
                            for (unsigned int i = 2 + optionValue.length() + 1; opt[i] != '\0'; i++){
                                fileFaceCam_ += opt[i];
                            }

                        } else if (optionValue.compare("dimension") == 0){
                            for (unsigned int i = 2 + optionValue.length() + 1; opt[i] != '\0'; i++){
                        // In case any of these characters is not a number:
                                if (!isdigit(opt[i])){
                                    std::cerr << "Wrong dimension value!" << std::endl;
                                    printHelp();
                                }
                                stringValue += opt[i];
                            }
                            unsigned int uiValue;
                            std::stringstream ss;
                            ss << stringValue;
                            ss >> uiValue;
                            dimension_ = uiValue * 1000000;
                        } else if (optionValue.compare("width") == 0){
                            for (unsigned int i = 2 + optionValue.length() + 1; opt[i] != '\0'; i++){
                                if (!isdigit(opt[i])){
                                    std::cerr << "Wrong image width value!" << std::endl;
                                    printHelp();
                                }
                                stringValue += opt[i];
                            }
                            unsigned int uiValue;
                            std::stringstream ss;
                            ss << stringValue;
                            ss >> uiValue;
                            imWidth_ = uiValue;
                        } else if (optionValue.compare("alpha") == 0){
                            for (unsigned int i = 2 + optionValue.length() + 1; opt[i] != '\0'; i++){
                                stringValue += opt[i];
                            }
                            std::stringstream ss;
                            ss << stringValue;
                            ss >> floatValue;
                            alpha_ = floatValue;
                        } else if (optionValue.compare("beta") == 0){
                            for (unsigned int i = 2 + optionValue.length() + 1; opt[i] != '\0'; i++){
                                stringValue += opt[i];
                            }
                            std::stringstream ss;
                            ss << stringValue;
                            ss >> floatValue;
                            beta_ = floatValue;
                        } else if (optionValue.compare("cache") == 0){
                            for (unsigned int i = 2 + optionValue.length() +1; opt[i] != '\0'; i++){
                                stringValue += opt[i];
                            }
                            std::stringstream ss;
                            ss << stringValue;
                            ss >> intValue;
                            imageCacheSize_ = intValue;
                        } else {
                            std::cerr << "Unknown option: "  << optionValue << std::endl;
                            printHelp();
                        }
                        break;
                    }

                    default:
                    std::cerr << "Unknown option: "  << argv[index] << std::endl;
                    printHelp();
                }
            } else { // if after '-' we have a number
                std::string stringValue;
                for (unsigned int i = 1; opt[i] != '\0'; i++){
                    if (!isdigit(opt[i])){
                        std::cerr << "Wrong number of cameras to mix!" << std::endl;
                        printHelp();
                    }
                    stringValue += opt[i];
                }
                opts.push_back(stringValue);
                unsigned int cam2mix;
                std::stringstream ss;
                ss << stringValue;
                ss >> cam2mix;
                num_cam_mix_ = cam2mix;

            }

        } else { // We are done with options
            break;
        }
    }

    // Now we read input files
    unsigned int nInputFiles = argc - index;
    if (nInputFiles < 3){
        std::cerr << "Wrong number of input files." << std::endl;
        printHelp();
    }

    unsigned int inputFileIndex = 0;
    for (; index < argc; index++, inputFileIndex++){
        switch (inputFileIndex){
        case 0:     fileNameIn_        = argv[index]; break;
        case 1:     fileNameCam_       = argv[index]; break;
        case 2:     fileNameImageList_ = argv[index]; break;
        case 3:     fileNameOut_       = argv[index]; break;
        case 4:     fileNameTexOut_    = argv[index]; break;
        }
    }

    std::cerr << "fileNameIn: " << fileNameIn_ << std::endl;

    // If there are no names for the output files,
    // we set them to default values
    if  (fileNameOut_.empty() || fileNameTexOut_.empty()) {
        // In case there is something still there
        fileNameOut_.clear();
        fileNameTexOut_.clear();

        std::string optionlist;
        for (std::vector<std::string>::iterator it = opts.begin(); it != opts.end(); it++){
            optionlist += '-';
            optionlist += *it;
        }

        // We remove the '.obj' extension from the input file name
        fileNameOut_ = fileNameIn_.substr(0, fileNameIn_.length()-4);

        // And we add the optionlist as part of the name, and the new extensions
        fileNameOut_ += optionlist;
        fileNameTexOut_ = fileNameOut_;
        out_extension_ = VRML;

        fileNameTexOut_ += ".jpg";
        
        // By default, the output is a VRML file, but when we use a color-per-vertex approach, it is an OBJ
        if (m_mode_ != VERTEX){
            fileNameOut_ += ".wrl";
        } else {
            fileNameOut_ += ".obj";
        }

    } else {

        std::string extension = fileNameOut_.substr(fileNameOut_.size()-3, fileNameOut_.size());
        if (extension.compare("obj") == 0 || extension.compare("OBJ") == 0){
            out_extension_ = OBJ;
        } else if (extension.compare("wrl") == 0 || extension.compare("WRL") == 0){
            out_extension_ = VRML;
        } else if (extension.compare("ply") == 0 || extension.compare("PLY") == 0){
            out_extension_ = PLY;
        } else {
            std::cerr << extension << " extension is Unknown!" << std::endl;
            printHelp();
        }

    }

    std::cerr << "Output files will be: " << std::endl;
    std::cerr << fileNameOut_ << std::endl;
    std::cerr << fileNameTexOut_ << std::endl;

    if(in_mode_ == SPLAT && ca_mode_ == AREA_OCCL){
        std::cerr << "Splat mode is on: splats normally overlap, so occlusions will not be taken into account" << std::endl;
        ca_mode_ = AREA;
    }

}

void Multitexturer::evaluateCameraRatings(){

    std::cerr << "Evaluating camera ratings..." << std::endl;

    // Originally, tri_ratings was a field in Camera class. However, due to memory allocation
    // issues, we have extracted it from there, and make a vector of vectors instead
    std::vector<std::vector<float> > cam_tri_ratings (nCam_, std::vector<float>(nTri_, 0.0));


    for (unsigned int c = 0 ; c < nCam_ ; c++){
        cameras_[c].vtx_ratings_.resize(nVtx_);
    }

    // This step will calculate every camera-triangle ratings
    // using the chosen system.
    switch (ca_mode_) {
    case NORMAL_VERTEX:
    case NORMAL_BARICENTER:
        evaluateNormal(cam_tri_ratings);
        break;
    case AREA:
        evaluateArea(cam_tri_ratings);
        break;
    case AREA_OCCL:
        evaluateAreaWithOcclusions(cam_tri_ratings);
        break;
    }

    // Create vtx2tri and tri2tri
    // vtx2vtx is a vector containing every direct neighbor for each vertex
    std::vector<std::vector<int> > vtx2tri(nVtx_);
    for (unsigned int i = 0; i < nTri_; i++) {
        for (unsigned int j = 0; j < 3; j++)
            vtx2tri[mesh_.getTriangle(i).getIndex(j)].push_back(i);
    }
    // tri2tri is a list containing every neigbor of each triangle
    std::vector<std::list<int> > tri2tri (nTri_);
    for (unsigned int i = 0; i < nVtx_; i++) {
        for (std::vector<int>::iterator ita = vtx2tri[i].begin(); ita != vtx2tri[i].end(); ++ita) {
            for (std::vector<int>::iterator itb = vtx2tri[i].begin(); itb != vtx2tri[i].end(); ++itb) {
                tri2tri[*ita].push_back(*itb);
                tri2tri[*itb].push_back(*ita);
            }
        }
    }

    for (unsigned int i = 0; i < nTri_; i++) {
        tri2tri[i].sort();
        tri2tri[i].unique();
    }


    std::cerr << "\n";
    std::cerr << "Smoothing triangle ratings... ";

    // Normal smoothing and weighting
    for (unsigned int i = 0; i < 3; i++){
        smoothRatings(tri2tri, cam_tri_ratings);
        evaluateWeightNormal(cam_tri_ratings);
    }

    std::cerr << "done!\n";

    if (fileFaceCam_.size() != 0){
        improveFaceRatings(cam_tri_ratings);
        evaluateWeightNormal(cam_tri_ratings);
    }

    // At this point, triangle ratings are already known,
    // and their average is calculated to set the vertex ratings 
    for(unsigned int c = 0; c < nCam_; c++){

        for(unsigned int i = 0; i < nVtx_ ; i++){
            std::vector<int>::iterator it;
            float totrating = 0.0;
            int numTri = 0;
            for (it = vtx2tri[i].begin(); it != vtx2tri[i].end(); ++it){
//                if (cameras_[c].tri_ratings_[*it] == 0.0){
                if (cam_tri_ratings[c][*it] == 0){
                    totrating = 0.0;
                    break;
                }
//                float rating = cameras_[c].tri_ratings_[*it];
                const float rating = cam_tri_ratings[c][*it];
                totrating += rating;
                numTri++;
            }
            if (numTri == 0){
                cameras_[c].vtx_ratings_[i] = 0;
            } else {
                cameras_[c].vtx_ratings_[i] = totrating/numTri;
            }
        }
    }

    // This is probably not necessary, but just in case...
    for (unsigned int c = 0; c < nCam_; c++){
        std::vector<float>().swap(cam_tri_ratings[c]);
    }
    std::vector<std::vector<float> >().swap(cam_tri_ratings);

    std::cerr << "\rdone!         " << std::endl;


}

void Multitexturer::meshUnwrap(){

    if (in_mode_ == SPLAT){
        Unwrapper::unwrapSplats(mesh_, charts_);
    } else {
        Unwrapper::unwrapMesh(mesh_, charts_);
    }
}

void Multitexturer::chartPacking(){

    Packer::pack(charts_, realWidth_, realHeight_);

}


void Multitexturer::printHelp() const {    
    std::string help[] = {
        "",
        "Multitex, a 3D mesh multi-texturing system.",
        "",
        "Usage:  multitex [options] <fileNameIn> <fileNameCam> <fileNameImageList>",
        "\t[<fileNameOut> <fileNameTexOut>]",
        "",
        "\t<fileNameIn> is the name of the input text file containing the 3D scene",
        "\t  described in OBJ format. Extension : obj.",
        "\t<fileNameCam> is the name of the input text file containing the camera",
        "\t  calibration matrices. Extension: txt or out.",
        "\t<fileNameImageList> is the name of the input text file containing the",
        "\t  list of images that will be used for texturing the mesh. Extension: txt.",
        "\t<fileNameOut> is the name of the output text file that'll be created,",
        "\t  containing the textured model -- if not specified, it'll",
        "\t  be built by appending \"options\" to <fileNameIn>. Extensions:",
        "\t  vrml, obj or ply.",
        "\t<fileNameTexOut> is the name of the output customized texture",
        "\t  that'll be created, -- if not specified, it'll be built by appending",
        "\t  \".jpg + options\" to <fileNameIn>. Extension: jpg.",
        "",
        "Options:",
        "-{n|b|a|l|p}\tAssing cameras to triangles using (n) their normals, (b) their",
        "\t\tnormals using the barycenter technique, (a) their area, (l)",
        "\t\ttheir area taking occlusions into account, (p) or both occlusions and",
        "\t\tchecking photoconsistency. Default: p.",
        "-{m|s}\t\tInput value is (m) common 3D mesh or (s) a splat based 3D mesh. Default: m",
        "-{#}\t\tNumber of maximum images mixed per triangle. Default: 2 (not very smooth mixing).",
        "-{v|t|f}\tShow (v) a mesh colored per vertex, (t) a mesh with textures or",
        "\t\t(f) a mesh colored with a flat color per chart. Default: t.",
        "-o\t\thighlights occlusions in yellow.",
        "--faceCam=<imageFileName>   in case there a frontal image showing the subject's face.",
        "--alpha=<alpha> alpha is the cutoff value of the normal weighting",
        "                function, in the interval (0, 1). Default: 0.5. ",
        "--beta=<beta>   beta is the curvature of the normal weighting",
        "                function, in the interval [0, inf). Default: 1.",
        "--dimension=<dimension> resolution of the output image measured in Mpixels. Default: 1.",
        "--width=<width> width of the output image measured in pixels. If this value is",
        "\t\tgreater than zero, then <dimension> is ignored.",
        "--cache=<cachesize> size of the image cache. Default: 75.",
        "-h\t\tPrint this help message."};

    for (unsigned int i = 0; i < sizeof(help) / sizeof(help[0]); ++i) {
        std::cerr << help[i] << "\n";
    }

    exit(-1);
}

void Multitexturer::loadInputData(){
    readInputMesh();
    readImageList();
    loadCameraInfo();
}

void Multitexturer::loadCameraInfo(){

    std::string ext = fileNameCam_.substr(fileNameCam_.length()-3, 3);
    bool bundler = false;

    if (std::strcmp(ext.c_str(),"out") == 0 || std::strcmp(ext.c_str(), "OUT") == 0){
        bundler = true;
    } else {
        bundler = false;
    }

    if (bundler){
        readBundlerFile();
    } else {
        readCameraFile();
    }
}


void Multitexturer::readCameraFile(){

    std::ifstream camFile(fileNameCam_.c_str());

    if (camFile.is_open()){

        // First line contains the number of cameras
        std::string line;
        std::getline(camFile, line);
        sscanf(line.c_str(), "%u", &nCam_);

        std::cerr << "Reading " << nCam_ << " camera parameters... ";

        // Now every camera calibration file is read
        for ( unsigned int i = 0; i < nCam_ ; i++){
            Camera c;
            std::getline(camFile,line);
            c.loadCameraParameters(line);
            cameras_.push_back(c);
        }

        std::cerr << "done!\n";


    } else {
        std::cerr << "Unable to open " << fileNameCam_ << " file!" << std::endl;
        exit(-1);
    }

    camFile.close();
}

void Multitexturer::readBundlerFile(){

    std::ifstream bundlerFile(fileNameCam_.c_str());

    if (bundlerFile.is_open()){

        // Comments are skipped
        std::string line;
        do {
            std::getline(bundlerFile, line);
        } while (line.at(0) == '#');

        std::stringstream ss (line);
        ss >> nCam_;
        std::cerr << "Reading " << nCam_ << " camera parameters... ";

        for (unsigned int i = 0; i < nCam_; i++){
            Camera c;
            c.loadBundlerCameraParameters(bundlerFile, imageList_[i]);
            cameras_.push_back(c);
        }

        std::cerr << "done!\n";

    } else {
        std::cerr << "Unable to open Bundler file " << fileNameCam_ << " file!" << std::endl;
        exit(-1);
    }

    bundlerFile.close();

}

void Multitexturer::readImageList(){

    std::cerr << "Reading image list file...";

    std::ifstream listFile(fileNameImageList_.c_str());

    if (listFile.is_open()){

        std::string line;
        while (!listFile.eof()){
            std::getline(listFile, line);
            imageList_.push_back(line);
        }

    } else {
        std::cerr << "Unable to open " << fileNameImageList_ << " file!" << std::endl;
        exit(-1);
    }

    std::cerr << " done!\n";

    listFile.close();
}

void Multitexturer::readInputMesh(){

    mesh_ = Mesh3D(fileNameIn_);
    nVtx_ = mesh_.getNVtx();
    nTri_ = mesh_.getNTri();

    origMesh_ = mesh_;

}



void Multitexturer::evaluateNormal(std::vector<std::vector<float> >& _cam_tri_ratings){

    for (unsigned int i = 0; i < nTri_; i++) {

        const Triangle& thistri = mesh_.getTriangle(i);
        // Find camera most orthogonal to this triangle
        Vector3f n = mesh_.getTriangleNormal(i);
        for (unsigned int j = 0; j < nCam_; j++) {
            
            Vector3f mf = mesh_.getVertex(thistri.getIndex(0));
            // In case we are using the baricenter (centroid) position
            if (ca_mode_ == NORMAL_BARICENTER) { 
                mf += mesh_.getVertex(thistri.getIndex(1));
                mf += mesh_.getVertex(thistri.getIndex(2));
                mf /= 3;
            }

            mf -= cameras_[j].getPosition();
            Vector3f nf = mf.normalized();
            const float dp = n.dot(nf);


            // test : true if all coordinates in [0,1]
            std::vector<Vector2f> uv_vtx;
            for (unsigned int k = 0; k < 3; k++) {
                const Vector3f& vc = mesh_.getVertex(thistri.getIndex(k));
                Vector2f uv = cameras_[j].transform2uvCoord(vc);
                uv_vtx.push_back(uv);
            }

            bool test = true;
            for (unsigned int k = 0; k < uv_vtx.size(); k++){
                const Vector2f thisUV = uv_vtx[k];
                if ( thisUV(0) < 0 || cameras_[j].getImageWidth()  < thisUV(0) ||
                   thisUV(1) < 0 || cameras_[j].getImageHeight() < thisUV(1) ){
                    test = false;
                }
            }

            if (test){
            // In case the camera is facing back, the rating assigned is 0
//            cameras_[j].tri_ratings_[i] = (dp < 0) ? ( -1 * dp) : 0;
                _cam_tri_ratings[j][i] = (dp < 0) ? ( -1 * dp) : 0;
            }

        }
        std::cerr << "\r" << (float)i/nTri_*100 << std::setw(4) << std::setprecision(4) << "%      "<< std::flush;

    }
} 


void Multitexturer::evaluateArea(std::vector<std::vector<float> >& _cam_tri_ratings){


    std::vector<Vector2f> uv_vtx(3, Vector2f(0.0,0.0));
    std::vector<float> tri_ratings (nTri_);


    for (unsigned int i = 0; i < nTri_; i++) {
        
        const Triangle& thistri = mesh_.getTriangle(i);
        Vector3f n = mesh_.getTriangleNormal(i);

        for (unsigned int j = 0; j < nCam_; j++) {
            
//            cameras_[j].tri_ratings_[i] = 0;
            _cam_tri_ratings[j][i] = 0;
            // Calculate dot product (dp), in order to discard backfacing
            // It only matters whether it is positive or negative
            Vector3f mf = mesh_.getVertex(thistri.getIndex(0));
            mf -= cameras_[j].getPosition();
            const float dp = mf.dot(n);
            
            if (dp < 0) {
                // Vertex projections to the image plane
                for (unsigned int k=0; k<3; k++) {
                    const Vector3f& vc = mesh_.getVertex(thistri.getIndex(k));
                    const Vector2f uv = cameras_[j].transform2uvCoord(vc);
                    uv_vtx[k] = uv;
                }

                // test : true if all coordinates in [0,1]
                bool test = true;
                for (unsigned int k = 0; k < uv_vtx.size(); k++){
                    const Vector2f& thisUV = uv_vtx[k];
                    if ( thisUV(0) < 0 || cameras_[j].getImageWidth()  < thisUV(0) ||
                         thisUV(1) < 0 || cameras_[j].getImageHeight() < thisUV(1) ){
                        test = false;
                    }
                }

                if (test) {
                    const Vector2f& v0 = uv_vtx[0];
                    const Vector2f& v1 = uv_vtx[1];
                    const Vector2f& v2 = uv_vtx[2];
                    float area = (v0(1)-v2(1)) * (v1(0)-v2(0)) - (v0(0)-v2(0)) * (v1(1)-v2(1)); // should be divided by 2, but it really does not matter
//                    cameras_[j].tri_ratings_[i] = area;
                    _cam_tri_ratings[j][i] = area;
                }

            } // else -> tri_ratings_ stays 0
        }

        if (0 == i % 1024){
            std::cerr << "\r" << (float)i/nTri_*100 << std::setw(4) << std::setprecision(4) << "%      "<< std::flush;
        }

    }

    std::cerr << "\rdone!          " << std::endl;
}

void Multitexturer::evaluateAreaWithOcclusions(std::vector<std::vector<float> >& _cam_tri_ratings){

    // which triangles contain each vertex
    std::vector<std::vector<int> > vtx2tri (nVtx_);

    for (unsigned int i = 0; i < nTri_; i++) {
        for (unsigned int j = 0; j < 3; j++){
            vtx2tri[mesh_.getTriangle(i).getIndex(j)].push_back(i);
        }
    }



    // Occlusions are evaluated in a kind of z-buffer whose size is determined
    // by the average size of the input images. However, this buffer is goes beyond image-pixel
    // resolution, as the optimal case would be to calculate occlusions in float WC. The buffer
    // is placed only in the region that the mesh covers when projected to each image.
    float fres = 0.0;
    for (unsigned int c = 0; c < nCam_; c++){
        fres += std::max(cameras_[c].getImageWidth(), cameras_[c].getImageHeight());
    }
    const unsigned int resolution = (unsigned int) floor(fres/(float)nCam_);

    // Vector containing the projection to the image plane for each vertex
    std::vector<Vector2f> vtx_st (nVtx_);
    // Vector containing the area of the triangle projected to the image plane
    std::vector<float> triArea(nTri_);
    // Vector containing a flag determining if the triangle has been discarded or not
    std::vector<bool> validTri (nTri_);
    // Vector containing each vertex mode:
    //       DARK : Not seen
    //       SHADOW : Not seen, covered by others
    //       LIGHT : Seen
    std::vector<VtxMode> vtxSeen (nVtx_);

    // Triangle buffer: how many triangles are in each position of the grid
    std::vector<std::vector<unsigned int> > triBuffer (resolution * resolution);



    // Vector containing the position of each vertex inside the triangle buffer
    std::vector<Vector2i> vtxInBuffer(nVtx_);


    // For each camera
    for (unsigned int c = 0; c < nCam_; c++) {

        const unsigned int width = cameras_[c].getImageWidth();
        const unsigned int height = cameras_[c].getImageHeight();

 
        // Every vertex is projected onto the image plane and stored
        for (unsigned int i = 0; i < nVtx_; i++){
            const Vector2f proj_st = cameras_[c].transform2uvCoord(mesh_.getVertex(i));
            vtx_st[i] = proj_st;
        }


        // The projected area is calculated and back-facing triangles are stored
        for (unsigned int j = 0; j < nTri_; j++){
            const Vector3i& triInx = mesh_.getTriangle(j).getIndices();
            const Vector2f& v0 = vtx_st[triInx(0)];
            const Vector2f& v1 = vtx_st[triInx(1)];
            const Vector2f& v2 = vtx_st[triInx(2)];
            triArea[j] = (v0(1) - v2(1)) * (v1(0) - v2(0)) - (v0(0) - v2(0)) * (v1(1) - v2(1));
            validTri[j] = triArea[j] > 0; // It's back-facing
        }


        // If a vertex is surrounded by back-facing triangles then it's occluded
        for (unsigned int i = 0; i < nVtx_; i++){
            bool occluded = true;
            for (std::vector<int>::iterator it = vtx2tri[i].begin(); it!=vtx2tri[i].end(); ++it) {
                if (validTri[*it]){
                    occluded = false;
                    break;
                }
            }
            if (occluded){
                vtxSeen[i] = DARK;
            } else {
                vtxSeen[i] = LIGHT;
            }
        }

        // Triangle buffer is cleared
        for (unsigned int i = 0; i < resolution; i++){
            for (unsigned int j = 0; j < resolution; j++){
                triBuffer[i * resolution + j].clear();
            }
        }

        // Triangle buffer positions are now set

        // Minimum and maximum back-projected values
        float min_s, max_s, min_t, max_t;

        min_s = min_t = FLT_MAX;
        max_s = max_t = -FLT_MAX;

        for (unsigned int i = 0; i < nVtx_; i++) {
            const Vector2f& st = vtx_st[i];
            if (st(0) < min_s) {
                min_s = st(0);
            } 
            if (st(0) > max_s){
                max_s = st(0);
            } 
            if (st(1) < min_t) {
                min_t = st(1);
            } 
            if (st(1) > max_t) {
                max_t = st(1);
            }
        }

        // Vertices positions inside the triangle buffer
        for (unsigned int i = 0; i < nVtx_; i++) {
            if (vtxSeen[i] == LIGHT) {
                const Vector2f& nst = vtx_st[i];
                const unsigned int v_s = findPosGrid(nst(0), min_s, max_s, resolution);
                const unsigned int v_t = findPosGrid(nst(1), min_t, max_t, resolution);
                vtxInBuffer[i] = Vector2i(v_s,v_t);
            }
        }

        // Now, we fill the triangle buffer
        for (unsigned int i = 0; i < nTri_; i++){
            if (validTri[i]){
                const Vector3i& triIdx = mesh_.getTriangle(i).getIndices();
                
                unsigned int min_si, min_ti, max_si, max_ti;
                min_si = min_ti = INT_MAX;
                max_si = max_ti = 0;

                for (unsigned int j = 0; j < 3; j++){
                    const Vector2i& vst = vtxInBuffer[triIdx(j)];
                    const unsigned int v_s = vst(0);
                    const unsigned int v_t = vst(1);
                    if (v_s < min_si){
                        min_si = v_s;
                    } 
                    if (v_s > max_si){
                        max_si = v_s;
                    }
                    if (v_t < min_ti){
                        min_ti = v_t;
                    } 
                    if (v_t > max_ti){
                        max_ti = v_t;
                    }
                }

                const Vector2f vt0 = vtxInBuffer[triIdx(0)].cast<float>();
                const Vector2f vt1 = vtxInBuffer[triIdx(1)].cast<float>();
                const Vector2f vt2 = vtxInBuffer[triIdx(2)].cast<float>();


                for (unsigned int s = min_si; s <= max_si; s++) {
                    for (unsigned int t = min_ti; t <= max_ti; t++){
                        if (isPinsideTri (Vector2f((float)s,(float)t), vt0, vt1, vt2)){
                            triBuffer[s * resolution + t].push_back(i);
                        }
                    }
                }

            }
        }

        // Occlusions are computed in the grid of the triangle buffer
        for (unsigned int i = 0; i < nVtx_; i++) {
            if (vtxSeen[i] == LIGHT) {

                const Vector2i& v_grid = vtxInBuffer[i];
                const Vector2f& st = vtx_st[i];
                
                // candidates: triangles in same grid part, except for the ones incident to i
                std::vector<unsigned int> candidates = triBuffer[v_grid(0) * resolution + v_grid(1)];
                for (std::vector<unsigned int>::iterator it = candidates.begin(); it != candidates.end(); ++it) {
                    const Vector3i& triIdx = mesh_.getTriangle(*it).getIndices();
                    if (isPinsideTri(st, vtx_st[triIdx(0)], vtx_st[triIdx(1)], vtx_st[triIdx(2)])){
                        // Check if triangle vertex is eclipsed
                        if (isVertexEclipsed(i, *it, c)){
                            vtxSeen[i] = SHADOW;
                        } else {
                            validTri[*it] = false;
                        }

                    }
                }
                if (st(0) < 0 || st(0) > width || st(1) < 0 || st(1) > height){
                    vtxSeen[i] = SHADOW;
                }
            }
            // If a triangles has a vertex with SHADOW mode, its discarded
            if (vtxSeen[i] == SHADOW){
                for(std::vector<int>::iterator it = vtx2tri[i].begin(); it != vtx2tri[i].end(); ++it){
                    validTri[*it] = false;

                }
            }
        }


        for (unsigned int i = 0; i < nTri_; i++){
            _cam_tri_ratings[c][i] = validTri[i] ? triArea[i] : 0;
        }

        std::cerr << "\r" << (float)(c+1)/nCam_*100 << std::setw(4) << std::setprecision(4) << "%      "<< std::flush;
        
    }

}


void Multitexturer::smoothRatings(std::vector<std::list<int> > &_tri2tri, std::vector<std::vector<float> >& _cam_tri_ratings){


    #pragma omp parallel for
    for (unsigned int c = 0; c < nCam_; c++) {
        std::vector<float> tri_rat_filter(nTri_);
        const std::vector<float>& tri_ratings = _cam_tri_ratings[c];
        for (unsigned int i = 0; i < nTri_; i++) {
            
            if (tri_ratings[i] != 0) {

                // Current vertex included in neighbors
                float sumneighbors = 0; 
                for (std::list<int>::iterator it = _tri2tri[i].begin(); it != _tri2tri[i].end(); ++it){
                    sumneighbors += tri_ratings[*it];
                }
                tri_rat_filter[i] = sumneighbors/_tri2tri[i].size();
            } else {
                tri_rat_filter[i] = 0;
            }
        }

        _cam_tri_ratings[c] = tri_rat_filter;

    }

}

void Multitexturer::evaluateWeightNormal(std::vector<std::vector<float> >& _cam_tri_ratings){

    const float invalpha = 1/alpha_;
    const float invoneminusalpha = 1 / (1-alpha_);

    // triangle normal
    //Vector3f n(0.0,0.0,0.0);

    #pragma omp parallel for
    for (unsigned int i = 0; i < nTri_; i++) {
        Vector3f mf, mmf;

        // Find camera most orthogonal to this triangle
        const Vector3f n = mesh_.getTriangleNormal(i); // Normalized normal
        const Triangle& thistri = mesh_.getTriangle(i);

        // We calculate the baricenter (centroid) of the triangle
        // Vector3f mf = mesh_.getVertex(thistri.getIndex(0));
        mf = mesh_.getVertex(thistri.getIndex(0));
        mf += mesh_.getVertex(thistri.getIndex(1));
        mf += mesh_.getVertex(thistri.getIndex(2));
        mf /= 3;


        for (unsigned int j = 0; j < nCam_; j++) {

            // We check the position of the camera with respect to the triangle
            // const Vector3f mmf = mf - cameras_[j].getPosition();
            mmf = mf - cameras_[j].getPosition();
            const Vector3f nf = mmf.normalized();
            float dp = n.dot(nf);
            dp *= -1;

            if (dp <= 0){
                _cam_tri_ratings[j][i] = 0;

            } else if (dp < alpha_){
                _cam_tri_ratings[j][i] *= 0.5 * pow(dp * invalpha, beta_);
            } else {
                _cam_tri_ratings[j][i]  *= 1 - 0.5 * pow( (1-dp) * invoneminusalpha, beta_);
            }
        }
    }
}

void Multitexturer::improveFaceRatings(std::vector<std::vector<float> >& _cam_tri_rating){

    if (fileFaceCam_.size() == 0){
        return;
    }

    const int faceCam = findCameraInList(fileFaceCam_);
    if (faceCam == -1){
        return;
    }

    float face_min_x, face_max_x, face_min_y, face_max_y;
    bool facefound = findFaceInImage (face_min_x, face_max_x, face_min_y, face_max_y );

    if (!facefound){
        std::cerr << "Unable to find a face in image " << fileFaceCam_ << std::endl;
        return;
    }

    std::cerr << "Face limits:\n";
    std::cerr << "\tface_min_x = " << face_min_x << std::endl;
    std::cerr << "\tface_max_x = " << face_max_x << std::endl;
    std::cerr << "\tface_min_y = " << face_min_y << std::endl;
    std::cerr << "\tface_max_y = " << face_max_y << std::endl;

    std::vector<bool> vtx_face (nVtx_);

    // We search for vertices lying inside the face area in the image
    for (unsigned int i = 0; i < nVtx_; i++) {
        const Vector2f v = cameras_[faceCam].transform2uvCoord(mesh_.getVertex(i));
        const float face_x = v(0);
        const float face_y = v(1);
        if ( (face_min_x < face_x) && (face_x < face_max_x) && (face_min_y < face_y) && (face_y < face_max_y) ) {
            vtx_face[i] = true;
        } else {
            vtx_face[i] = false;
        }
    }

    // If any of the vertices of a triangle is considered "face vertex"
    // the rating of the triangle is multipied by 4
    for (unsigned int t = 0; t < nTri_; t++) {
        const Triangle& thistri = mesh_.getTriangle(t);
        if ( (vtx_face[thistri.getIndex(0)]) || (vtx_face[thistri.getIndex(1)]) || (vtx_face[thistri.getIndex(2)]) ){
            //cameras_[faceCam].tri_ratings_[t] *= 4;
            _cam_tri_rating[faceCam][t] *= 4;
        }
    }

}

unsigned int Multitexturer::findPosGrid (float _x, float _min, float _max, unsigned int _resolution) const {
    
    unsigned int d = (unsigned int) ((_x - _min) / (_max - _min) * _resolution);
    if (d == _resolution)
        return _resolution - 1;

    return d;
}

bool Multitexturer::isVertexEclipsed (int _v, int _t, int _c) const {

    const Vector3f& va = mesh_.getVertex(mesh_.getTriangle(_t).getIndex(0));
    Vector3f vb = mesh_.getVertex(mesh_.getTriangle(_t).getIndex(1));
    Vector3f vc = mesh_.getVertex(mesh_.getTriangle(_t).getIndex(2));

    // All vertices are defined with respect to Va
    vb -= va; 
    vc -= va;

    Vector3f n = vb.cross(vc);

    Vector3f acam, av;

    acam = cameras_[_c].getPosition();
    acam -= va;

    av = mesh_.getVertex(_v);
    av -= va;

    // We check if they are in the same side
    // of the triangle using the dot product
    float dpnacam = n.dot(acam);
    float dpnav   = n.dot(av);

    return ((dpnacam > 0 && dpnav < 0) || (dpnacam < 0 && dpnav > 0));
}

bool Multitexturer::isVertexEclipsed (const Vector3f& _v, int _t, int _c) const {

    const Vector3f& va = mesh_.getVertex(mesh_.getTriangle(_t).getIndex(0));
    Vector3f vb = mesh_.getVertex(mesh_.getTriangle(_t).getIndex(1));
    Vector3f vc = mesh_.getVertex(mesh_.getTriangle(_t).getIndex(2));

    // All vertices are defined with respect to Va
    vb -= va;
    vc -= va;

    Vector3f n = vb.cross(vc);

    Vector3f acam, av;

    acam = cameras_[_c].getPosition();
    acam -= va;

    av = _v;
    av -= va;

    // We check if they are in the same side
    // of the triangle using the dot product
    float dpnacam = n.dot(acam);
    float dpnav   = n.dot(av);

    return ((dpnacam > 0 && dpnav < 0) || (dpnacam < 0 && dpnav > 0));
}

bool Multitexturer::lineTriangleIntersection(const Vector3f &_a, const Vector3f &_b, const Vector3f &_v0, const Vector3f &_v1, const Vector3f &_v2, Vector3f& _intersection) const{

    const Vector3f p1p0 = _v1 - _v0;
    const Vector3f p2p0 = _v2 - _v0;
    const Vector3f ba = _b - _a;

    Matrix3f M, Minv;
    for (unsigned i = 0; i < 3; i++){
        M(i,0) = ba(i);
    }
    for (unsigned i = 0; i < 3; i++){
        M(i,1) = p1p0(i);
    }
    for (unsigned i = 0; i < 3; i++){
        M(i,2) = p2p0(i);
    }

    Minv = M.inverse();

    Vector3f tuv = Minv*(_b - _v0);
    const float t = tuv(0);
    const float u = tuv(1);
    const float v = tuv(2);

    _intersection = _b + (_a - _b) * t; // Intersection of line and plane

    // intersection is inside the triangle if u,v € [0,1] and u+1 <= 1
    if ( (0 <= u) && (u <= 1) && (0 <= v) && (v <= 1) && (u+v <= 1) ){
        return true;
    } else {
        return false;
    }
}


bool Multitexturer::isPinsideTri (const Vector2f& _p, const Vector2f& _a, const Vector2f& _b, const Vector2f& _c) const {

    const Vector2f uv = uvPtri(_p, _a, _b, _c);
    const float u = uv(0);
    const float v = uv(1);

    return (u > 0) && (v > 0) && (u + v < 1);

}

Vector2f Multitexturer::uvPtri (const Vector2f& _p, const Vector2f& _a, const Vector2f& _b, const Vector2f& _c) const {

    const Vector2f v0 = _c - _a;
    const Vector2f v1 = _b - _a;
    const Vector2f v2 = _p - _a;

    const float invDenom = 1 / (v0(0) * v1(1) - v1(0) * v0(1));

    const float u = invDenom * (v1(1) * v2(0) - v1(0) * v2(1));
    const float v = invDenom * (v0(0) * v2(1) - v0(1) * v2(0));

    return Vector2f(u,v);

}

Vector2f Multitexturer::lineIntersect(const Vector2f& _a, const Vector2f& _va, const Vector2f& _b, const Vector2f& _vb) const {
    const float u = (_va(1) * (_b(0) -_a(0)) + _va(0) * (_a(1) - _b(1))) / (_va(0) * _vb(1) - _va(1) * _vb(0));
    return _b + _vb * u;
}




int Multitexturer::findCameraInList(const std::string& _fileName) const{

    for (unsigned int i = 0; i < nCam_; i++){
        if (imageList_[i].compare(_fileName) == 0){
            return i;
        }
    }
    std::cerr << "There is no image file " << _fileName << " in the image list." << std::endl;
    return -1;

}

void Multitexturer::loadImageToCache(const std::string& _fileName){


    if (imageCache_.size() == imageCacheSize_){
        // We clear as many images from the cache as num_cam_mix_/2
        // but at a random location of the cache
        const unsigned int index = rand() % (imageCacheSize_ - (unsigned int) (num_cam_mix_*0.5));
        std::map<std::string, Image>::iterator it = imageCache_.begin();
        std::map<std::string, Image>::iterator it_end = imageCache_.begin();
        std::advance(it, index);
        std::advance(it_end, index + (unsigned int)(num_cam_mix_*0.5));
        imageCache_.erase(it,it_end);
    }

    imageCache_[_fileName] = Image(_fileName);

}

bool Multitexturer::findFaceInImage(float& _face_min_x, float& _face_max_x, float& _face_min_y, float& _face_max_y) const {


    // You migh have to change this address to wherever your classifier is located
    std::string cascadeName = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_default.xml";

    cv::Mat image;
    if ( fileFaceCam_.size() != 0 ){
        image = cv::imread( fileFaceCam_, 1 );
    } else {
        std::cerr << "FaceDetect: could not load face image" << std::endl;
        return false;
    }

    cv::CascadeClassifier cascade;
    if( !cascade.load( cascadeName ) ) { 
        std::cerr << "FaceDetect: could not load file with face patterns" << std::endl;
        return false;
    }

    double scale = 1;  

    if( image.empty() ){
        return false;
    }

    std::vector<cv::Rect> faces;
    cv::Mat gray;
    cv::Mat smallImg( cvRound (image.rows/scale), cvRound(image.cols/scale), CV_8UC1 );
    cvtColor( image, gray, CV_BGR2GRAY );
    cv::resize( gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR );
    cv::equalizeHist( smallImg, smallImg );

    cascade.detectMultiScale( smallImg, faces, 1.1, 2, 0 | CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(30, 30) );

    if (faces.empty()){
        return false;
    }

    // If there is a false positive of a very size it might be impossible to detect :S
    int bigFace = 0;
    int bigArea = -INT_MAX;
    for (unsigned int i = 0; i < faces.size(); i++){
        const int area = faces[i].area();
        if (area > bigArea){
            bigArea = area;
            bigFace = i;
        }
    }

    _face_min_x =  faces[bigFace].x;
    _face_max_x =  faces[bigFace].x + faces[bigFace].width;
    _face_min_y =  faces[bigFace].y;
    _face_max_y =  faces[bigFace].y + faces[bigFace].height;

    return true;

}

void Multitexturer::calculateImageSize() {

    if (realWidth_ == 0 || realHeight_ == 0){
        std::cerr << "Cannot calculate image size!" << std::endl;
        return;
    }

    // In case we didn't set a fixed width value:
    if (imWidth_ == 0){
        const float k_hw = realHeight_ / realWidth_;
        const float area = (float) dimension_;
        imWidth_ = (unsigned int) floor(sqrt(area/k_hw));
        imHeight_ = (unsigned int) floor(k_hw * imWidth_);

    // In case a width value has been set
    } else {
        const float k_hw = realHeight_ / realWidth_;
        imHeight_ = (unsigned int) floor(k_hw * (float)imWidth_);
    }

}

void Multitexturer::chartColoring() {

    std::cerr << "Chart coloring process started..." << std::endl;

    calculateImageSize();

    std::cerr << "Output image dimensions: " << imWidth_ << " x " << imHeight_ << std::endl;

    // Arrays with pixel information

    // pix_frontier: this array searches for the unwraps borders
    //               -1 if there is a border in the pixel, 0 elsewhere
    //               later on, -2 will be assigned to pixels inside borders
    ArrayXXi pix_frontier = ArrayXXi::Zero(imHeight_, imWidth_);

    // pix_triangle: this array contains the corresponding triangle in the 3D mesh of each pixel
    //               -1 if there is no triangle assigned to the pixel
    ArrayXXi pix_triangle = ArrayXXi::Zero(imHeight_, imWidth_);
    pix_triangle += -1;

    if (photoconsistency_){
        
        ArrayXXi pix_frontier_init = ArrayXXi::Zero(imHeight_, imWidth_);
        ArrayXXi pix_triangle_init = ArrayXXi::Zero(imHeight_, imWidth_);
        pix_triangle_init += -1;
        rasterizeTriangles(pix_frontier_init, pix_triangle_init);
        origMesh_ = mesh_;
        subdivideCharts(3);

    } 

    // Triangles are firstly rasterized:
    // pix_frontier and pix_triangle arrays are filled
    rasterizeTriangles(pix_frontier, pix_triangle);

    evaluateCameraRatings();

    if (photoconsistency_) {
//        checkPhotoconsistency();
        checkPhotoconsistencyPerPhoto();
    } else {
        origMesh_ = mesh_;
    }

//    // THIS NEEDS TO BE IMPROVED
//    for (unsigned int i = 10; i < 30; i++){
//        exportCamColorMesh(i);
//    }
//    std::cerr << "Exported ratings-cam models" << std::endl;


    Image imout;

    if (m_mode_ == FLAT){
        imout = colorFlatCharts(pix_triangle);
    } else if (m_mode_ == TEXTURE){
        imout = colorTextureAtlas(pix_triangle);
    } else {
        std::cerr << "Mode: " << m_mode_ << " is unknown!" << std::endl;
        return;
    }

    dilateAtlas(pix_frontier, imout, 20);
    // dilateAtlasCV(pix_triangle, imout);
    imout.save(fileNameTexOut_);

}

void Multitexturer::rasterizeTriangles(ArrayXXi& _pix_frontier, ArrayXXi& _pix_triangle){


    std::vector<Chart>::iterator unwit;
    int trcnt = 0;

    // To avoid many divisions...
    const float maxwbyimwidth =realWidth_/imWidth_;

    for (unwit = charts_.begin(); unwit != charts_.end(); ++unwit){

        findChartBorders(*unwit, _pix_frontier, _pix_triangle);

        // for each triangle we find a bounding box determined by its maximum and minimum values of x and y pixels
        for (unsigned int i = 0; i < (*unwit).m_.getNTri(); i++){
            const Triangle& tpres = (*unwit).m_.getTriangle(i);
            trcnt++;

            // Vector2f vt0,vt1,vt2;
            // vertices of the triangle
            const Vector2f vt0 = (*unwit).m_.getVertex(tpres.getIndex(0));
            const Vector2f vt1 = (*unwit).m_.getVertex(tpres.getIndex(1));
            const Vector2f vt2 = (*unwit).m_.getVertex(tpres.getIndex(2));

            float xmax, xmin, ymax, ymin;

            // maximum x value
            xmax = vt0(0) > vt1(0) ? vt0(0) : vt1(0);
            xmax = xmax > vt2(0) ? xmax : vt2(0);
            // minimum x value
            xmin = vt0(0) < vt1(0) ? vt0(0) : vt1(0);
            xmin = xmin < vt2(0) ? xmin : vt2(0);
            // maximum y value
            ymax = vt0(1) > vt1(1) ? vt0(1) : vt1(1);
            ymax = ymax > vt2(1) ? ymax : vt2(1);
            // minimum y value
            ymin = vt0(1) < vt1(1) ? vt0(1) : vt1(1);
            ymin = ymin < vt2(1) ? ymin : vt2(1);

            unsigned int xmaxp = findPosGrid(xmax, 0, realWidth_, imWidth_);
            unsigned int xminp = findPosGrid(xmin, 0, realHeight_, imHeight_);
            unsigned int ymaxp = findPosGrid(ymax, 0, realWidth_, imWidth_);
            unsigned int yminp = findPosGrid(ymin, 0, realHeight_, imHeight_);


            // Step 4.3: 3D coordinates are found to the determine pixel color

            double u0,u1,u2,v0,v1,v2;

            // Triangle vertices: vt0, vt1, vt2
            // we check the relation between 2D and 3D vertices
            // u,v, image coordinates normalized are calculated

            // double vt0dx, vt1dx, vt2dx, vt0dy, vt1dy, vt2dy, maxwd, maxhd;
            const double vt0dx = (double) vt0(0);
            const double vt0dy = (double) vt0(1);
            const double vt1dx = (double) vt1(0);
            const double vt1dy = (double) vt1(1);
            const double vt2dx = (double) vt2(0);
            const double vt2dy = (double) vt2(1);
            const double maxwd = (double) realWidth_;
            const double maxhd = (double) realHeight_;


            const int tpres_orig3D = (*unwit).m_.getOrigTri(i);
            const int vt0_orig3D = (*unwit).m_.getOrigVtx(tpres.getIndex(0));
            const int vt1_orig3D = (*unwit).m_.getOrigVtx(tpres.getIndex(1));
            if (mesh_.getTriangle(tpres_orig3D).getIndex(0) == vt0_orig3D){
                u0 = vt0dx/maxwd;
                v0 = vt0dy/maxhd;
                u1 = vt1dx/maxwd;
                v1 = vt1dy/maxhd;
                u2 = vt2dx/maxwd;
                v2 = vt2dy/maxhd;
            } else if (mesh_.getTriangle(tpres_orig3D).getIndex(0) == vt1_orig3D){
                u0 = vt1dx/maxwd;
                v0 = vt1dy/maxhd;
                u1 = vt2dx/maxwd;
                v1 = vt2dy/maxhd;
                u2 = vt0dx/maxwd;
                v2 = vt0dy/maxhd;
            } else {
                u0 = vt2dx/maxwd;
                v0 = vt2dy/maxhd;
                u1 = vt0dx/maxwd;
                v1 = vt0dy/maxhd;
                u2 = vt1dx/maxwd;
                v2 = vt1dy/maxhd;
            }

            // image coordinates are assignated and camera mode is initialized to -2
            const Vector3d tri_u(u0, u1, u2);
            const Vector3d tri_v(v0, v1, v2);


            mesh_.setTriangleUV(tpres_orig3D, tri_u, tri_v);

            // For every pixel in the triangle bounding box
            for (unsigned int colp = xminp; colp < xmaxp; colp++){ // En la original aquí había un <=
                for (unsigned int rowp = yminp; rowp < ymaxp; rowp++){ // y Aquí también 
                    // pixel center is calculated
                    Vector2f pixcenter;
                    pixcenter(0) = (float)(colp + 0.5) * maxwbyimwidth;
                    pixcenter(1) = (float)(rowp + 0.5) * maxwbyimwidth;
                    // if the pixel is inside the triangle or we are in the frontier
                    const bool condition = isPinsideTri (pixcenter, vt0, vt1, vt2);
                    if (    condition || ((_pix_frontier(rowp, colp) == -1) &&
                                          (_pix_triangle(rowp, colp) == tpres_orig3D)) ){
                        // if the pixel is inside the triangle and is not a frontier -> -2
                        if (_pix_frontier(rowp, colp) != -1){
                            _pix_frontier(rowp, colp) = -2;     
                            _pix_triangle(rowp, colp) = tpres_orig3D;
                        }
                    }
                }
            }
            if (0 == trcnt % 1024) { // Esto no siempre llega a 100%, claro.
                std::cerr << "\r" << (float)trcnt/nTri_*100 << std::setw(4) << std::setprecision(4) << "% of triangles rasterized.      ";
            }

        }
    }
    std::cerr << "\r" << 100 << std::setw(4) << std::setprecision(4) << "% of triangles rasterized.      ";
    std::cerr << "\n";
}

void Multitexturer::findChartBorders(Chart& _chart, ArrayXXi& _pix_frontier, ArrayXXi& _pix_triangle){

    // This algorithm is a custom version of Bressenham's line algotithm, accross all charts

    const float invimwidth = 1/((float) imWidth_);

    // We iterate through all the packed charts
    // we look for the positions in our "pixel grid" of each edge
    std::list<Edge>::iterator edgeit;
    for (edgeit = _chart.perimeter_.begin(); edgeit!=_chart.perimeter_.end(); edgeit++){
        Vector2f vtx_a = _chart.m_.getVertex((*edgeit).pa);
        Vector2f vtx_b = _chart.m_.getVertex((*edgeit).pb);

        // the vertices related to the current edge are first assigned
        unsigned int a_pix_x = findPosGrid(vtx_a(0), 0, realWidth_,imWidth_);
        unsigned int a_pix_y = findPosGrid(vtx_a(1), 0, realHeight_,imHeight_);
        unsigned int b_pix_x = findPosGrid(vtx_b(0), 0, realWidth_,imWidth_);
        unsigned int b_pix_y = findPosGrid(vtx_b(1), 0, realHeight_,imHeight_);

        // Safety stuff
        if (!(a_pix_x < imWidth_)) std::cout << "x " << a_pix_x << " width " << imWidth_ << std::endl;
        if (!(a_pix_y < imHeight_)) std::cout << "y " << a_pix_y << " height " << imHeight_ << std::endl;
        assert(a_pix_x < imWidth_);
        assert(a_pix_y < imHeight_);

        _pix_frontier(a_pix_y, a_pix_x) = -1;
        _pix_frontier(b_pix_y, b_pix_x) = -1;
        _pix_triangle(a_pix_y, a_pix_x) = (*edgeit).Present;
        _pix_triangle(b_pix_y, b_pix_x) = (*edgeit).Present;

        // minimum and maximum values of pixel height for the edge
        unsigned int pix_y_min = a_pix_y <= b_pix_y ? a_pix_y : b_pix_y;
        unsigned int pix_y_max = b_pix_y >= a_pix_y ? b_pix_y : a_pix_y;

        // in case they are in the same column
        unsigned int col, prow;
        if (a_pix_x == b_pix_x){

            for (prow = pix_y_min; prow < pix_y_max; prow++){
                _pix_frontier (prow, a_pix_x) = -1;
                _pix_triangle (prow, a_pix_x) = (*edgeit).Present;
            }
        // in case they are not in the same column
        } else {

            // slope of the edge, which will determine the straight line related to the edge
            float slope = 0.0;

            if (vtx_a(0) != vtx_b(0)){ // just in case we are dividing by 0
                    const Vector2f vtx_o = vtx_b - vtx_a;
                    slope = (vtx_o(1)) / (vtx_o(0));
                }

            // minimum and maximum values of pixel width for the edge
            const unsigned int pix_x_min = a_pix_x < b_pix_x ? a_pix_x : b_pix_x;
            const unsigned int pix_x_max = b_pix_x > a_pix_x ? b_pix_x : a_pix_x;

            if (slope >= 0){
                for (col = pix_x_min; col < pix_x_max; col++){
                    // the "cuts" of the edge with the pixel grid are determined
                    const float row_aux = slope * ((col+1) * invimwidth * (realWidth_) - vtx_a(0)) + vtx_a(1);
                    // and the y-pixel position for each cut
                    const unsigned int mrow = findPosGrid (row_aux,0,realHeight_,imHeight_);
                    // both previous and later x-pixels are assigned
                     _pix_frontier(mrow, col) = -1;
                    _pix_frontier(mrow, col+1) = -1;
                    _pix_triangle(mrow, col) = (*edgeit).Present;
                    _pix_triangle(mrow, col+1) = (*edgeit).Present;

                    // the rest of the pixels in the column are assigned
                    for (prow = pix_y_min; prow < mrow; prow++){
                        _pix_frontier(prow, col) = -1;
                       _pix_triangle(prow, col) = (*edgeit).Present;

                    }

                    pix_y_min = mrow;
                    // in case the next column is the maximum one, pixels of this column are also assigned
                    if ((col+1) == pix_x_max){
                        for (prow = mrow; prow<pix_y_max; prow++){
                            _pix_frontier(prow, col+1) = -1;
                            _pix_triangle(prow, col+1) = (*edgeit).Present;
                        }
                    }
                }
            } else { // slope < 0
                for (col = pix_x_max; col > pix_x_min; col--){
                    const float row_aux = slope * ((col) * invimwidth * (realWidth_) - vtx_a(0)) + vtx_a(1);
                    const unsigned int mrow = findPosGrid (row_aux,0,realHeight_,imHeight_);

                    _pix_frontier(mrow, col) = -1;
                    _pix_frontier(mrow, col-1) = -1;
                    _pix_triangle(mrow, col) = (*edgeit).Present;
                    _pix_triangle(mrow, col-1) = (*edgeit).Present;

                    for (prow = pix_y_min; prow < mrow; prow++){
                        _pix_frontier(prow, col) = -1;
                        _pix_triangle(prow, col) = (*edgeit).Present;

                    }
                    pix_y_min = mrow;

                    if ((col-1) == pix_x_min){
                        for (prow = mrow; prow < pix_y_max; prow++){
                            _pix_frontier(prow, col-1) = -1;
                            _pix_triangle(prow, col-1) = (*edgeit).Present;

                        }
                    }
                }
            }
        }
    } 
}

void Multitexturer::subdivideCharts(unsigned int _iterations){

    std::cerr << "Subdividing mesh..." << std::endl;

    std::vector<bool> repVtx (mesh_.getNVtx(), true);

    for (unsigned int iteration = 0; iteration < _iterations; iteration++){

        std::vector<Triangle> new3dtris;

        // This map keeps track of the vertices already added
        std::map<std::pair<int,int>, int> addedVtx;

        std::vector<Chart>::iterator chit;
        for (chit = charts_.begin(); chit != charts_.end(); ++chit){
            Chart& chart = *chit;
            Mesh2D mesh2d = chart.m_;

            std::vector<Triangle> new2dtris;
            std::vector<int> neworigtri;

            for (unsigned int i = 0; i < mesh2d.getNTri(); i++){

                const Triangle t2d = mesh2d.getTriangle(i);
                //const Triangle t3d = mesh_.getTriangle(mesh2d.getOrigTri(i));

                // We get the vertices of the triangle both in 2D and 3D
                Vector2f v0,v1,v2;
                v0 = mesh2d.getVertex(t2d.getIndex(0));
                v1 = mesh2d.getVertex(t2d.getIndex(1));
                v2 = mesh2d.getVertex(t2d.getIndex(2));

                Vector3f V0,V1,V2;
                int tri0,tri1,tri2;
                tri0 = mesh2d.getOrigVtx(t2d.getIndex(0));
                tri1 = mesh2d.getOrigVtx(t2d.getIndex(1));
                tri2 = mesh2d.getOrigVtx(t2d.getIndex(2));
                V0 = mesh_.getVertex(tri0);
                V1 = mesh_.getVertex(tri1);
                V2 = mesh_.getVertex(tri2);

                // We create three new vertices both in 2D and 3D
                Vector2f n0,n1,n2;
                n0 = (v0 + v1) / 2;
                n1 = (v1 + v2) / 2;
                n2 = (v2 + v0) / 2;
                unsigned int i0,i1,i2; // Indices of the new vertices

                i0 = mesh2d.getNVtx();
                mesh2d.addVector(n0);
                i1 = mesh2d.getNVtx();
                mesh2d.addVector(n1);
                i2 = mesh2d.getNVtx();
                mesh2d.addVector(n2);

                Vector3f N0,N1,N2;
                N0 = (V0 + V1) / 2;
                N1 = (V1 + V2) / 2;
                N2 = (V2 + V0) / 2;
                unsigned int I0,I1,I2;

                // We check if the new vertices are already in the mesh
                if ( addedVtx.find(std::make_pair(std::min(tri0, tri1), std::max(tri0, tri1))) != addedVtx.end() ) {
                    I0 = addedVtx[std::make_pair(std::min(tri0, tri1), std::max(tri0, tri1))];
                    //addedVtx.erase(std::make_pair(std::min(tri0, tri1), std::max(tri0, tri1))); // These lines are commented until I test this with non-manifold meshes...
                } else {
                    I0 = mesh_.getNVtx();
                    mesh_.addVector(N0);
                    addedVtx[std::make_pair(std::min(tri0,tri1), std::max(tri0,tri1))] = I0;
                }

                if ( addedVtx.find(std::make_pair(std::min(tri1, tri2), std::max(tri1, tri2))) != addedVtx.end() ) {
                    I1 = addedVtx[std::make_pair(std::min(tri1, tri2), std::max(tri1, tri2))];
                    //addedVtx.erase(std::make_pair(std::min(tri1, tri2), std::max(tri1, tri2)));
                } else {
                    I1 = mesh_.getNVtx();
                    mesh_.addVector(N1);
                    addedVtx[std::make_pair(std::min(tri1, tri2), std::max(tri1, tri2))] = I1;
                }

                if ( addedVtx.find(std::make_pair(std::min(tri0, tri2), std::max(tri0, tri2))) != addedVtx.end() ) {
                    I2 = addedVtx[std::make_pair(std::min(tri0, tri2), std::max(tri0, tri2))];
                    //addedVtx.erase(std::make_pair(std::min(tri0, tri2), std::max(tri0, tri2)));
                } else {
                    I2 = mesh_.getNVtx();
                    mesh_.addVector(N2);
                    addedVtx[std::make_pair(std::min(tri0,tri2), std::max(tri0,tri2))] = I2;
                }

                // 
                mesh2d.setOrigVtx(i0, I0);
                mesh2d.setOrigVtx(i1, I1);
                mesh2d.setOrigVtx(i2, I2);

                // We add the four new triangles to the new lists
                new2dtris.push_back(Triangle(t2d.getIndex(0), i0, i2)); // v0, n0, n2
                new3dtris.push_back(Triangle(mesh2d.getOrigVtx(t2d.getIndex(0)), I0, I2)); // v0, n0, n2
                neworigtri.push_back(new3dtris.size()-1);

                new2dtris.push_back(Triangle(i0, t2d.getIndex(1), i1)); // n0, v1, n1
                new3dtris.push_back(Triangle(I0, mesh2d.getOrigVtx(t2d.getIndex(1)), I1)); // n0, v1, n1
                neworigtri.push_back(new3dtris.size()-1);

                new2dtris.push_back(Triangle(i1, t2d.getIndex(2), i2)); // n1, v2, n2
                new3dtris.push_back(Triangle(I1, mesh2d.getOrigVtx(t2d.getIndex(2)), I2)); // n1, v2, n2
                neworigtri.push_back(new3dtris.size()-1);

                new2dtris.push_back(Triangle(i0, i1, i2)); // n0, n1, n2
                new3dtris.push_back(Triangle(I0, I1, I2)); // n0, n1, n2
                neworigtri.push_back(new3dtris.size()-1);

            }

            // We add the new 2D triangles to the chart 
            mesh2d.replaceTriangles(new2dtris);
            mesh2d.replaceOrigTri(neworigtri);
            chit->setMesh(mesh2d);
        }

        // We add the new 3D triangles to the mesh
        mesh_.replaceTriangles(new3dtris);
        updateNumbers();

        std::cerr << "\r" << (float)(iteration+1)/_iterations*100 << std::setw(4) << std::setprecision(4) << "%               ";

    }

    std::cerr << "\n";

}

void Multitexturer::updateNumbers(){
    nTri_ = mesh_.getNTri();
    nVtx_ = mesh_.getNVtx();
}

void Multitexturer::checkPhotoconsistency(){

    for (unsigned int i = 0; i < nVtx_; i++){
        const Vector3f current = mesh_.getVertex(i);

        // Cameras with a rating different than 0 are stored in this multimap
        std::multimap<float, int> ratings;
        for (unsigned int c = 0; c < nCam_; c++){
            if (cameras_[c].vtx_ratings_[i] != 0){
                ratings.insert(std::pair<float, int>(cameras_[c].vtx_ratings_[i], c));
            }
        }

        std::multimap<float, int>::iterator it= ratings.begin();
        std::vector<Color> camColors;
        Color c_ave(0.0,0.0,0.0);
        for (; it != ratings.end(); ++it){
            // std::cerr << "area/index: " << it->first << " " << it->second << std::endl;
            int camera = it->second;

            // cache stuff
            std::string imageName = imageList_[camera];
            if (imageCache_.find(imageName) == imageCache_.end()){
                loadImageToCache(imageName);
            }

            const Vector2f v_st = cameras_[camera].transform2uvCoord(current);
            // Projection coordinates
            const float proj_s = v_st(0);
            const float proj_t = v_st(1);

            if (proj_s < 0.0 || proj_t < 0.0){ // This may happen and it's very wrong
                continue;
            }

            float image_row = (float)imageCache_[imageName].getHeight() - proj_t;
            float image_col = proj_s;

            // In case a rounding error gives us a pixel outside the image
            image_row = std::min (image_row, (float)imageCache_[imageName].getHeight());
            image_col = std::min (image_col, (float)imageCache_[imageName].getWidth());
            image_row = std::max (image_row, 0.0f);
            image_col = std::max (image_col, 0.0f);

            Color col = imageCache_[imageName].interpolate(image_row, image_col, BILINEAR);
            camColors.push_back(col);
            c_ave += col;
        }

        c_ave = c_ave / (float)ratings.size(); // Average color


        std::vector<Color> colDif; // color diference with respect to the average
        std::vector<Color>::iterator cit;
        for (cit = camColors.begin(); cit != camColors.end(); ++cit){
            const Color cc = *cit;
            colDif.push_back(cc - c_ave);
        }        

        float var_r, var_g, var_b; // variance
        var_r = var_g = var_b = 0.0;
        for (cit = camColors.begin(); cit != camColors.end(); ++cit){
            var_r += (cit->getRed() - c_ave.getRed())     * (cit->getRed() - c_ave.getRed());
            var_g += (cit->getGreen() - c_ave.getGreen()) * (cit->getGreen() - c_ave.getGreen());
            var_b += (cit->getBlue() - c_ave.getBlue())   * (cit->getBlue() - c_ave.getBlue());
        }

        var_r = var_r / (float) camColors.size();
        var_g = var_g / (float) camColors.size();
        var_b = var_b / (float) camColors.size();

        float dev_r, dev_g, dev_b; // Standard deviation
        dev_r = sqrt(var_r);
        dev_g = sqrt(var_g);
        dev_b = sqrt(var_b);

        it = ratings.begin();
        std::vector<Color>::iterator dit = colDif.begin();
        for (; dit != colDif.end(); ++dit, ++it){
            Color cc = *dit;
            if (fabs(cc.getRed()) > dev_r || fabs(cc.getGreen())> dev_g || fabs(cc.getBlue()) > dev_b){
                const int camindex = it->second;
                cameras_[camindex].vtx_ratings_[i] = 0;
            }
        }
        if (0 == (i+1) % 1024) { // Too much information will kill you
            std::cerr << "\r" << (float)(i+1)/nVtx_*100 << std::setw(4) << std::setprecision(4) << "% photoconsistency check. ";
            std::cerr << (float)imageCache_.size()/imageCacheSize_ * 100 << std::setw(4) << std::setprecision(4) << "% of cache usage (";
            std::cerr << imageCache_.size() << "/" << imageCacheSize_ << ").      " << std::flush;
        }
    }
    std::cerr << "\n";

}

void Multitexturer::checkPhotoconsistencyPerPhoto() {

    std::cerr << "Checking photoconsistency..." << std::endl;

    std::vector<std::vector<Color> > colors_per_vtx (nVtx_);
    std::vector<std::vector<float> > ratings_per_vtx (nVtx_);
    std::vector<Color> ave_colors (nVtx_, Color(0.0,0.0,0.0));
    std::vector<int> ave_num (nVtx_, 0);

    for (unsigned int i = 0; i < nVtx_; i++){
        colors_per_vtx[i].assign(nCam_, Color(0.0,0.0,0.0));
        ratings_per_vtx[i].assign(nCam_, -1.0);
    }

    #pragma omp parallel for
    for (unsigned int c = 0; c < nCam_; c++){

        std::string imageName = imageList_[c];
        Image image(imageName);

        for (unsigned int i = 0; i < nVtx_; i++){
            const Vector3f & current = mesh_.getVertex(i);

            if (cameras_[c].vtx_ratings_[i] > 0.0){

                // In this case, we save the Color information from this camera

                const Vector2f v_st = cameras_[c].transform2uvCoord(current);
                // Projection coordinates
                const   float proj_s = v_st(0);
                const float proj_t = v_st(1);

                if (proj_s < 0.0 || proj_t < 0.0){ // This may happen and it's very wrong
                    continue;
                }

                float image_row = (float) image.getHeight() - proj_t;
                float image_col = proj_s;

                // In case a rounding error gives us a pixel outside the image
                image_row = std::min (image_row, (float) image.getHeight());
                image_col = std::min (image_col, (float) image.getWidth());
                image_row = std::max (image_row, 0.0f);
                image_col = std::max (image_col, 0.0f);

                Color col = image.interpolate(image_row, image_col, BILINEAR);

                colors_per_vtx[i][c] = col;
                ratings_per_vtx[i][c] = cameras_[c].vtx_ratings_[i];

//                if (0 == (i+1) % 1024) { // Too much information will kill you
//                    std::cerr << "\r" << "Image " << c+1 << "/" << nCam_ << ". Progress: ";
//                    std::cerr << (float)(i+1)/nVtx_*100 << std::setw(4) << std::setprecision(4) << "%.       ";
//                }
            }
        }
//        std::cerr << "\r" << "Image " << c+1 << "/" << nCam_ << ". Progress: 100%             ";

    }

    #pragma omp parallel for
    for (unsigned int i = 0; i < nVtx_; i++){
        for (unsigned int c = 0; c < nCam_; c++){
            if (ratings_per_vtx[i][c] > 0.0){
                ave_colors[i] += colors_per_vtx[i][c];
                ave_num[i] += 1;
            }
        }
    }

    //std::cerr << "\n";

    #pragma omp parallel for
    for (unsigned int i = 0; i < nVtx_; i++){
        std::vector<Color>& camColors = colors_per_vtx[i];
        std::vector<float>& camRatings = ratings_per_vtx[i];

        ave_colors[i] = ave_colors[i] / (float) ave_num[i];

        const Color& c_ave = ave_colors[i];
        std::vector<Color> colDif; // color diference with respect to the average
        std::vector<Color> finalCamColors;
        std::vector<int> camDif;

        std::vector<Color>::iterator cit;

        unsigned int r = 0;
        for (cit = camColors.begin(); cit != camColors.end(); ++cit, r++){
            if (camRatings[r] < 0.0) continue;
            const Color cc = *cit;
            finalCamColors.push_back(cc);
            colDif.push_back(cc - c_ave);
            camDif.push_back(r);
        }

        float var_r, var_g, var_b; // variance
        var_r = var_g = var_b = 0.0;
        for (cit = finalCamColors.begin(); cit != finalCamColors.end(); ++cit){
            var_r += (cit->getRed() - c_ave.getRed())     * (cit->getRed() - c_ave.getRed());
            var_g += (cit->getGreen() - c_ave.getGreen()) * (cit->getGreen() - c_ave.getGreen());
            var_b += (cit->getBlue() - c_ave.getBlue())   * (cit->getBlue() - c_ave.getBlue());
        }

        var_r = var_r / (float) finalCamColors.size();
        var_g = var_g / (float) finalCamColors.size();
        var_b = var_b / (float) finalCamColors.size();


        float dev_r, dev_g, dev_b; // Standard deviation
        dev_r = sqrt(var_r);
        dev_g = sqrt(var_g);
        dev_b = sqrt(var_b);

//        it = ratings.begin();
        std::vector<int>::iterator it = camDif.begin();
        std::vector<Color>::iterator dit = colDif.begin();
        for (; dit != colDif.end(); ++dit, ++it){
            Color cc = *dit;
            if (fabs(cc.getRed()) > dev_r || fabs(cc.getGreen())> dev_g || fabs(cc.getBlue()) > dev_b){
                const int camindex = *it;
                cameras_[camindex].vtx_ratings_[i] = 0;
            }
        }

    }
    std::cerr << "done!" << std::endl;

}


void Multitexturer::colorVertices(std::vector<Color>& _meshcolors){

    _meshcolors.clear();
    Color black(0.0,0.0,0.0);
    for (unsigned int i = 0; i < nVtx_; i++){
        _meshcolors.push_back(black);
    }


    evaluateCameraRatings();

    checkPhotoconsistency();


    for (unsigned int i = 0; i < nVtx_; i++){

        std::multimap<float, int> ratings_cam;
        const Vector3f current = mesh_.getVertex(i);


        // Best cameras are assigned
        ratings_cam.clear();
        for (unsigned int c = 0; c < nCam_; c++) {
            if (cameras_[c].vtx_ratings_[i] != 0){
                ratings_cam.insert(std::pair<float,int>(cameras_[c].vtx_ratings_[i],c));
            }
        }

        // Number of cameras to mix is the minimun between:
        // our input value and the number of cameras available for the current pixel
        const unsigned int tomix = ratings_cam.size() < (unsigned int) num_cam_mix_ ? ratings_cam.size() : num_cam_mix_;

        // Calculation of the weights
        std::vector<int> cameras_order;
        std::vector<float> weights_order;
        if (tomix != 0) {
            unsigned int p;
            float sumratings = 0;
            cameras_order.resize(tomix);
            weights_order.resize(tomix);

            // Naïve way of calculating weights... could be improved
            std::multimap<float, int>::iterator it;
            for (it = ratings_cam.end(), p = 0; p < tomix; ++p) {
                it--;
                sumratings += (*it).first;
                cameras_order[p] = (*it).second;
            }
            for (it = ratings_cam.end(), p = 0; p < tomix; ++p) {
                it--;
                weights_order[p] = (*it).first/sumratings;
            }
        }

        Color col;
        for (unsigned int p = 0; p < tomix; p++) {
            int camera = cameras_order[p];
            float weight = weights_order[p];

            // cache stuff
            std::string imageName = imageList_[camera];
            if (imageCache_.find(imageName) == imageCache_.end()){
                loadImageToCache(imageName);
            }

            const Vector2f v_st = cameras_[camera].transform2uvCoord(current);
            // Projection coordinates
            const float proj_s = v_st(0);
            const float proj_t = v_st(1);

            if (proj_s < 0.0 || proj_t < 0.0){ // This may happen and it's very wrong
                continue;
            }

            float image_row = (float)imageCache_[imageName].getHeight() - proj_t;
            float image_col = proj_s;

            // In case a rounding error gives us a pixel outside the image
            image_row = std::min (image_row, (float)imageCache_[imageName].getHeight());
            image_col = std::min (image_col, (float)imageCache_[imageName].getWidth());
            image_row = std::max (image_row, 0.0f);
            image_col = std::max (image_col, 0.0f);

            if (p == 0) { // Difference : = vs. +=
                col = imageCache_[imageName].interpolate(image_row, image_col, BILINEAR) * weight;
            } else {
                col += imageCache_[imageName].interpolate(image_row, image_col, BILINEAR) * weight;   
            }
        }

        // color is assigned to the vertex
        _meshcolors[i] = col;

        std::cerr << "\r" << (float)(i+1)/nVtx_*100 << std::setw(4) << std::setprecision(4) << "% of vertices colored. ";
        std::cerr << (float)imageCache_.size()/imageCacheSize_ * 100 << std::setw(4) << std::setprecision(4) << "% of cache usage (";
        std::cerr << imageCache_.size() << "/" << imageCacheSize_ << ").      " << std::flush;
    }

    std::cerr << "\n";

}


Image Multitexturer::colorTextureAtlas(const ArrayXXi& _pix_triangle) {


    // Output image is initialized
    Image imout =  Image (imHeight_, imWidth_);

    // pix_ratings: this vector contains a rating for each camera. It will be re-used for every pixel
    std::vector<float> pix_ratings (nCam_, 0.0);

    // ratings_cam:
    std::multimap<float,int> ratings_cam;

    std::vector<Chart>::iterator unwit;

    // To avoid divisions...
    const float maxwbyimwidth =realWidth_/imWidth_;

    int trcnt = 0;
//#pragma omp parallel for
    for (unwit = charts_.begin(); unwit != charts_.end(); ++unwit){

        for (unsigned int i = 0; i < (*unwit).m_.getNTri(); i++){
            
            const Triangle& tpres = (*unwit).m_.getTriangle(i);
//            const int tpres_orig3D = (*unwit).m_.getOrigTri(i);

            trcnt++;

            // Vector2f vt0,vt1,vt2;
            // vertices of the triangle
            const Vector2f vt0 = (*unwit).m_.getVertex(tpres.getIndex(0));
            const Vector2f vt1 = (*unwit).m_.getVertex(tpres.getIndex(1));
            const Vector2f vt2 = (*unwit).m_.getVertex(tpres.getIndex(2));

            float xmax, xmin, ymax, ymin;

            // maximum x value
            xmax = vt0(0) > vt1(0) ? vt0(0) : vt1(0);
            xmax = xmax > vt2(0) ? xmax : vt2(0);
            // minimum x value
            xmin = vt0(0) < vt1(0) ? vt0(0) : vt1(0);
            xmin = xmin < vt2(0) ? xmin : vt2(0);
            // maximum y value
            ymax = vt0(1) > vt1(1) ? vt0(1) : vt1(1);
            ymax = ymax > vt2(1) ? ymax : vt2(1);
            // minimum y value
            ymin = vt0(1) < vt1(1) ? vt0(1) : vt1(1);
            ymin = ymin < vt2(1) ? ymin : vt2(1);

            unsigned int xmaxp = findPosGrid(xmax, 0, realWidth_, imWidth_);
            unsigned int xminp = findPosGrid(xmin, 0, realHeight_, imHeight_);
            unsigned int ymaxp = findPosGrid(ymax, 0, realWidth_, imWidth_);
            unsigned int yminp = findPosGrid(ymin, 0, realHeight_, imHeight_);


            const int vt0_orig3D = (*unwit).m_.getOrigVtx(tpres.getIndex(0));
            const int vt1_orig3D = (*unwit).m_.getOrigVtx(tpres.getIndex(1));
            const int vt2_orig3D = (*unwit).m_.getOrigVtx(tpres.getIndex(2));

            for (unsigned int colp = xminp; colp <= xmaxp; colp++){
                for (unsigned int rowp = yminp; rowp <= ymaxp; rowp++){
                    if (colp == imWidth_ || rowp == imHeight_) continue;
                    // pixel center is calculated
                    Vector2f pixcenter;
                    pixcenter(0) = (float)(colp + 0.5) * maxwbyimwidth;
                    pixcenter(1) = (float)(rowp + 0.5) * maxwbyimwidth;

                    // if the pixel is inside the triangle or we are in the frontier
                    if (_pix_triangle(rowp, colp) != -1){
 
                        const Vector2f R = vt2 - vt1;
                        const Vector2f vt2vt0 = vt2-vt0;
                        const Vector2f vt1vt0 = vt1-vt0;

                        // E and D are the proyections of the pixel center in
                        // two of the sides of the triangle following a parallel
                        // line to the third side
                        Vector2f E = lineIntersect(vt0, vt2vt0, pixcenter, R);
                        Vector2f D = lineIntersect(vt0, vt1vt0, pixcenter, R);

                        const float vt0_E = (E - vt0).norm();
                        const float vt0_vt2 = (vt2 - vt0).norm();
                        const float E_D = (D - E).norm();
                        const float E_F = (pixcenter - E).norm();
                        
                        float ro, delta;
                        if (pixcenter == vt0){
                            ro = 0;
                            delta = 0;
                        } else if (pixcenter == vt1){
                            ro = 1;
                            delta = 1;
                        } else if (pixcenter == vt2){
                            ro = 1;
                            delta = 0;
                        } else { // ro and delta should be € [0,1]
                            ro = std::max(vt0_E/vt0_vt2, 0.0f);
                            ro = std::min(ro, 1.0f);
                            delta = std::max(E_F/E_D, 0.0f);
                            delta = std::min(delta, 1.0f);
                        }

                        // Weights for each vertex    
                        const float weight0 = 1-ro; // delta*(1-ro) + (1-delta)*(1-ro) = 1 -ro
                        const float weight1 = delta*ro;
                        const float weight2 = (1-delta)*ro;


                        // we calculate the rate for the pixel for each camera
                        for (unsigned int c = 0; c < nCam_; c++){
//                            if (cameras_[c].tri_ratings_[tpres_orig3D] == 0){
//                                pix_ratings[c] = 0;
//                                continue;
//                            }

                            const float vt0rat = cameras_[c].vtx_ratings_[vt0_orig3D];
                            const float vt1rat = cameras_[c].vtx_ratings_[vt1_orig3D];
                            const float vt2rat = cameras_[c].vtx_ratings_[vt2_orig3D];
                            // this expression comes from a triple linear interpolation of the vertex ratings
                            const float Frat =  weight0 * vt0rat + weight1 * vt1rat + weight2 * vt2rat;
                            pix_ratings[c] = Frat;
                        }

                        // Best cameras are assigned
                        ratings_cam.clear();
                        for (unsigned int c = 0; c < nCam_; c++) {
                            if (pix_ratings[c] != 0){
                                ratings_cam.insert(std::pair<float,int>(pix_ratings[c],c));
                            }
                        }
                        // Number of cameras to mix is the minimun between:
                        // our input value and the number of cameras available for the current pixel
                        const unsigned int tomix = ratings_cam.size() < (unsigned int) num_cam_mix_ ? ratings_cam.size() : num_cam_mix_;


                        // Calculation of the weights
                        std::vector<int> cameras_order;
                        std::vector<float> weights_order;
                        if (tomix != 0) {
                            unsigned int p;
                            float sumratings = 0;
                            cameras_order.resize(tomix);
                            weights_order.resize(tomix);

                            // Naïve way of calculating weights... could be improved
                            std::multimap<float, int>::iterator it;
                            for (it = ratings_cam.end(), p = 0; p < tomix; ++p) {
                                it--;
                                sumratings += (*it).first;
                                cameras_order[p] = (*it).second;
                            }
                            for (it = ratings_cam.end(), p = 0; p < tomix; ++p) {
                                it--;
                                weights_order[p] = (*it).first/sumratings;
                            }
                        }

                        // Color Assignment:
                        // 2D
                        // u,v, coordinates are calculated
                        const Vector2f pix_uv = uvPtri(pixcenter, vt0, vt1, vt2);

                        // 3D
                        // pixcenter of the 3D image is calculated
                        // Vector pixcenter3D, vA, vB, vC, vAB, vAC;
                        const Vector3f vA = mesh_.getVertex(vt0_orig3D);
                        const Vector3f vB = mesh_.getVertex(vt1_orig3D);
                        const Vector3f vC = mesh_.getVertex(vt2_orig3D);
                        const Vector3f vAB = vB-vA;
                        const Vector3f vAC = vC-vA;

                        // we use the 2D u,v, coodinates to assign the 3D pixcenter
                        const Vector3f pixcenter3D = vAB * pix_uv(1) + vAC * pix_uv(0) + vA; //
                        // Colors
                        Color col;

                        // If no camera sees the triangle...
                        if (tomix == 0){
                            if (highlightOcclusions_){
                                imout.setColor(Color(255,255,0),rowp,colp);
                            } else {
                                // This should do something else than painting them black...
                                // but currently it does not do anything else
                                imout.setColor(Color(0,0,0),rowp,colp);
                            }
                            continue;
                        }

                        for (unsigned int p = 0; p < tomix; p++) {
                            int camera = cameras_order[p];
                            float weight = weights_order[p];

                            // cache stuff
                            std::string imageName = imageList_[camera];
                            if (imageCache_.find(imageName) == imageCache_.end()){
                                loadImageToCache(imageName);
                            }

                            // Vector3f v = pixcenter3D.CoordTransform(&cam[camera]);
                            const Vector2f v_st = cameras_[camera].transform2uvCoord(pixcenter3D);
                            // Projection coordinates
                            const float proj_s = v_st(0);
                            const float proj_t = v_st(1);

                            if (proj_s < 0.0 || proj_t < 0.0){ // This may happen and it's very wrong
                                continue;
                            }

                            float image_row = (float)imageCache_[imageName].getHeight() - proj_t;
                            float image_col = proj_s;
                            
                            // In case a rounding error gives us a pixel outside the image
                            image_row = std::min (image_row, (float)imageCache_[imageName].getHeight());
                            image_col = std::min (image_col, (float)imageCache_[imageName].getWidth());
                            image_row = std::max (image_row, 0.0f);
                            image_col = std::max (image_col, 0.0f);

                            if (p == 0) { // Difference : = vs. +=
                                col = imageCache_[imageName].interpolate(image_row, image_col, BILINEAR) * weight;
                                //col = imageCache_[imageName].interpolate(image_row, image_col) * weight;
                            } else {
                                col += imageCache_[imageName].interpolate(image_row, image_col, BILINEAR) * weight;   
                                //col += imageCache_[imageName].interpolate(image_row, image_col) * weight;   
                            }
                        }

                        // color is assigned to the pixel
                        imout.setColor(col, rowp, colp);
                    }
                }
            }

            if (0 == trcnt % 1024) {
                std::cerr << "\r" << (float)trcnt/nTri_*100 << std::setw(4) << std::setprecision(4) << "% of triangles colored. ";
                std::cerr << (float)imageCache_.size()/imageCacheSize_ * 100 << std::setw(4) << std::setprecision(4) << "% of cache usage (";
                std::cerr << imageCache_.size() << "/" << imageCacheSize_ << ").      " << std::flush;
            }
        }
    }

    std::cerr << "\n";

    return imout;

}

Image Multitexturer::colorFlatCharts(const ArrayXXi& _pix_triangle){

    std::vector<Color> colorPool;
    // pre-defined colors
    colorPool.push_back(Color(255,0,0));
    colorPool.push_back(Color(255,255,0));
    colorPool.push_back(Color(255,255,255));
    colorPool.push_back(Color(0,255,0));
    colorPool.push_back(Color(0,0,255));


    Image imout =  Image (imHeight_, imWidth_);

    std::vector<Chart>::iterator unwit;

    int trcnt = 0;
    for (unwit = charts_.begin(); unwit != charts_.end(); ++unwit){

        const int col_index = rand() % colorPool.size();
        Color chart_col = colorPool[col_index];


        for (unsigned int i = 0; i < (*unwit).m_.getNTri(); i++){
            
            const Triangle& tpres = (*unwit).m_.getTriangle(i);

            trcnt++;

            // Vector2f vt0,vt1,vt2;
            // vertices of the triangle
            const Vector2f& vt0 = (*unwit).m_.getVertex(tpres.getIndex(0));
            const Vector2f& vt1 = (*unwit).m_.getVertex(tpres.getIndex(1));
            const Vector2f& vt2 = (*unwit).m_.getVertex(tpres.getIndex(2));

            float xmax, xmin, ymax, ymin;

            // maximum x value
            xmax = vt0(0) > vt1(0) ? vt0(0) : vt1(0);
            xmax = xmax > vt2(0) ? xmax : vt2(0);
            // minimum x value
            xmin = vt0(0) < vt1(0) ? vt0(0) : vt1(0);
            xmin = xmin < vt2(0) ? xmin : vt2(0);
            // maximum y value
            ymax = vt0(1) > vt1(1) ? vt0(1) : vt1(1);
            ymax = ymax > vt2(1) ? ymax : vt2(1);
            // minimum y value
            ymin = vt0(1) < vt1(1) ? vt0(1) : vt1(1);
            ymin = ymin < vt2(1) ? ymin : vt2(1);

            unsigned int xmaxp = findPosGrid(xmax, 0, realWidth_, imWidth_);
            unsigned int xminp = findPosGrid(xmin, 0, realHeight_, imHeight_);
            unsigned int ymaxp = findPosGrid(ymax, 0, realWidth_, imWidth_);
            unsigned int yminp = findPosGrid(ymin, 0, realHeight_, imHeight_);


            for (unsigned int colp = xminp; colp <= xmaxp; colp++){
                for (unsigned int rowp = yminp; rowp <= ymaxp; rowp++){
                    if (_pix_triangle(rowp, colp) != -1 ){
                        imout.setColor(chart_col, rowp, colp);
                    }
                }
            }

            std::cerr << "\r" << (float)trcnt/nTri_*100 << std::setw(4) << std::setprecision(4) << "% of triangles colored. ";
        }
    }

    std::cerr << "\n";


    return imout;

}

void Multitexturer::exportCamColorMesh(unsigned int _camIndex) {

    if (_camIndex > nCam_) _camIndex = nCam_ -1;

    std::stringstream outname;
    outname << "test_col_";
    outname << _camIndex;
    outname << ".obj";

    std::vector<Color> colors (nVtx_);

    float maxRating = 0.0;
    for (unsigned int i = 0; i < nVtx_; i++){
        if (cameras_[_camIndex].vtx_ratings_[i] > maxRating){
            maxRating = cameras_[_camIndex].vtx_ratings_[i];
        }
    }

    // We code the color with hue value

    for (unsigned int i = 0; i < nVtx_; i++){

        float hue = 240 - 240 * cameras_[_camIndex].vtx_ratings_[i] / maxRating; // from 0 tp 240º to avoid purple
        Color col = Color::hsv2rgb(hue, 1, 1);
        colors  [i] = col * 255;
    }

//    mesh_.writeColorPerVertexOBJ("test_col.obj", colors);
    mesh_.writeColorPerVertexOBJ(outname.str(), colors);

}

void Multitexturer::dilateAtlasCV(const ArrayXXi& _pix_triangle, Image& _image) const{

    cv::Mat mask (imHeight_, imWidth_, CV_8UC1, cv::Scalar(255)); 
    cv::Mat inImage (imHeight_, imWidth_, CV_8UC3, cv::Scalar(0,0,0));

    std::cerr << "Dilating charts... ";

    for (unsigned int row = 0; row < imHeight_; row++){
        for (unsigned int col = 0; col < imWidth_; col++){
            if (_pix_triangle(row,col) != -1){
                const uchar v = 0;
                mask.at<uchar>(imHeight_ - row - 1,col) = v; // inverted axis in opencv
            }
            const Color color = _image.getColor(row, col);
            const cv::Vec3b v((unsigned char)color.getBlue(),(unsigned char)color.getGreen(),(unsigned char)color.getRed());
            inImage.at<cv::Vec3b>(imHeight_ - row - 1,col) = v;
        }
    }

    cv::Mat outImage; 
    cv::inpaint(inImage, mask, outImage, 1, cv::INPAINT_TELEA);

    for (unsigned int row = 0; row < imHeight_; row++){
        for (unsigned int col = 0; col < imWidth_; col++){
            if (_pix_triangle(row,col) == -1){
                const cv::Vec3b color = outImage.at<cv::Vec3b>(imHeight_ - row - 1, col);
                const Color fcolor ((float) color(2), (float) color(1), (float) color(0));
                _image.setColor(fcolor, row, col);
            }
        }
    }

    std::cerr << "done!" << std::endl;
}

void Multitexturer::dilateAtlasCV2(const ArrayXXi& _pix_triangle, Image& _image) const{

    cv::Mat mask (imHeight_, imWidth_, CV_8UC1, cv::Scalar(255)); 
    cv::Mat inImage (imHeight_, imWidth_, CV_8UC3, cv::Scalar(0,0,0));

    std::cerr << "Dilating charts... ";

    for (unsigned int row = 0; row < imHeight_; row++){
        for (unsigned int col = 0; col < imWidth_; col++){
            if (_pix_triangle(row,col) != -1){
                const uchar v = 0;
                mask.at<uchar>(imHeight_ - row - 1,col) = v; // inverted axis in opencv
            }
            const Color color = _image.getColor(row, col);
            const cv::Vec3b v((unsigned char)color.getBlue(),(unsigned char)color.getGreen(),(unsigned char)color.getRed());
            inImage.at<cv::Vec3b>(imHeight_ - row - 1,col) = v;
        }
    }

    cv::Mat mask2 = mask.clone();
    cv::Mat eroder  = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
    cv::erode(mask, mask, eroder);

    cv::Mat finalmask (imHeight_, imWidth_, CV_8UC1, 255); 
    finalmask = mask2 - mask;

    cv::Mat outImage; 
    cv::inpaint(inImage, finalmask, outImage, 1, cv::INPAINT_TELEA);

    for (unsigned int row = 0; row < imHeight_; row++){
        for (unsigned int col = 0; col < imWidth_; col++){
            if (_pix_triangle(row,col) == -1){
                const cv::Vec3b color = outImage.at<cv::Vec3b>(imHeight_ - row - 1, col);
                const Color fcolor ((float) color(2), (float) color(1), (float) color(0));
                _image.setColor(fcolor, row, col);
            }
        }
    }

    std::cerr << "done!" << std::endl;
}

void Multitexturer::dilateAtlas(ArrayXXi& _pix_frontier, Image& _image, unsigned int _nIter) const {

    std::cerr << "Dilation process started...\n";
    Color current;

    for (unsigned int cnt = 0; cnt < _nIter; cnt++){
        // First vertically
        for (unsigned int col = 0; col < imWidth_; col++){
            unsigned int prev = INT_MAX;

            for (unsigned int row = 1; row < imHeight_ - 1; row++){
                // if the pixel explored already has a color -> continue
                if (_pix_frontier(row,col) < 0){ // -1 (frontier), or -2 (internal)
                    continue;
                // if the pixel explred doesn't have a color but the following does
                } else if (_pix_frontier(row+1, col) != 0){
                    current = _image.getColor(row+1, col);
                    _pix_frontier(row, col) = -4;
                    _image.setColor(current, row, col);
                } else if ((_pix_frontier(row-1, col) != 0) && (row-1 != prev)){
                    current = _image.getColor(row-1,col);
                    _pix_frontier(row, col) = -4;
                    _image.setColor(current, row, col);
                    prev = row;
                } 
            }
        }

        // Then horizontally
        for (unsigned int row = 0; row < imHeight_; row++){
            unsigned int prev = INT_MAX;
            
            for (unsigned int col = 1; col < imWidth_ - 1; col++){
                if (_pix_frontier(row, col) < 0){
                    continue;
                } else if (_pix_frontier(row, col+1) != 0){
                    current = _image.getColor(row, col+1);
                    _pix_frontier(row, col+1) = -3;
                    _image.setColor(current, row, col);
                } else if ((_pix_frontier(row, col-1) != 0) && (col-1 != prev)){
                    current = _image.getColor(row, col-1);
                    _pix_frontier(row, col-1) = -3;
                    _image.setColor(current, row, col);
                    prev = col;
                } 
            }
        }

        std::cerr << "\r" << (float)(cnt+1)/_nIter*100 << std::setw(4) << std::setprecision(4) << "%      ";

    }

    std::cerr << "\ndone!" << std::endl;
}

void Multitexturer::exportTexturedModel(){

    std::cerr << "Subdivided model: " << mesh_.getNTri() << " " << mesh_.getNVtx() << std::endl;
    mesh_ = origMesh_; // we export the original mesh, and not the subdivided one
    std::cerr << "Resulting  model: " << mesh_.getNTri() << " " << mesh_.getNVtx() << std::endl;


    if (out_extension_ == OBJ){
        mesh_.writeOBJ(fileNameOut_, fileNameTexOut_);
    } else if (out_extension_ == VRML){
        mesh_.writeVRML(fileNameOut_, fileNameTexOut_);
    } else if (out_extension_ == PLY){
        mesh_.writePLY(fileNameOut_, fileNameTexOut_);
    } else {
        std::cerr << "Unknown extension!" << std::endl;
    }

}

void Multitexturer::exportColorPerVertexModel(const std::vector<Color>& _vertexcolors){
    mesh_.writeColorPerVertexOBJ(fileNameOut_, _vertexcolors);
}

MappingMode Multitexturer::getMappingMode() const {
    return m_mode_;
}


void Multitexturer::exportOBJcharts(const std::string& _fileName){

    std::ofstream outMesh(_fileName.c_str());

    int vtxindex = 0;

    for (unsigned int c = 0; c < charts_.size(); c++){

        const Chart thischart = charts_[c];

        for (unsigned int i = 0; i < thischart.m_.getNVtx(); i++){
            const Vector2f& current = thischart.m_.getVertex(i);
            outMesh << "v " << current(0) << " " << current(1) << " 0\n";
        }
        for (unsigned int i = 0; i < thischart.m_.getNTri(); i++){
            const Vector3i& current = thischart.m_.getTriangle(i).getIndices();
            outMesh << "f";
            for (unsigned int j = 0; j < 3; j++){
                outMesh << " " << current(j)+1+vtxindex; // OBJ indices start at 1
            }
            outMesh << "\n";        
        }
        vtxindex += thischart.m_.getNVtx();

    }

    outMesh.close();
}



