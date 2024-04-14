#include <vector>

#include "CGL/CGL.h"
#include "pointMass.h"

using namespace CGL;
using namespace std;

class FrustTriangle {
public:
    FrustTriangle(Vector3D p1, Vector3D p2, Vector3D p3, Vector3D uv1, Vector3D uv2, Vector3D uv3)
        : p1(p1), p2(p2), p3(p3), uv1(uv1), uv2(uv2), uv3(uv3) {}

    // Static references to constituent mesh objects
    Vector3D p1;
    Vector3D p2;
    Vector3D p3;

    // UV values for each of the points.
  // Uses Vector3D for convenience. This means that the z dimension
  // is not used, and xy corresponds to uv.
    Vector3D uv1;
    Vector3D uv2;
    Vector3D uv3;

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
