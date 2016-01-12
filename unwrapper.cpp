#include "unwrapper.h"

#include <iomanip>


void Unwrapper::unwrapMesh(const Mesh3D& _mesh, std::vector<Chart>& _charts){

    // This value avoids zeros in some operations
    const float EPSILON = 1e-6;


	std::vector<unsigned int> adj_count (_mesh.getNTri(), 0); // we start with zero neighbors per triangle
	std::vector<int> triNeighbor(_mesh.getNTri()*3);

	findTriangleNeighbors(_mesh, adj_count, triNeighbor);

    std::cerr << "Unwrapping mesh..." << std::endl;

    // Array which determines is a triangle has already been used
    std::vector<bool> usedTri(_mesh.getNTri(), false);

    // Order value assigned to each Chart
    int unw_order = 0;


    // Each chart starts with a seed triangle which is picked randomnly
    // from the list of triangles to avoid small charts formed by contiguos triangles. 
    
    // To keep track of the used triangle indices, we have uTri
    std::vector<int> uTri;
    std::vector<int>::iterator usit;
    for (unsigned int i = 0; i < _mesh.getNTri(); i++){
        uTri.push_back(i);
    }

    int numftri= uTri.size();
    int tri_count = 0;

    // While the number of unused triangles is still bigger than zero
    while (numftri > 0){

        numftri = uTri.size();

        // We pick a random triangle here, of index "q"
        const int randtri = rand() % numftri;
        int q = uTri[randtri];

        tri_count++;
        numftri--;

        // The iterator points to the triangle to delete in the list
        usit = uTri.begin() + randtri;
        uTri.erase(usit);

        // If it hasn't been used, it becomes the seed of a new chart,
        // stored in unw:
        Chart unw;

        int gen = 0; // generation : the date of birth of the edge we are processing now
        int date = 0; // date increases every time we add an edge to the perimeter

        // Step 1 : place the seed triangle in the 2D mesh

        // It extracts the three components of the triangle and searchs
        // for the longest edge of the first triangle (q)
        const Triangle thistri = _mesh.getTriangle(q);
        const Vector3f v0 = _mesh.getVertex(thistri.getIndex(0));
        const Vector3f v1 = _mesh.getVertex(thistri.getIndex(1));
        const Vector3f v2 = _mesh.getVertex(thistri.getIndex(2));

        float eleng[3];
        int le;
        eleng[0] = (v0-v1).norm();
        eleng[1] = (v1-v2).norm();
        eleng[2] = (v2-v0).norm();
        le = eleng[1] > eleng [0] ? 1 : 0;
        le = eleng[2] > eleng[le] ? 2 : le;

        // We make the seed triangle lie on its longest side, so 
        // it minimizes the area of the bounding box in case of a 
        // chart of just one triangle, but it is not really necessary. 

        // Assign A_w, B_w and E_w
        // A_w : first vertex of longest edge
        // B_w : second vertex of longest edge
        // E_w : the other vertex

        Vector3f A_w, B_w, E_w;
        int vtxCase;

        if (le == 0) {
            A_w = v0;
            B_w = v1;
            E_w = v2;
            vtxCase = 0;
        } else if (le == 1) {
            A_w = v1;
            B_w = v2;
            E_w = v0;
            vtxCase = 1;
        } else {
            A_w = v2;
            B_w = v0;
            E_w = v1;
            vtxCase = 2;
        }

        // Two sides of the triangle
        const Vector3f x_w = B_w - A_w;
        const Vector3f e_w = E_w - A_w;

        float mx = x_w.norm();
        float me = e_w.norm();

        // We don't want to divide by zero
        if (mx==0.0){
            mx = EPSILON;
        }
        if (me==0.0){
            me = EPSILON;
        }

        // If the new 2D vertices of the triangle are a, b and c:
        // a = (0,0)
        // b = (mx, 0)
        // c = (proj(e_w -> x_w), |e_w - x_w/mx·proj(e_w -> x_w)|) // Important: x_w should be normalized

        const Vector2f a_flat (0,0);
        const Vector2f b_flat (mx,0);
        // proj(e_w -> x_w) = me·cos(alpha) = (e_w · x_w) / mx
        const float dp = x_w.dot(e_w);
        const float proj = dp / mx;
        const Vector2f c_flat (proj, (e_w - x_w.normalized() * proj).norm());

       // The new vertices and triangle are added to the mesh

        unw.m_.addVector(a_flat, thistri.getIndex(vtxCase));
        unw.m_.addVector(b_flat, thistri.getIndex((vtxCase + 1) % 3));
        unw.m_.addVector(c_flat, thistri.getIndex((vtxCase + 2) % 3));

        const Triangle seed(0,1,2);

        unw.m_.addTriangle(seed, q);
        usedTri[q] = true;

        unw.setNEdgePos(0);

        // Step 2 : Add the three edges of the triangle to our Edge list
        for (unsigned int c = 0; c < 3; c++){

            Edge ed;
            ed.a = thistri.getIndex(c);
            ed.b = thistri.getIndex((c+1)%3);

            switch (vtxCase){
            case 0:
                ed.pa = seed.getIndex(c);
                ed.pb = seed.getIndex((c+1)%3);
                break;
            case 1:
                ed.pa = seed.getIndex((c+2)%3);
                ed.pb = seed.getIndex(c);
                break;
            case 2:
                ed.pa = seed.getIndex((c+1)%3);
                ed.pb = seed.getIndex((c+2)%3);
                break;
            }


            for (unsigned int j=0; j < adj_count[q]; j++){
                const Triangle t1 = _mesh.getTriangle(triNeighbor[3*q+j]);

                // If the neighbor triangle really shares the edge
                if( ((t1.getIndex(0) == ed.a)  &&  (t1.getIndex(1) == ed.b)) ||
                    ((t1.getIndex(1) == ed.a)  &&  (t1.getIndex(2) == ed.b)) ||
                    ((t1.getIndex(2) == ed.a)  &&  (t1.getIndex(0) == ed.b)) ||
                    ((t1.getIndex(0) == ed.b)  &&  (t1.getIndex(1) == ed.a)) ||
                    ((t1.getIndex(1) == ed.b)  &&  (t1.getIndex(2) == ed.a)) ||
                    ((t1.getIndex(2) == ed.b)  &&  (t1.getIndex(0) == ed.a))  ){
                    // and if it hasn't been used before
                    if(!usedTri[triNeighbor[3*q+j]]){
                        // we add that triangle as candidate for that position
                        ed.Candidate = triNeighbor[3*q+j];
                        unw.increaseNEdgePos();
                    } else {
                        // If there is no valid candidate, then we set it to -1
                        ed.Candidate = -1;
                    }
                    break;
                }
            }

            ed.birth = date++;
            ed.Present = q;

            unw.perimeter_.push_back(ed);
        }

        // Step 3: Fill the rest of the chart with triangles

        // While the number of candidate edges is still above zero
        while (unw.getNEdgePos() > 0) {

            // Step 3.1: we choose en Edge born in generation "gen"

            std::list<Edge>::iterator itedge;
            for (itedge = unw.perimeter_.begin(); itedge != unw.perimeter_.end() ; itedge++){
                if ( ((*itedge).birth) == gen)
                    break;
            }
            Edge e = *itedge;

            gen++;

            // If it doesn't have a candidate, reevaluate "while" condition
            if (itedge->Candidate == -1)
                continue;

            unw.decreaseNEdgePos();

            // If its candidate (not yet added when the edge was created)
            // has been added to the 2D mesh, reevaluate "while" condition
            if (usedTri[e.Candidate])
                continue;

            // Step 3.2 : Check if the new triangle overlaps with the chart

            // Hypothetical new vertex and edges

            const Triangle cand = _mesh.getTriangle(e.Candidate);
            int newVtx;
            const int c0 = cand.getIndex(0);
            const int c1 = cand.getIndex(1);
            const int c2 = cand.getIndex(2);

            if ( ((e.a == c0) && (e.b == c1))  ||  ((e.b == c0) && (e.a == c1)) ) {
                newVtx = c2;
            } else if ( ((e.a == c1) && (e.b == c2))  ||  ((e.b == c1) && (e.a == c2)) ) {
                newVtx = c0;
            } else if ( ((e.a == c2) && (e.b == c0))  ||  ((e.b == c2) && (e.a == c0)) ) {
                newVtx = c1;
            }

            // New hypothetical edges of the new triangle

            Edge ed1, ed2;

            // We have to respect the order in the perimeter, so...
            ed1.a = e.a;
            ed1.b = newVtx;
            ed1.pa = e.pa;
            ed1.pb =  unw.m_.getNVtx();

            ed2.a = newVtx;
            ed2.b = e.b;
            ed2.pa = unw.m_.getNVtx();
            ed2.pb = e.pb;

            // Candidate for ed1
            for (unsigned int j = 0; j < adj_count[e.Candidate]; j++){
                const Triangle t1 = _mesh.getTriangle(triNeighbor[3*(e.Candidate)+j]);
                if( ((t1.getIndex(0) == ed1.a)  &&  (t1.getIndex(1) == ed1.b)) ||
                    ((t1.getIndex(1) == ed1.a)  &&  (t1.getIndex(2) == ed1.b)) ||
                    ((t1.getIndex(2) == ed1.a)  &&  (t1.getIndex(0) == ed1.b)) ||
                    ((t1.getIndex(0) == ed1.b)  &&  (t1.getIndex(1) == ed1.a)) ||
                    ((t1.getIndex(1) == ed1.b)  &&  (t1.getIndex(2) == ed1.a)) ||
                    ((t1.getIndex(2) == ed1.b)  &&  (t1.getIndex(0) == ed1.a))  ){
                    if(!usedTri[triNeighbor[3*(e.Candidate)+j]])
                        ed1.Candidate = triNeighbor[3*(e.Candidate)+j];
                    else
                        ed1.Candidate = -1;
                    break;
                }
            }

            // Candidate for ed2
            for (int j=0; j<adj_count[e.Candidate]; j++){
                const Triangle t1 = _mesh.getTriangle(triNeighbor[3*(e.Candidate)+j]);
                if( ((t1.getIndex(0) == ed2.a)  &&  (t1.getIndex(1) == ed2.b)) ||
                    ((t1.getIndex(1) == ed2.a)  &&  (t1.getIndex(2) == ed2.b)) ||
                    ((t1.getIndex(2) == ed2.a)  &&  (t1.getIndex(0) == ed2.b)) ||
                    ((t1.getIndex(0) == ed2.b)  &&  (t1.getIndex(1) == ed2.a)) ||
                    ((t1.getIndex(1) == ed2.b)  &&  (t1.getIndex(2) == ed2.a)) ||
                    ((t1.getIndex(2) == ed2.b)  &&  (t1.getIndex(0) == ed2.a))  ){
                    if(!usedTri[triNeighbor[3*(e.Candidate)+j]])
                        ed2.Candidate = triNeighbor[3*(e.Candidate)+j];
                    else
                        ed2.Candidate = -1;
                    break;
                }
            }

            // Similarly as we did in Step 1: we calculate the 2D position of these edges
            // 3D
            // double ang;

            const Vector3f x_na = _mesh.getVertex(e.b) - _mesh.getVertex(e.a); // 3D equivalent of edge "e"
            const Vector3f e_na = _mesh.getVertex(newVtx) - _mesh.getVertex(e.a); // 3D equivalent of edge "ed1"
            double xna = x_na.norm();
            double ena = e_na.norm(); // REMEMBER: Same magnitude in 3D and 2D

            // We still don't want to divide by zero
            if (xna == 0.0)
                xna=EPSILON;
            if (ena == 0.0)
                ena=EPSILON;    

            const double dpn = x_na.dot(e_na);
            const double proj = dpn / xna;
            // const double nproj = proj / xna; // This value represents the proportion of the projection with respect to vector x_na

            // 2D
            // const Vector2f x_na_flat = unw.m_.getVertex(e.pb) - unw.m_.getVertex(e.pa); // 2D equivalent of edge "e"
            // const Vector2f x_na_perp (-x_na_flat(1), x_na_flat(0));
            // // We are searching for the 2D position of newVtx so, its components k1, k2 with respect edge e are:
            // const Vector2f k1 = proj * x_na_flat / xna;
            // const Vector2f k2 = (e_na - x_na.normalized()*proj).norm() * x_na_perp / xna;
            // const Vector2f nv = unw.m_.getVertex(e.pa) + k1 + k2; // 2D version of newVtx



            // -----------------------------------------------------------------------------
            // This is the old way to calculate the point:

            double dpnxnaena = dpn/(xna*ena);
            dpnxnaena = dpnxnaena < -1.0 ? -1.0 : dpnxnaena; // double precision
            dpnxnaena = dpnxnaena >  1.0 ?  1.0 : dpnxnaena;
            double ang = acos(dpnxnaena); // REMEMBER: Same angle in 3D and 2D

            // 2D

            Vector2f pab;
            double dpu, angu, angr;

            pab = unw.m_.getVertex(e.pb) - unw.m_.getVertex(e.pa); // 2D equivalent of edge "e"
            dpu = pab(0);

            double dpuinvxna = dpu/xna;
            dpuinvxna = dpuinvxna < -1.0 ? -1.0 : dpuinvxna; // double precision
            dpuinvxna = dpuinvxna >  1.0 ?  1.0 : dpuinvxna;
            angu = acos(dpuinvxna); // Angle of 2D equivalent of edge "e"

            if (pab(1) < 0)
                angu *= -1;
            angr = angu-ang; // Angle of 2D equivalent of edge "ed1"
            double vnx, vny;
            Vector2f nv;

            vnx = ena*cos(angr); // Horizontal coordinate of 2D equivalent of edge "ed1"
            vny = ena*sin(angr); // Vertical coordinate of 2D equivalent of edge "ed1"
            Vector2f nvu((float)vnx, (float)vny);

            // "nv" is the position of the hypothetical new vertex

            nv = nvu + unw.m_.getVertex(e.pa); // newVtx in 2D
            // nv.orig3D = newVtx;
            //-----------------------------------------------------------------------------

            // Check if the new edges intersect with any other

            bool overlap = false;

            Vector2f ed1a = unw.m_.getVertex(ed1.pa);
            Vector2f ed2b = unw.m_.getVertex(ed2.pb);

            // First: Edge ed1
            // This edge cannot intersect with the actual edge or with the previous one

            std::list<Edge>::iterator forb = itedge;
            if (forb == unw.perimeter_.begin()) {
                forb = unw.perimeter_.end();
                forb--;
            } else {
                forb--;
            }

            std::list<Edge>::iterator traveler;

            for (traveler = unw.perimeter_.begin(); traveler != unw.perimeter_.end(); ++traveler)  {
                if ((traveler == itedge) || (traveler == forb))
                    continue;
                Edge ec = *traveler;
                const Vector2f ec_a = unw.m_.getVertex(ec.pa);
                const Vector2f ec_b = unw.m_.getVertex(ec.pb);
                if (vectorIntersec(ec_a, ec_b, ed1a, nv)){
                    overlap = true;
                    break;
                }
            }

            if (overlap)
                continue;

            // Second: ed2: this edge cannot interesect with the actual edge and with the following
            // so we skip these two

            forb = itedge;
            forb++;
            if (forb == unw.perimeter_.end())
                forb = unw.perimeter_.begin();

            for (traveler = unw.perimeter_.begin(); traveler != unw.perimeter_.end(); ++traveler)  {
                if ((traveler == itedge) || (traveler == forb))
                    continue;
                Edge ec = *traveler;
                const Vector2f ec_a = unw.m_.getVertex(ec.pa);
                const Vector2f ec_b = unw.m_.getVertex(ec.pb);
                if (vectorIntersec(ec_a, ec_b, nv, ed2b)){
                    overlap = true;
                    break;
                }
            }

            if (overlap)
                continue;

            // We get to this point if the new triangle doesn't overlap the 2D mesh
            // We still have to check whether the area ratio is acceptable

            // Step 3.3 : Check the area ratio

            float hypoth_bbox_area = unw.m_.getHypotheticalBBoxArea(nv);

            float triarea = ((nv-ed1a)(0) * (ed2b-ed1a)(1) - (nv-ed1a)(1) * (ed2b-ed1a)(0)) / 2; // OJO CON LA NOMENCLATURA AQUÍ
            float hypoth_tri_area = unw.m_.getTriArea () + triarea;

            //_
            float relation__meshbbox_area = (float)(0.7/200) * unw.m_.getNTri ();
            //_

            float current_bbox_area = unw.getHeight() * unw.getWidth();
            float current_tri_area  = unw.m_.getTriArea();
            float current_relation__meshbbox_area = current_tri_area / current_bbox_area;

            //float relation__meshbbox_area = 0.7 * (unw.m.GetNTri() > 10 ? 1.0 : 0.3);
            //float relation__meshbbox_area = 0.5;

            //                if ((unw.m.GetNTri() > 5) &&  ((hypoth_tri_area/hypoth_bbox_area)   <   current_relation__meshbbox_area   )) {
            //                    continue;
            //                }
            if (   (hypoth_tri_area/hypoth_bbox_area)   <   relation__meshbbox_area   ){
                continue;
            }

            if (   unw.m_.getNTri() == 200   ) {
                continue;
            }

            // Step 3.4 : If all tests are passed, we add the vertex, triangle, and edges

            int newVtx2D = unw.m_.getNVtx();

            unw.m_.addVector(nv, newVtx);
            const Triangle newtri(e.pa, newVtx2D, e.pb);
            unw.m_.addTriangle(newtri, e.Candidate );

            usedTri[itedge->Candidate] = true;

            tri_count++;

            int tri2del = itedge->Candidate;
            usit=uTri.begin();

            int int2del;
            for (unsigned int cn= 0; cn != _mesh.getNTri(); cn++){
                if (uTri[cn]==tri2del){
                    int2del = cn;
                    break;
                }
            }

            for (unsigned int cn= 0; cn != _mesh.getNTri(); cn++){
                if (cn!=int2del){
                    usit++;
                } else {
                    break;
                }
            }
            numftri--;
            uTri.erase(usit);



            std::cerr << "\r" << (float)tri_count/_mesh.getNTri()*100 << std::setw(4) << std::setprecision(4) << "%      "<< std::flush;
            
            // the old Edge is now deleted, and the new ones are inserted in its place
            itedge = unw.perimeter_.erase(itedge);
            ed1.birth = date++;

            ed1.Present = e.Candidate;
            ed2.Present = e.Candidate;
            ed2.birth = date++;
            unw.perimeter_.insert(itedge, ed1);
            unw.perimeter_.insert(itedge, ed2);
            if (ed1.Candidate != -1)
                unw.increaseNEdgePos();
            if (ed2.Candidate != -1)
                unw.increaseNEdgePos();
        }
        // unw.order = unw_order++;
        unw.setOrder(unw_order++);
        Vector2f origin(0.0,0.0);
        unw.displace(origin - unw.m_.getBBoxMin());
        _charts.push_back(unw);
    }


    // it is necessary to add some offset around the unwraps for the later dilation
//    float prc = (float)0.040;
    float prc = (float)0.010;
    std::vector<Chart>::iterator ito;
    float maxArea = 0.0;
    float maxWidth = 0.0;
    for(ito = _charts.begin(); ito != _charts.end(); ++ito){
        float area = (*ito).getHeight()*(*ito).getWidth();
        if (area > maxArea){
            maxArea = area;
            maxWidth = (*ito).getWidth();
        }
    }

    float offset = (maxWidth)*prc;
    for(ito = _charts.begin(); ito != _charts.end(); ++ito){
        (*ito).addOffset2BoundingBox(offset);
    }

    std::cerr << "\rdone!   " << std::endl;


}

