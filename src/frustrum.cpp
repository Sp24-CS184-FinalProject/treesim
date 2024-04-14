#include <iostream>
#include <math.h>
#include <cmath>
#include <random>
#include <vector>

#include "collision/plane.h"
#include "collision/sphere.h"
#include "frustrum.h"
#include "frustrumMesh.h"


using namespace std;



Frustrum::Frustrum(Vector3D baseOriginPos, double baseRadius, Vector3D topOriginPos, double topRadius, double height, int numSides) {
    this->baseOriginPos = baseOriginPos;
    this->baseRadius = baseRadius;
    this->topOriginPos = topOriginPos;
    this->topRadius = topRadius;
    this->height = height;
    this->numSides = numSides;
    
    buildFrustrumMesh();
}


void Frustrum::buildFrustrumMesh() {
    FrustrumMesh* mesh = new FrustrumMesh();
    vector<Triangle*> triangles;

    std::vector<Vector3D> baseVertices;
    std::vector<Vector3D> topVertices;
    std::vector<Vector3D> lateralVertices;

    // Calculate vertices for the bottom & top of circle
    for (int i = 0; i < numSides; ++i) {
        double angle = 2 * PI * i / numSides;
        double x = baseOriginPos.x + baseRadius * cos(angle);
        double y = baseOriginPos.y + baseRadius * sin(angle);
        baseVertices.push_back(Vector3D(x, y, baseOriginPos.z));
    }

    // Calculate vertices for the top circle 
    for (int i = 0; i < numSides; ++i) {
        double angle = 2 * PI * i / numSides;
        double x = topOriginPos.x + topRadius * cos(angle);
        double y = topOriginPos.y + topRadius * sin(angle);
        topVertices.push_back(Vector3D(x, y, topOriginPos.z));
        
    }

    // Calculate vertices for the lateral surface using the previous two sets of vertices
    // lateral vertices appear on the line connecting two vertices on the top and bottom circles (make sure angle is same)
    // For now assume we only need one lateral vertex per pair of vertices on the circles.
    for (int i = 0; i < numSides; ++i) {
        Vector3D baseVert = baseVertices[i];
        Vector3D topVert = topVertices[i];
        Vector3D line = (topVert - baseVert).unit();
        Vector3D lateralVert = baseVert + (height / 2) * line; // move from base vert towards topVert moving half the distance between them
        lateralVertices.push_back(lateralVert);

    }
    
    

    //Build  Lateral Facing Triangles Based on Calculated Vertices.
    vector<FrustTriangle *> tris;
    // First Row of Triangles 
    for (int x = 0; x < numSides; x++) {
        /*                     *
        *  A ---------  B   *
        *             /        *
        *  |         /   |     *
        *  |        /    |     *
        *  |       /     |     *
        *  |      /      |     *
        *  |     /       |     *
        *  |    /        |     *
        *      /               *
        *  C ----------  D   *
        *                      *
        */
        // Add triangles that connect last index to 0th index
        if (x == numSides - 1) {
            int y = 0;
            float u_min = x;
            u_min /= numSides - 1;
            float u_max = x + 1;
            u_max /= numSides - 1;
            // In current implementation there are only three levels in terms of height ( top vertices, middle vertices, bottom vertices) 
            float v_min = y;
            v_min /= 3;
            float v_max = y + 1;
            v_max /= 3;

            Vector3D A = topVertices[x];
            Vector3D B = topVertices[0];
            Vector3D C = lateralVertices[x];
            Vector3D D = lateralVertices[0];


            Vector3D uv_A = Vector3D(u_min, v_min, 0);
            Vector3D uv_B = Vector3D(u_max, v_min, 0);
            Vector3D uv_C = Vector3D(u_min, v_max, 0);
            Vector3D uv_D = Vector3D(u_max, v_max, 0);

            // Both triangles defined by vertices in counter-clockwise orientation

            tris.push_back(new FrustTriangle(A, C, B,
                uv_A, uv_C, uv_B));
            tris.push_back(new FrustTriangle(B, C, D,
                uv_B, uv_C, uv_D));
            continue;
        }
        int y = 0;
        float u_min = x;
        u_min /= numSides - 1;
        float u_max = x + 1;
        u_max /= numSides - 1;
        // In current implementation there are only three levels in terms of height ( top vertices, middle vertices, bottom vertices) 
        float v_min = y;
        v_min /= 3;
        float v_max = y + 1;
        v_max /= 3;

        Vector3D A = topVertices[x];
        Vector3D B = topVertices[x + 1];
        Vector3D C = lateralVertices[x];
        Vector3D D = lateralVertices[x + 1];


        Vector3D uv_A = Vector3D(u_min, v_min, 0);
        Vector3D uv_B = Vector3D(u_max, v_min, 0);
        Vector3D uv_C = Vector3D(u_min, v_max, 0);
        Vector3D uv_D = Vector3D(u_max, v_max, 0);

        // Both triangles defined by vertices in counter-clockwise orientation
            
        tris.push_back(new FrustTriangle(A, C, B,
                uv_A, uv_C, uv_B));
        tris.push_back(new FrustTriangle(B, C, D,
                uv_B, uv_C, uv_D));
     }
    
    //Second Row of Lateral Triangles 
    for (int x = 0; x < numSides; x++) {
        /*                     *
        *  A ---------  B   *
        *             /        *
        *  |         /   |     *
        *  |        /    |     *
        *  |       /     |     *
        *  |      /      |     *
        *  |     /       |     *
        *  |    /        |     *
        *      /               *
        *  C ----------  D   *
        *                      *
        */
        // Add triangles that connect last index to 0th index
        if (x == numSides - 1) {
            int y = 1;
            float u_min = x;
            u_min /= numSides - 1;
            float u_max = x + 1;
            u_max /= numSides - 1;
            // In current implementation there are only three levels in terms of height ( top vertices, middle vertices, bottom vertices) 
            float v_min = y;
            v_min /= 3;
            float v_max = y + 1;
            v_max /= 3;

            Vector3D A = lateralVertices[x];
            Vector3D B = lateralVertices[0];
            Vector3D C = baseVertices[x];
            Vector3D D = baseVertices[0];


            Vector3D uv_A = Vector3D(u_min, v_min, 0);
            Vector3D uv_B = Vector3D(u_max, v_min, 0);
            Vector3D uv_C = Vector3D(u_min, v_max, 0);
            Vector3D uv_D = Vector3D(u_max, v_max, 0);

            // Both triangles defined by vertices in counter-clockwise orientation

            tris.push_back(new FrustTriangle(A, C, B,
                uv_A, uv_C, uv_B));
            tris.push_back(new FrustTriangle(B, C, D,
                uv_B, uv_C, uv_D));
            continue;
        }
        int y = 1;
        float u_min = x;
        u_min /= numSides - 1;
        float u_max = x + 1;
        u_max /= numSides - 1;
        // In current implementation there are only three levels in terms of height ( top vertices, middle vertices, bottom vertices) 
        float v_min = y;
        v_min /= 3;
        float v_max = y + 1;
        v_max /= 3;

        Vector3D A = lateralVertices[x];
        Vector3D B = lateralVertices[x + 1];
        Vector3D C = baseVertices[x];
        Vector3D D = baseVertices[x + 1];


        Vector3D uv_A = Vector3D(u_min, v_min, 0);
        Vector3D uv_B = Vector3D(u_max, v_min, 0);
        Vector3D uv_C = Vector3D(u_min, v_max, 0);
        Vector3D uv_D = Vector3D(u_max, v_max, 0);

        // Both triangles defined by vertices in counter-clockwise orientation

        tris.push_back(new FrustTriangle(A, C, B,
            uv_A, uv_C, uv_B));
        tris.push_back(new FrustTriangle(B, C, D,
            uv_B, uv_C, uv_D));
    }

    // Add triangles on BaseCircle Surface
    for (int x = 0; x < numSides; x++) {
        int y = 0;
        float u_min = x;
        u_min /= numSides - 1;
        float u_max = x + 1;
        u_max /= numSides - 1;
        // In current implementation there are only three levels in terms of height ( top vertices, middle vertices, bottom vertices) 
        float v_min = y;
        v_min /= 3;
        float v_max = y + 1;
        v_max /= 3;

        Vector3D A = baseOriginPos;
        Vector3D B = baseVertices[x];
        Vector3D C;
        if (x == numSides - 1) { C = topVertices[0]; }
        else { Vector3D C = baseVertices[x + 1]; }
        


        Vector3D uv_A = Vector3D(u_min, v_min, 0);
        Vector3D uv_B = Vector3D(u_max, v_min, 0);
        Vector3D uv_C = Vector3D(u_min, v_max, 0);
        

        // Both triangles defined by vertices in counter-clockwise orientation
        tris.push_back(new FrustTriangle(A, C, B,
            uv_A, uv_C, uv_B));
    }

    // Add triangles on TopCircle Surface
    for (int x = 0; x < numSides; x++) {
        int y = 0;
        float u_min = x;
        u_min /= numSides - 1;
        float u_max = x + 1;
        u_max /= numSides - 1;
        // In current implementation there are only three levels in terms of height ( top vertices, middle vertices, bottom vertices) 
        float v_min = y;
        v_min /= 3;
        float v_max = y + 1;
        v_max /= 3;

        Vector3D A = topOriginPos;
        Vector3D B = topVertices[x];
        Vector3D C;
        if (x == numSides - 1) { C = topVertices[0]; }
        else { Vector3D C = topVertices[x + 1]; }
        



        Vector3D uv_A = Vector3D(u_min, v_min, 0);
        Vector3D uv_B = Vector3D(u_max, v_min, 0);
        Vector3D uv_C = Vector3D(u_min, v_max, 0);


        // Both triangles defined by vertices in counter-clockwise orientation
        tris.push_back(new FrustTriangle(A, C, B,
            uv_A, uv_C, uv_B));
    }

    //All Triangles Added, Now Assign to HalfEdge pointers
    // For each triangle in row-order, create 3 edges and 3 internal halfedges
    for (int i = 0; i < tris.size(); i++) {
        FrustTriangle* t = tris[i];

        // Allocate new halfedges on heap
        Halfedge* h1 = new Halfedge();
        Halfedge* h2 = new Halfedge();
        Halfedge* h3 = new Halfedge();

        // Allocate new edges on heap
        Edge* e1 = new Edge();
        Edge* e2 = new Edge();
        Edge* e3 = new Edge();

        // Assign a halfedge pointer to the triangle
        t->halfedge = h1;

        // Assign halfedge pointers to point masses
        t->pm1->halfedge = h1;
        t->pm2->halfedge = h2;
        t->pm3->halfedge = h3;

        // Update all halfedge pointers
        h1->edge = e1;
        h1->next = h2;
        h1->pm = t->pm1;
        h1->triangle = t;

        h2->edge = e2;
        h2->next = h3;
        h2->pm = t->pm2;
        h2->triangle = t;

        h3->edge = e3;
        h3->next = h1;
        h3->pm = t->pm3;
        h3->triangle = t;
    }

}