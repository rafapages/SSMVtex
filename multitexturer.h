#ifndef MULTITEXTURER_H
#define MULTITEXTURER_H

#include "mesh3d.h"
#include "camera.h"



class Multitexturer
{

public:

    Multitexturer();
    ~Multitexturer();




private:


    Mesh3D mesh_;

    std::vector<Camera> cameras_;
    std::vector<std::string> imageList_;


};

#endif // MULTITEXTURER_H
