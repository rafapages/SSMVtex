#include <iostream>

#include "multitexturer.h"

int main(int argc, char *argv[]){

    Multitexturer multitex;
    multitex.parseCommandLine(argc, argv);

    multitex.loadInputData();

    multitex.evaluateCameraRatings();

    multitex.meshUnwrap();
    multitex.chartPacking();

    multitex.exportOBJcharts("hola.obj");


    return 0;
}

