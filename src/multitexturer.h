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

#ifndef MULTITEXTURER_H
#define MULTITEXTURER_H

#include <stdlib.h>
#include <map>
#include <list>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "camera.h"
#include "image.h"
#include "unwrapper.h"
#include "packer.h"

typedef enum {TEXTURE, POINT} MappingMode;
typedef enum {NORMAL_VERTEX, NORMAL_BARICENTER, AREA, AREA_OCCL} CamAssignMode;
typedef enum {LIGHT, SHADOW, DARK} VtxMode;
typedef enum {MESH, SPLAT} InputMode;
typedef enum {VRML, OBJ, PLY} OutputExtension;

class Multitexturer {

public:

    Multitexturer();
    virtual ~Multitexturer();

    // Parses the command line and sets the options
    void parseCommandLine(int argc, char *argv[]);

    // Loads the needed 
    void loadInputData();

    // Evaluate how each cameras sees each triangle
    void evaluateCameraRatings();

    // Turns a 3D mesh into a set of 2D charts that
    // are stored in the std::vector charts_
    void meshUnwrap();

    // Rearranges the 2D charts stored in charts_
    // so they are efficently packed
    void chartPacking();

    // Creates a texture atlas and colors it
    void chartColoring();

    //
    void exportTexturedModel();

    // TEST chart exporter: the packed charts are exported
    // as a flat 3D mesh in OBJ format
    void exportOBJcharts(const std::string& _fileName);



private:

    void printHelp(); // DUDA: tiene sentido una función void sin parámetros ser definida const?
    
    // Access input data
    void readInputMesh();
    void readCameraFile();
    void readImageList();


    // Geometry stuff ***
    // According to camera c, is vertex v eclipsed by triangle t ?
    bool isVertexEclipsed(int _v, int _t, int _c) const;
    // Samples x with respect to the given resolution
    unsigned int findPosGrid (float _x, float _min, float _max, unsigned int _resolution) const;
    // Checks if the point p is included in the triangle defined by vertices a, b and c
    bool isPinsideTri (const Vector2f& _p, const Vector2f& _a, const Vector2f& _b, const Vector2f& _c) const; 
    // This function returns a 2D Vector containing the u,v, image coordinates
    // of a point (_p) in relation to a triangle (_a, _b, _c)
    Vector2f uvPtri (const Vector2f& _p, const Vector2f& _a, const Vector2f& _b, const Vector2f& _c) const;
    // Returns the intersecting point of two lines defined by point _a and vector _va and _b and _vb
    Vector2f lineIntersect(const Vector2f& _a, const Vector2f& _va, const Vector2f& _b, const Vector2f& _vb) const;
 


    // Different ways to estimate camera weights:
    // They all fill the vector tri_ratings_ of each camera
    // 
    // Uses the normal of the triangle
    void evaluateNormal(); 
    // Uses the projected area of the triangle
    void evaluateArea();
    // Uses the projected area taking into account occlusions
    void evaluateAreaWithOcclusions(unsigned int _resolution);
    // Smoothes the values estimated so transitions are seamless
    void smoothRatings(std::list<int> *_tri2tri);
    // Weights the normals with respect to a function with
    // curvature beta_ and cutoff value alpha_
    void evaluateWeightNormal();
    // 
    void improveFaceRatings();


    // Finds a camera in the list and returns its position
    // if the camera is not found, -1 is returned
    int findCameraInList(const std::string& _fileName) const;

    // Loads the selected image to the cache
    void loadImageToCache(const std::string& _fileName);

    // Finds a face in the determined image
    // Returns true if found or false if not
    // Stores the corners of the box where the face is contained
    bool findFaceInImage(float& _face_min_x, float& _face_max_x, float& _face_min_y, float& _face_max_y) const;

    // Chart coloring functions:
    // This functions calculates the output image size
    void calculateImageSize();

    // Fills arrays _pix_frontier and _pix_triangle, which represent the final image
    void rasterizeTriangles(ArrayXXi& _pix_frontier, ArrayXXi& _pix_triangle);

    // Finds the borders of every chart and fills in the corresponding arrays
    void findChartBorders(Chart& _chart, ArrayXXi& _pix_frontier, ArrayXXi& _pix_triangle);

    // Performs the multi-texturing and returns a texture image
    Image colorTextureAtlas(const ArrayXXi& _pix_frontier, const ArrayXXi& _pix_triangle);

    // Dilates the atlases by inpainting the background
    void dilateAtlas(const ArrayXXi& _pix_triangle, Image& _image) const;




    // Input 3D mesh
    Mesh3D  mesh_;

    // Input files
    std::vector<Camera> cameras_;
    std::vector<std::string> imageList_;
    unsigned int nCam_;

    // Images are stored in a cache
    // so there are no memory issues
    std::map<std::string, Image> imageCache_;

    // Group of 2D charts created by unwrapping the 3D mesh
    std::vector<Chart> charts_;

    // Dimensions of the final image:
    // Real-scale floating point coordinates
    float realWidth_, realHeight_;
    // Image dimensions
    unsigned int imWidth_, imHeight_;

    // Options - Default values
    CamAssignMode ca_mode_; //  AREA_OCCL
    MappingMode m_mode_; //  TEXTURE
    InputMode in_mode_; //  MESH
    OutputExtension out_extension_; // VRML
    int num_cam_mix_; //  1
    float alpha_; // 0.5
    float beta_; // 1.0
    unsigned int dimension_; // 10,000,000
    unsigned int imageCacheSize_; // 75

    // File names
    std::string fileNameIn_;
    std::string fileNameCam_;
    std::string fileNameImageList_;
    std::string fileNameOut_;
    std::string fileNameTexOut_;
    std::string fileFaceCam_;

};

#endif // MULTITEXTURER_H