void Unwrapper::unwrapSplats(const Mesh3D& _mesh, std::vector<Chart>& _charts){

    std::cerr << "Analizing splats...";

    int unw_order = 0;
    for (unsigned int i = 0; i < _mesh.getNTri(); i++){

        const Triangle thistri = _mesh.getTriangle(i);
        // The current chart is created
        Chart unw;

        Vector3f v0, v1, v2, v3;

        v0 = _mesh.getVertex(thistri.getIndex(0));
        v1 = _mesh.getVertex(thistri.getIndex(1));
        v2 = _mesh.getVertex(thistri.getIndex(2));
        v3 = _mesh.getVertex(_mesh.getTriangle(i+1).getIndex(1)); // This is the vertex of the second triangle which is not shared

        // We create 2 2D (planar) triangles and 4 2D vertices
        Vector2f vp0(0.0,0.0);
        Vector2f vp1((v1-v0).norm(), 0.0);
        Vector2f vp2(vp1(0), (v3-v0).norm());
        Vector2f vp3(0.0,vp2(1));

        unw.m_.addVector(vp0, thistri.getIndex(0));
        unw.m_.addVector(vp1, thistri.getIndex(1));
        unw.m_.addVector(vp2, thistri.getIndex(2));
        unw.m_.addVector(vp3, _mesh.getTriangle(i+1).getIndex(1));

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
        e3.b = _mesh.getTriangle(i+1).getIndex(1);
        e3.pa = unw.m_.getTriangle(1).getIndex(0);
        e3.pb = unw.m_.getTriangle(1).getIndex(1);
        e3.birth = date++;
        e3.Present = i+1;
        e3.Candidate = -1;


        e4.a = _mesh.getTriangle(i+1).getIndex(1);
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

        _charts.push_back(unw);
        i++; // We have used two triangles, so we need to increase the counter again

    }

    // it is necessary to add some offset around the unwraps for the later dilation
    float prc = 0.5;
    std::vector<Chart>::iterator ito;
    float maxArea = 0.0;
    float maxWidth = 0.0;
    for(ito = _charts.begin(); ito != _charts.end(); ito++){
        float area = (*ito).getHeight() * (*ito).getWidth();
        if (area > maxArea){
            maxArea = area;
            maxWidth = (*ito).getWidth();
        }
    }

    float offset = (maxWidth)*prc;
    for(ito = _charts.begin(); ito != _charts.end(); ito++){
        (*ito).addOffset2BoundingBox(offset);
    }

    std::cerr << " done!" << std::endl;
    std::cerr << "Number of charts: " << _charts.size() << std::endl;
}


