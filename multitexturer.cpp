#include <list>

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

    nCam_ = 0;
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
            case 'n':	ca_mode_ = NORMAL_VERTEX;		opts.push_back(c); break;
            case 'b':	ca_mode_ = NORMAL_BARICENTER;	opts.push_back(c); break;
            case 'a':	ca_mode_ = AREA;				opts.push_back(c); break;
            case 'l':	ca_mode_ = AREA_OCCL;			opts.push_back(c); break;

            case '1':	num_cam_mix_ = 1;	opts.push_back(c); break;
            case '2':	num_cam_mix_ = 2;	opts.push_back(c); break;
            case '3':	num_cam_mix_ = 3;	opts.push_back(c); break;
            case '4':	num_cam_mix_ = 4;	opts.push_back(c); break;
            case '5':	num_cam_mix_ = 5;	opts.push_back(c); break;
            case '6':	num_cam_mix_ = 6;	opts.push_back(c); break;
            case '7':	num_cam_mix_ = 7;	opts.push_back(c); break;
            case '8':	num_cam_mix_ = 8;	opts.push_back(c); break;
            case '9':	num_cam_mix_ = 9;   opts.push_back(c); break;

            case 'e':	m_mode_ = NONE;     opts.push_back(c); break;
            case 'u':	m_mode_ = COLOR;    opts.push_back(c); break;
            case 't':	m_mode_ = TEXTURE;  opts.push_back(c); break;
            case 'p':   m_mode_ = POINT;    opts.push_back(c); break;

            case 'm':   in_mode_ = MESH;    opts.push_back(c); break;
            case 's':   in_mode_ = SPLAT;   opts.push_back(c); break;

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
                    std::stringstream ss;
                    ss << stringValue;
                    ss >> intValue;
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
        // In case there is something still there
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

void Multitexturer::loadInputData(){
    readInputMesh();
    readCameraFile();
    readImageList();
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

}

