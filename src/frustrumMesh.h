#include <vector>

#include "CGL/CGL.h"
#include "pointMass.h"

using namespace CGL;
using namespace std;

class Triangle {
public:
    Triangle(Vector3D* p1, Vector3D* p2, Vector3D* p3)
        : p1(p1), p2(p2), p3(p3) {}

    // Static references to constituent mesh objects
    Vector3D* p1;
    Vector3D* p2;
    Vector3D* p3;

    Halfedge* halfedge;
}; // struct Triangle

class Edge {
public:
    Halfedge* halfedge;
}; // struct Edge

class Halfedge {
public:
    Edge* edge;
    Halfedge* next;
    Halfedge* twin;
    Triangle* triangle;
    Vector3D* pos;
}; // struct Halfedge

class FrustrumMesh {
public:
    ~FrustrumMesh() {}

    vector<Triangle*> triangles;
}; // struct ClothMesh
