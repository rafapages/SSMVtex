/* 
 *  Copyright (c) 2017  Rafael Pagés
 *
 *  This file is part of SSMVtex
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <iostream>

#include "multitexturer.h"

int main(int argc, char *argv[]){

    Multitexturer multitex;
    multitex.parseCommandLine(argc, argv);

    MappingMode mode = multitex.getMappingMode(); // [TEXTURE, FLAT, VERTEX]

    multitex.loadInputData();

    if (mode == TEXTURE){

        multitex.meshUnwrap();
        multitex.chartPacking();

        multitex.chartColoring();
        multitex.exportTexturedModel();

    } else if (mode == FLAT) {

        // In case a FLAT coloring mode has been chosen,
        // it is not necessary to evaluate the camera ratings

        multitex.meshUnwrap();
        multitex.chartPacking();

        multitex.chartColoring();
        multitex.exportTexturedModel();

    } else if (mode == VERTEX) {
        
        // In case a color per VERTEX approach has been chose,
        // it is not necessary to unwrap and pack the 3D mesh

        std::vector<Color> colors;
        multitex.colorVertices(colors);

        multitex.exportColorPerVertexModel(colors);

    } else {

        std::cerr << "Unknown mapping mode: " << mode << std::endl;
        return 0;
    }

    return 0;
}

