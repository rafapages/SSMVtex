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

#include <iostream>

#include "multitexturer.h"

int main(int argc, char *argv[]){

    Multitexturer multitex;
    multitex.parseCommandLine(argc, argv);

    MappingMode mode = multitex.getMappingMode(); // [TEXTURE, FLAT, VERTEX]

    multitex.loadInputData();

    // In case a color per VERTEX approach has been chose,
    // it is not necessary to unwrap and pack the 3D mesh
    if (mode != VERTEX){
        multitex.meshUnwrap();
        multitex.chartPacking();
    }

    // In case a FLAT coloring mode has been chosen,
    // it is not necessary to evaluate the camera ratings
    if (mode != FLAT){
        multitex.evaluateCameraRatings();
    }

    if (mode == VERTEX){
        std::vector<Color> colors;
        multitex.colorVertices(colors);
        multitex.exportColorPerVertexModel(colors);
    } else {

        multitex.chartColoring();
        multitex.exportTexturedModel();
    }





    return 0;
}

