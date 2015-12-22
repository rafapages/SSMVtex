#ifndef MULTITEXTURER_H
#define MULTITEXTURER_H

#include <iostream>

#include "mesh3d.h"
#include "camera.h"

typedef enum {NONE, COLOR, TEXTURE, POINT} MappingMode;
typedef enum {NORMAL_VERTEX, NORMAL_BARICENTER, AREA, AREA_OCCL} CamAssignMode;
//typedef enum {LIGHT, SHADOW, DARK} VtxMode;
typedef enum {OBJ, TXT, WRL, OBJCAM, PNG, UNKNOWN} Extension;
//typedef enum {RECYCLE, NEWTEX} Option;
typedef enum {MESH, SPLAT} InputMode;

class Multitexturer
{

public:

    Multitexturer();
    ~Multitexturer();

    // Parses the command line and sets the options
    void parseCommandLine(int argc, char *argv[]);


private:

    void printHelp(); // DUDA: tiene sentido una función void sin parámetros ser definida const?


    Mesh3D mesh_;

    std::vector<Camera> cameras_;
    std::vector<std::string> imageList_;

    // Options - Default values
    CamAssignMode ca_mode_; //  AREA_OCCL
    MappingMode m_mode_; //  TEXTURE
    InputMode in_mode_; //  MESH
    int num_cam_mix_; //  1
    float alpha_; // 0.5
    float beta_; // 1.0
    unsigned int dimension_; // 10000000
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
