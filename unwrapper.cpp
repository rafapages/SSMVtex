#include "unwrapper.h"

Unwrapper::Unwrapper(){
}

Unwrapper::~Unwrapper(){
}

void Unwrapper::setInputMesh(const Mesh3D& _mesh){
	mesh_ = _mesh;
}

void Unwrapper::getCharts(std::vector<Chart>& _charts) {
	_charts = charts_;
}

void Unwrapper::unwrapMesh(){

	std::vector<unsigned int> adj_count (mesh_.getNTri(), 0); // we start with zero neighbors per triangle
	std::vector<int> triNeighbor(mesh_.getNTri()*3);

	findTriangleNeighbors(adj_count, triNeighbor);
}

void Unwrapper::unwrapSplats(){

    std::cerr << "Analizing splats...";

    int unw_order = 0;
    for (unsigned int i = 0; i < mesh_.getNTri(); i++){

        const Triangle thistri = mesh_.getTriangle(i);
        // The current chart is created
        Chart unw;

        Vector3f v0, v1, v2, v3;

        v0 = mesh_.getVertex(thistri.getIndex(0));
        v1 = mesh_.getVertex(thistri.getIndex(1));
        v2 = mesh_.getVertex(thistri.getIndex(2));
        v3 = mesh_.getVertex(mesh_.getTriangle(i+1).getIndex(1)); // This is the vertex of the second triangle which is not shared

        // We create 2 2D (planar) triangles and 4 2D vertices
        Vector2f vp0(0.0,0.0);
        Vector2f vp1((v1-v0).norm(), 0.0);
        Vector2f vp2(vp1(0), (v3-v0).norm());
        Vector2f vp3(0.0,vp2(1));

        unw.m_.addVector(vp0, thistri.getIndex(0));
        unw.m_.addVector(vp1, thistri.getIndex(1));
        unw.m_.addVector(vp2, thistri.getIndex(2));
        unw.m_.addVector(vp3, mesh_.getTriangle(i+1).getIndex(1));

        unw.m_.addTriangle(Triangle(0,1,2), i);
        unw.m_.addTriangle(Triangle(2,3,0), i+1);

        // Now we update the 4 Edges, though in this case they are unnecessary...
        Edge e1, e2, e3, e4;
        int date = 0; // date increases every time we add an edge to the perimeter

        e1.a = thistri.getIndex(0);
        e1.b = thistri.getIndex(1);
        e1.pa = unw.m_.getTriangle(0).getIndex(0);
        e1.pb = unw.m_.getTriangle(0).getIndex(1);
        e1.birth = date++;
        e1.Present = i;
        e1.Candidate = -1;


        e2.a = thistri.getIndex(1);
        e2.b = thistri.getIndex(2);
        e2.pa = unw.m_.getTriangle(0).getIndex(1);
        e2.pb = unw.m_.getTriangle(0).getIndex(2);
        e2.birth = date++;
        e2.Present = i;
        e2.Candidate = -1;


        e3.a = thistri.getIndex(2);
        e3.b = mesh_.getTriangle(i+1).getIndex(1);
        e3.pa = unw.m_.getTriangle(1).getIndex(0);
        e3.pb = unw.m_.getTriangle(1).getIndex(1);
        e3.birth = date++;
        e3.Present = i+1;
        e3.Candidate = -1;


        e4.a = mesh_.getTriangle(i+1).getIndex(1);
        e4.b = thistri.getIndex(0);
        e4.pa = unw.m_.getTriangle(1).getIndex(1);
        e4.pb = unw.m_.getTriangle(1).getIndex(2);
        e4.birth = date++;
        e4.Present = i+1;
        e4.Candidate = -1;

        unw.perimeter_.push_back(e1);
        unw.perimeter_.push_back(e2);
        unw.perimeter_.push_back(e3);
        unw.perimeter_.push_back(e4);

        Vector2f origin(0.0,0.0);
        unw.displace(origin - unw.m_.getBBoxMin());
        unw.setNEdgePos(0);
        unw.setOrder(unw_order++);

        charts_.push_back(unw);
        i++; // We have used two triangles, so we need to increase the counter again

    }

    // it is necessary to add some offset around the unwraps for the later dilation
    float prc = 0.5;
    std::vector<Chart>::iterator ito;
    float maxArea = 0.0;
    float maxWidth = 0.0;
    for(ito = charts_.begin(); ito != charts_.end(); ito++){
        float area = (*ito).getHeight() * (*ito).getWidth();
        if (area > maxArea){
            maxArea = area;
            maxWidth = (*ito).getWidth();
        }
    }

    float offset = (maxWidth)*prc;
    for(ito = charts_.begin(); ito != charts_.end(); ito++){
        (*ito).addOffset2BoundingBox(offset);
    }

    std::cerr << " done!" << std::endl;
    std::cerr << "Number of charts: " << charts_.size() << std::endl;
}