void Multitexturer::evaluateCameraRatings(){

    for (unsigned int c = 0 ; c < nCam_ ; c++){
        cameras_[c].tri_ratings_.resize(mesh_.getNTri());
        cameras_[c].vtx_ratings_.resize(mesh_.getNVtx());
    }

    // This step will calculate every camera-triangle ratings
    // using the chosen system.
    switch (ca_mode_) {
    case NORMAL_VERTEX:
    case NORMAL_BARICENTER:
        evaluateNormal();
        break;
    case AREA:
        evaluateArea();
        break;
    case AREA_OCCL:
        evaluateAreaWithOcclusions(40);
        break;
    }

    // Create vtx2tri and tri2tri
    // vtx2vtx is a vector containing every direct neighbor for each vertex
    std::vector<int> *vtx2tri = new std::vector<int> [mesh_.getNVtx()];
    for (unsigned int i = 0; i < mesh_.getNTri(); i++) {
        for (unsigned int j = 0; j < 3; j++)
            // vtx2tri[tri[i].i[j]].push_back(i);
            vtx2tri[mesh_.getTriangle(i).getIndex(j)].push_back(i);
    }
    // tri2tri is a list containing every neigbor of eac triangle
    std::list<int> *tri2tri = new std::list<int> [mesh_.getNTri()];
    for (unsigned int i = 0; i < mesh_.getNVtx(); i++) {
        for (std::vector<int>::iterator ita = vtx2tri[i].begin(); ita != vtx2tri[i].end(); ++ita) {
            for (std::vector<int>::iterator itb = vtx2tri[i].begin(); itb != vtx2tri[i].end(); ++itb) {
                tri2tri[*ita].push_back(*itb);
                tri2tri[*itb].push_back(*ita);
            }
        }
    }

    for (unsigned int i = 0; i < mesh_.getNTri(); i++) {
        tri2tri[i].sort();
        tri2tri[i].unique();
    }

    // SmoothRatings(vtx2tri, tri2tri);
    // EvaluateWeightNormal();
    // SmoothRatings(vtx2tri, tri2tri);
    // EvaluateWeightNormal();
    // SmoothRatings(vtx2tri, tri2tri);
    // EvaluateWeightNormal();

    // PostProcessAreaOccl ();
    // EvaluateWeightNormal();


    // At this point, triangle ratings are already known,
    // and their average is calculated to set the vertex ratings 
    for(unsigned int c = 0; c < nCam_; c++){

        for(unsigned int i = 0; i < mesh_.getNVtx() ; i++){
            std::vector<int>::iterator it;
            float totrating = 0.0;
            int numTri = 0;
            for (it = vtx2tri[i].begin(); it != vtx2tri[i].end(); ++it){
                if (cameras_[c].tri_ratings_[*it] == 0.0){
                    totrating = 0.0;
                    break;
                }
                float rating = cameras_[c].tri_ratings_[*it];
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

}

void Multitexturer::evaluateNormal(){

    for (unsigned int i = 0; i < mesh_.getNTri(); i++) {

        const Triangle3D thistri = mesh_.getTriangle(i);
        // Find camera most orthogonal to this triangle
        Vector3f n = mesh_.getTriangleNormal(i);
        for (unsigned int j = 0; j < nCam_; j++) {
            
            Vector3f mf = mesh_.getVertex(thistri.getIndex(0));
            if (ca_mode_ == NORMAL_BARICENTER) {
                mf += mesh_.getVertex(thistri.getIndex(1));
                mf += mesh_.getVertex(thistri.getIndex(2));
                mf /= 3;
            }

            mf -= cameras_[j].getPosition();
            Vector3f nf = mf.normalized();
            const float dp = n.dot(nf);
            // In case the camera is facing back, the rating assigned is 0
            cameras_[j].tri_ratings_[i] = (dp < 0) ? ( -1 * dp) : 0;
        }
    }
} 


void Multitexturer::evaluateArea(){


    for (unsigned int i = 0; i < mesh_.getNTri(); i++) {
        
        const Triangle3D thistri = mesh_.getTriangle(i);
        Vector3f n = mesh_.getTriangleNormal(i);

        for (unsigned int j = 0; j < nCam_; j++) {
            
            cameras_[j].tri_ratings_[i] = 0;
            // Calculate dot product (dp), in order to discard backfacing
            // It only matters whether it is positive or negative
            Vector3f mf = mesh_.getVertex(thistri.getIndex(0));
            mf -= cameras_[j].getPosition();
            const float dp = mf.dot(n);
            
            if (dp < 0) {
                // Vertex projections to the image plane
                std::vector<Vector2f> uv_vtx;
                for (unsigned int k=0; k<3; k++) {
                    const Vector3f vc = mesh_.getVertex(thistri.getIndex(k));
                    Vector2f uv = cameras_[j].transform2uvCoord(vc);
                    uv_vtx.push_back(uv);
                }

                const Vector2f v0 = uv_vtx[0];
                const Vector2f v1 = uv_vtx[1];
                const Vector2f v2 = uv_vtx[2];
                float area = 0.5 * ((v0(1)-v2(1)) * (v1(0)-v2(0)) - (v0(0)-v2(0)) * (v1(1)-v2(1)));
                    //area = (v[0]-v[2]) * (u[1]-u[2]) - (u[0]-u[2]) * (v[1]-v[2]);
                cameras_[j].tri_ratings_[i] = area;

            } // else -> tri_ratings_ stays 0
        }
    }

}
void Multitexturer::evaluateAreaWithOcclusions(unsigned int _resolution){

    // Define grid search resolution in both dimensions
    const unsigned int grid_s_resol = _resolution;
    const unsigned int grid_t_resol = _resolution;

    // These arrays are calculated only once (here), because they are the
    // same for all cameras... that is why it is calculated before the
    // "for each camera" loop
    std::vector<float> tri_areamax (mesh_.getNTri(), 0.0);
    // which triangles contain each vertex
    std::vector<int> *vtx2tri = new std::vector<int> [mesh_.getNVtx()];

    for (unsigned int i = 0; i < mesh_.getNTri(); i++) {
        for (unsigned int j = 0; j < 3; j++){
            vtx2tri[mesh_.getTriangle(i).getIndex(j)].push_back(i);
        }
    }

    // This arrays are updated for each camera (this is just memory allocation)
    //    vtx_s : horizontal coordinate in texture of each vertex
    //    vtx_t : vertical coordinate in texture of each vertex
    //    vtx_pos_grid_s : horizontal position in of each vertex
    //    vtx_pos_grid_t : vertical position in grid of each vertex
    //    grid_vtx : vertices in each cell of the grid
    //    vtx_seen : for each vertex
    //       DARK : Not seen
    //       SHADOW : Not seen, covered by others
    //       LIGHT : Seen
    //    tri_area : projected area of each triangle
    //    tri_frontfacing : true if projected area > 0
    //    tri_alive : true (after processing each camera) if it can be seen
    //    grid_tri : triangles whose projection is included in each cell of the grid

    std::vector<float> vtx_s (mesh_.getNVtx());
    std::vector<float> vtx_t (mesh_.getNVtx());
    std::vector<unsigned int> vtx_pos_grid_s (mesh_.getNVtx());
    std::vector<unsigned int> vtx_pos_grid_t (mesh_.getNVtx());
    std::vector<int> *grid_vtx = new std::vector<int> [grid_s_resol * grid_t_resol];
    std::vector<VtxMode> vtx_seen (mesh_.getNVtx());
    std::vector<float> tri_area(mesh_.getNTri());
    std::vector<bool> tri_frontfacing (mesh_.getNTri());
    std::vector<bool> tri_alive (mesh_.getNTri());
    std::vector<int> *grid_tri = new std::vector<int> [grid_s_resol * grid_t_resol];



    // FOR EACH CAMERA
    for (unsigned int c = 0; c < nCam_; c++) {

        // STEP 1 : Kill backfacing triangles

        // Vertex projections
        for (unsigned int i = 0; i < mesh_.getNVtx(); i++) {
            Vector2f v = cameras_[c].transform2uvCoord(mesh_.getVertex(i));
            vtx_s[i] = v(0);
            vtx_t[i] = v(1);
        }

        // Calculate the area of each triangle and kill triangles
        // with a negative area
        for (unsigned int j = 0; j < mesh_.getNTri(); j++) {
            const Triangle3D vl = mesh_.getTriangle(j);
            tri_area[j] = (vtx_t[vl.getIndex(0)]-vtx_t[vl.getIndex(2)]) * (vtx_s[vl.getIndex(1)]-vtx_s[vl.getIndex(2)]) - (vtx_s[vl.getIndex(0)]-vtx_s[vl.getIndex(2)]) * (vtx_t[vl.getIndex(1)]-vtx_t[vl.getIndex(2)]);
            tri_alive[j] = tri_frontfacing[j] = tri_area[j] > 0;
        }


        // STEP 2: All vertices surrounded only by dead triangles -> DARK
        // Rest of them -> LIGHT

        for (unsigned int i = 0; i < mesh_.getNVtx(); i++) {
            bool tobekilled = true;
            for (std::vector<int>::iterator it = vtx2tri[i].begin(); it!=vtx2tri[i].end(); ++it) {
                if (tri_frontfacing[*it]) {
                    tobekilled = false;
                    break;
                }
            }
            if (tobekilled){
                vtx_seen[i] = DARK;
            } else {
                vtx_seen[i] = LIGHT;
            }
        }

        // THE NEXT STEP NEEDS THE GRIDS TO BE UPDATED

        // Clear grids
        for (unsigned int i = 0; i < grid_s_resol; i++) {
            for (unsigned int j = 0; j < grid_t_resol; j++) {
                grid_vtx[i*grid_s_resol+j].clear();
                grid_tri[i*grid_s_resol+j].clear();
            }
        }

        // Clasify LIGHT vertices in the vertex grid
        float min_s, max_s;
        float min_t, max_t;

        min_s = max_s = vtx_s[0];
        min_t = max_t = vtx_t[0];

        for (unsigned int i = 1; i < mesh_.getNVtx(); i++) {
            if (vtx_s[i] < min_s) {
                min_s = vtx_s[i];
            } else if (vtx_s[i] > max_s){
                max_s = vtx_s[i];
            } if (vtx_t[i] < min_t) {
                min_t = vtx_t[i];
            } else if (vtx_t[i] > max_t) {
                max_t = vtx_t[i];
            }
        }

        for (unsigned int i = 0; i < mesh_.getNVtx(); i++) {
            if (vtx_seen[i] == LIGHT) {
                vtx_pos_grid_s[i] = findPosGrid(vtx_s[i], min_s, max_s, grid_s_resol);
                vtx_pos_grid_t[i] = findPosGrid(vtx_t[i], min_t, max_t, grid_t_resol);
                grid_vtx[vtx_pos_grid_s[i]*grid_s_resol+vtx_pos_grid_t[i]].push_back(i);
            }
        }

        // Clasify living triangles in triangle grid
        for (unsigned int i = 0; i < mesh_.getNTri(); i++) {
            if (tri_alive[i]) {
                // unsigned int * vl = tri[i].i;
                const Triangle3D vl = mesh_.getTriangle(i);
                unsigned int min_pos_grid_s = findPosGrid(vtx_s[vl.getIndex(0)], min_s, max_s, grid_s_resol);
                unsigned int min_pos_grid_t = findPosGrid(vtx_t[vl.getIndex(0)], min_t, max_t, grid_t_resol);
                unsigned int max_pos_grid_s = min_pos_grid_s;
                unsigned int max_pos_grid_t = min_pos_grid_t;
                for (unsigned int j = 1; j < 3; j++) {
                    unsigned int pos_grid_s = findPosGrid(vtx_s[vl.getIndex(j)], min_s, max_s, grid_s_resol);
                    unsigned int pos_grid_t = findPosGrid(vtx_t[vl.getIndex(j)], min_t, max_t, grid_t_resol);
                    if (pos_grid_s < min_pos_grid_s)        min_pos_grid_s = pos_grid_s;
                    else if (pos_grid_s > max_pos_grid_s)   max_pos_grid_s = pos_grid_s;
                    if (pos_grid_t < min_pos_grid_t)        min_pos_grid_t = pos_grid_t;
                    else if (pos_grid_t > max_pos_grid_t)   max_pos_grid_t = pos_grid_t;
                }
                for (unsigned int s = min_pos_grid_s; s <= max_pos_grid_s; s++) {
                    for (unsigned int t = min_pos_grid_t; t <= max_pos_grid_t; t++)
                        grid_tri[s*grid_s_resol+t].push_back(i);
                }
            }
        }

        // STEP 3 : LIGHT vertices covered by living triangles or outside texture turn into SHADOW vertices
        for (unsigned int i = 0; i < mesh_.getNVtx(); i++) {
            if (vtx_seen[i] == LIGHT) {
                // candidates: triangles in same grid part, except for the ones incident to i
                std::vector<int> candidates = grid_tri[vtx_pos_grid_s[i]*grid_s_resol + vtx_pos_grid_t[i]];
                for (std::vector<int>::iterator it = candidates.begin(); it != candidates.end(); it++) {
                    // unsigned int * vl = tri[*it].i;
                    const Triangle3D vl = mesh_.getTriangle(*it);
                    if (vtx_in_tri(vtx_s[i], vtx_t[i], vtx_s[vl.getIndex(0)], vtx_t[vl.getIndex(0)], vtx_s[vl.getIndex(1)], vtx_t[vl.getIndex(1)], vtx_s[vl.getIndex(2)], vtx_t[vl.getIndex(2)])) {
                        // Check if triangle vertex is eclipsed
                        if (isVertexEclipsed(i, *it, c)){
                            vtx_seen[i] = SHADOW;
                        } else {
                            tri_alive[*it] = false;
                        }
                    }
                }
                if (vtx_s[i]<0 || (cameras_[c].getImageWidth() < vtx_s[i]) || vtx_t[i]<0 || (cameras_[c].getImageHeight() < vtx_t[i])){
                    vtx_seen[i] = SHADOW;
                }
            }
        }

        // STEP 4 : Kill triangles that touch SHADOW vertices

        for (unsigned int i = 0; i < mesh_.getNVtx(); i++) {
            if (vtx_seen[i] == SHADOW) {
                for (std::vector<int>::iterator it = vtx2tri[i].begin(); it != vtx2tri[i].end(); it++) {
                    tri_alive[*it] = false;
                }
            }
        }

        // STEP 5 : Process each living triangle

        // For each triangle, if it is still alive and its area is greater than areamax, update
        for (unsigned int i = 0; i < mesh_.getNTri(); i++){
            cameras_[c].tri_ratings_[i] = tri_alive[i] ? tri_area[i] : 0;
        }
    }

    delete [] vtx2tri;

    delete [] grid_vtx;
    delete [] grid_tri;
}

unsigned int Multitexturer::findPosGrid (float _x, float _min, float _max, unsigned int _resolution) {
    
    unsigned int d = (unsigned int) ((_x - _min) / (_max - _min) * _resolution);
    if (d == _resolution)
        return _resolution - 1;

    return d;
}

bool Multitexturer::isVertexEclipsed (int _v, int _t, int _c) const {

    Vector3f va = mesh_.getVertex(mesh_.getTriangle(_t).getIndex(0));
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

bool Multitexturer::vtx_in_tri (float pt_s, float pt_t, float a_s, float a_t,
                float  b_s, float  b_t, float c_s, float c_t) {

                    Vector3f A, B, C, P;
                    A(0) = a_s; A(1) = a_t; A(2) = 0;
                    B(0) = b_s; B(1) = b_t; B(2) = 0;
                    C(0) = c_s; C(1) = c_t; C(2) = 0;
                    P(0) = pt_s; P(1) = pt_t; P(2) = 0;

                    Vector3f v0, v1, v2;
                    v0 = C; v1 = B; v2 = P;
                    v0 -= A; 
                    v1 -= A;
                    v2 -= A;

                    float invDenom = 1 / (v0(0) * v1(1) - v1(0) * v0(1));
                    
                    float u = invDenom * (v1(1) * v2(0) - v1(0) * v2(1));
                    float v = invDenom * (v0(0) * v2(1) - v0(1) * v2(0));

                    return (u > 0) && (v > 0) && (u + v < 1);
}


