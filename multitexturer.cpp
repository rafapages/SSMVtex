#include <stdlib.h>

#include "multitexturer.h"

Multitexturer::Multitexturer(){

    ca_mode_ = AREA_OCCL;
    m_mode_ = TEXTURE;
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

    std::vector<char> opts;
    int index = 1;

    // We first read options
    for (; index < argc ; ++index){

        char* opt = argv[index];
        char c;
        if (opt[0] == '-'){ // options starting with '-' or '--'

            switch (c = opt[1]) {
            case 'n':			ca_mode_ = NORMAL_VERTEX;				opts.push_back(c); break;
            case 'b':			ca_mode_ = NORMAL_BARICENTER;			opts.push_back(c); break;
            case 'a':			ca_mode_ = AREA;						opts.push_back(c); break;
            case 'l':			ca_mode_ = AREA_OCCL;					opts.push_back(c); break;

            case '1':			num_cam_mix_ = 1;						opts.push_back(c); break;
            case '2':			num_cam_mix_ = 2;						opts.push_back(c); break;
            case '3':			num_cam_mix_ = 3;						opts.push_back(c); break;
            case '4':			num_cam_mix_ = 4;						opts.push_back(c); break;
            case '5':			num_cam_mix_ = 5;						opts.push_back(c); break;
            case '6':			num_cam_mix_ = 6;						opts.push_back(c); break;
            case '7':			num_cam_mix_ = 7;						opts.push_back(c); break;
            case '8':			num_cam_mix_ = 8;						opts.push_back(c); break;
            case '9':			num_cam_mix_ = 9;						opts.push_back(c); break;

            case 'e':			m_mode_ = NONE;							opts.push_back(c); break;
            case 'u':			m_mode_ = COLOR;							opts.push_back(c); break;
            case 't':			m_mode_ = TEXTURE;						opts.push_back(c); break;
            case 'p':           m_mode_ = POINT;                         opts.push_back(c); break;

            case 'm':           in_mode_ = MESH;                         opts.push_back(c); break;
            case 's':           in_mode_ = SPLAT;                        opts.push_back(c); break;

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
                        //                        stringValue += opt[i];
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
                    std::stringstream ss;
                    ss << stringValue;
                    ss >> intValue;
                    //                    cerr << intValue << endl;
                    dimension_ = intValue;

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
        fileNameOut_.clear();
        fileNameTexOut_.clear();
        std::string optionlist;
        for (std::vector<char>::iterator it = opts.begin(); it != opts.end(); it++){
            optionlist += '-';
            optionlist += *it;
        }

        // We remove the '.obj' extension from the input file name
        fileNameOut_ = fileNameIn_.substr(0, fileNameIn_.length()-4);

        // And we add the optionlist as part of the name, and the new extensions
        fileNameOut_ += optionlist;
        fileNameTexOut_ = fileNameOut_;
        fileNameOut_ += ".wrl";
        fileNameTexOut_ += ".jpg";
    }

    std::cerr << "Output files will be: " << std::endl;
    std::cerr << fileNameOut_ << std::endl;
    std::cerr << fileNameTexOut_ << std::endl;

}

void Multitexturer::printHelp(){    
    std::string help[] = {
        "",
        "Multitexmap, a 3D mesh multi-texturing system.",
        "",
        "Usage:  texmap [options] <fileNameIn> <fileNameCam> <fileNameImageList>",
        "\t[<fileNameOut> <fileNameTexOut>]",
        "",
        "\t<fileNameIn> is the name of the input text file containing the 3D scene",
        "\t  described in OBJ format. Extension : obj.",
        "\t<fileNameCam> is the name of the input text file containing the camera",
        "\t  calibration matrices. Extension: txt.",
        "\t<fileNameImageList> is the name of the input text file containing the",
        "\t  list of images that will be used for texturing the mesh. Extension: txt.",
        "\t<fileNameOut> is the name of the output text file that'll be created,",
        "\t  containing the same scene in VRML97 format -- if not specified, it'll",
        "\t  be built by appending \".wrl + options\" to <fileNameIn>. Extensions:",
        "\t  wrl or vrml.",
        "\t<fileNameTexOut> is the name of the output customized texture",
        "\t  that'll be created, -- if not specified, it'll be built by appending",
        "\t  \".jpg + options\" to <fileNameIn>. Extension: jpg.",
        "",
        "Options:",
        "-{n|b|a|l}\tAssing cameras to triangles using (n) their normals, (b) their",
        "\t\tnormals using the baricenter technique, (a) their area, (l)",
        "\t\tor their area taking occlusions into account. Default: l.",
        "-{m|s}\t\tInput value is (m) common 3D mesh or (s) a splat based 3D mesh. Default: m",
        "-{1-9}\t\tIf B option has been chosen, this argument indicates the",
        "\t\tmaximum amount of images per triangle used in order to create",
        "\t\tthe cumtomized texture. Default: 1.",
        "-{e|u|p|t}\tShow (e) a naked mesh, (u) a mesh colored per triangle, (p) a mesh",
        "\t\tcolored per vertex or (t) a mesh with textures in the VRML97 file. Default: t.",
        "--faceCam=<imageFileName>   in case there a frontal image showing the subject's face.",
        "--alpha=<alpha> alpha is the cutoff value of the normal weighting",
        "                function, in the interval (0, 1). Default: 0.5. ",
        "--beta=<beta>   beta is the curvature of the normal weighting",
        "                function, in the interval [0, inf). Default: 1.",
        "--dimension=<dimension> resolution of the output image measured in pixels.",
        "--cache=<cachesize> size of the image cache. Default: 75.",
        "-h\t\tPrint this help message."};

    for (unsigned int i = 0; i < sizeof(help) / sizeof(help[0]); ++i) {
        std::cerr << help[i] << "\n";
    }

    exit(-1);

}