// This method is too trivial and probably very time consuming,
// something more efficient should be implemented.
void Unwrapper::findTriangleNeighbors(std::vector<unsigned int>& _adj_count, std::vector<int>& _triNeighbor){

	// which triangles contain each vertex
    std::vector<int> *vtx2tri = new std::vector<int> [mesh_.getNVtx()];
    for (unsigned int i = 0; i < mesh_.getNTri(); i++) {
    	const Triangle thistri = mesh_.getTriangle(i);
        for (unsigned int j = 0; j < 3; j++){
            vtx2tri[thistri.getIndex(j)].push_back(i);
        }
    }


    for(unsigned int i = 0; i < mesh_.getNVtx(); i++){
        std::vector<int>::iterator ita = vtx2tri[i].begin();

        for(; ita != vtx2tri[i].end(); ita++){
            std::vector<int>::iterator itb = vtx2tri[i].begin();

            for(; itb != vtx2tri[i].end(); itb++){

            	// in case both iterators are pointing to the same triangle
                if ((*ita)==(*itb))
                    continue;

                // Variables needed for the identification of adjacent triangles
                int va0, va1, va2;
                int vb0, vb1, vb2;

                va0 = mesh_.getTriangle(*ita).getIndex(0);
                va1 = mesh_.getTriangle(*ita).getIndex(1);
                va2 = mesh_.getTriangle(*ita).getIndex(2);
                vb0 = mesh_.getTriangle(*itb).getIndex(0);
                vb1 = mesh_.getTriangle(*itb).getIndex(1);
                vb2 = mesh_.getTriangle(*itb).getIndex(2);


                // In case an edge of each triangle is the same
                if ((((va0==vb0)||(va0==vb1)||(va0==vb2))&&((va1==vb0)||(va1==vb1)||(va1==vb2)))||
                    (((va0==vb0)||(va0==vb1)||(va0==vb2))&&((va2==vb0)||(va2==vb1)||(va2==vb2)))||
                    (((va2==vb0)||(va2==vb1)||(va2==vb2))&&((va1==vb0)||(va1==vb1)||(va1==vb2)))) {

                    // The values number of adj triangles is updated
                    // for both pointed triangles and the neighbor
                    // triangle index as well

                    // case it-A
                    switch (_adj_count[*ita]){
                    case 0:
                        _triNeighbor[3*(*ita)] = *itb;
                        _adj_count[*ita] = 1;
                        break;
                    case 1:
                        if (_triNeighbor[3*(*ita)] != *itb){
                            _triNeighbor[3*(*ita)+1] = *itb;
                            _adj_count[*ita] = 2;
                        }
                        break;
                    case 2:
                        if ((_triNeighbor[3*(*ita)] != *itb) && (_triNeighbor[3*(*ita)+1] != *itb)){
                            _triNeighbor[3*(*ita)+2] = *itb;
                            _adj_count[*ita] = 3;
                        }
                    }

                    // case it-B
                    switch (_adj_count[*itb]){
                    case 0:
                        _triNeighbor[3*(*itb)] = *ita;
                        _adj_count[*itb] = 1;
                        break;
                    case 1:
                        if (_triNeighbor[3*(*itb)] != *ita){
                            _triNeighbor[3*(*itb)+1] = *ita;
                            _adj_count[*itb] = 2;
                        }
                        break;
                    case 2:
                        if ((_triNeighbor[3*(*itb)] != *ita) && (_triNeighbor[3*(*itb)+1] != *ita)){
                            _triNeighbor[3*(*itb)+2] = *ita;
                            _adj_count[*itb] = 3;
                        }
                    }
                }
            }
        }
    }
    delete [] vtx2tri;

}


