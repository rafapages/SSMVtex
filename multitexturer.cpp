#include "multitexturer.h"

Multitexturer::Multitexturer(){

    ca_mode_ = AREA_OCCL;
    m_mode_ = TEXTURE;
    option_ = NEWTEX;
    in_mode_ = MESH;
    num_cam_mix_ = 1;
    alpha_ = 0.5;
    beta_ = 1.0;
    dimension_ = 10000000;
    imageCacheSize_ = 75;

}

Multitexturer::~Multitexturer(){

}

void Multitexturer::parseCommandLine(int argc, char *argv[]){

}
