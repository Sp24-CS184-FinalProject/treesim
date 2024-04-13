
#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "CGL/vector3D.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "frustrumMesh.h"
#include "collision/collisionObject.h"
#include "spring.h"

using namespace CGL;

// Forward Declarations 
class Halfedge;

struct Frustrum {
	Frustrum() {}
	Frustrum(Vector3D baseOriginPos, double baseRadius, Vector3D topOriginPos, double topRadius, double height, int numSides)
		: baseOriginPos(baseOriginPos), baseOriginLastPos(baseOriginPos),
		topOriginPos(topOriginPos), topOriginLastPos(topOriginLastPos), numSides(numSides) {}
	~Frustrum();

	
	void buildFrustrumMesh();
	
	


	// Dynamic Values
		Vector3D baseOriginPos;
		Vector3D baseOriginLastPos;
		Vector3D topOriginPos;
		Vector3D topOriginLastPos;
		int numSides; // determines detail of mesh: number of vertices and triangles

	//Static Values 
		double baseRadius;
		double topRadius;
		double height;

	// Mesh Reference
		FrustrumMesh* mesh;
};