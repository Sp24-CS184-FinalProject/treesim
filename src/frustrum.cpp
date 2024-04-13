#include <iostream>
#include <math.h>
#include <cmath>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"
#include "frustrum.h"


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
    
    


}