// This method is too trivial and probably very time consuming,
// something more efficient should be implemented.
void Unwrapper::findTriangleNeighbors(const Mesh3D& _mesh, std::vector<unsigned int>& _adj_count, std::vector<int>& _triNeighbor){

	// which triangles contain each vertex
    std::vector<int> *vtx2tri = new std::vector<int> [_mesh.getNVtx()];
    for (unsigned int i = 0; i < _mesh.getNTri(); i++) {
    	const Triangle thistri = _mesh.getTriangle(i);
        for (unsigned int j = 0; j < 3; j++){
            vtx2tri[thistri.getIndex(j)].push_back(i);
        }
    }


    for(unsigned int i = 0; i < _mesh.getNVtx(); i++){
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

                va0 = _mesh.getTriangle(*ita).getIndex(0);
                va1 = _mesh.getTriangle(*ita).getIndex(1);
                va2 = _mesh.getTriangle(*ita).getIndex(2);
                vb0 = _mesh.getTriangle(*itb).getIndex(0);
                vb1 = _mesh.getTriangle(*itb).getIndex(1);
                vb2 = _mesh.getTriangle(*itb).getIndex(2);


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


bool Unwrapper::vectorIntersec(const Vector2f& _v1a, const Vector2f& _v1b, const Vector2f& _v2a, const Vector2f& _v2b)  {
    
    const Vector2f v1 = _v1b - _v1a;
    const Vector2f v2 = _v2b - _v2a;

    const float v = ( v1(0) * (_v1a(1) - _v2a(1)) + v1(1) * (_v2a(0) - _v1a(0)) ) / ( v2(1) * v1(0) - v2(0) * v1(1));
    const float u = ( _v2a(0) + v2(0) * v - _v1a(0) ) / v1(0);

    if ( ( (u > 0) && (u < 1) ) && ( (v > 0) && (v < 1) ) ){
        return true;
    }
    return false;
}


