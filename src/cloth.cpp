#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
  
    // Create Pinned Masses Boolean Vector
    vector<vector<bool>> pin(num_width_points, vector<bool>(num_height_points, false));
    for (auto point : pinned) {
        pin[point[0]][point[1]] = true;
    }
    double xSpace = static_cast<double>(width) / (num_width_points - 1);
    double otherSpace = static_cast<double>(height) / (num_height_points - 1);
    //Set the y coordinate for all point masses to 1 while varying positions over the xz plane
    if (this->orientation == HORIZONTAL) {
        for (int z = 0; z < num_height_points; z++) {
            for (int x = 0; x < num_width_points; x++) {
                Vector3D position = Vector3D(x * xSpace, 1, z * otherSpace);
                point_masses.emplace_back(PointMass(position, pin[x][z]));
            }
        }
    }
    else {
        //generate a small random offset between -1/1000 and 1/1000 for each point mass and use that as the z coordinate while varying positions over the xy plane
        for (int y = 0; y < num_height_points; y++) {
            for (int x = 0; x < num_width_points; x++) {
                double zOffSet = -0.001 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) / 0.002);
                Vector3D position = Vector3D(x * xSpace, y * otherSpace, zOffSet);
                point_masses.emplace_back(PointMass(position, pin[x][y]));
            }
        }
    }
    // add springs
    for (int j = 0; j < num_height_points; j++) {
        for (int i = 0; i < num_width_points; i++) {
            PointMass *p = &point_masses[j * num_width_points + i];

            // Structural Constraint with point mass to left ( i - 1) and above (j - 1)
            if (i - 1 >= 0) { springs.emplace_back(Spring(p, &point_masses[j * num_width_points + (i-1)], STRUCTURAL)); }
            if (j - 1 >= 0) { springs.emplace_back(Spring(p, &point_masses[(j - 1) * num_width_points + i], STRUCTURAL)); }

            // Shearing Constraint with point mass to diagonal upper left and upper right (i-1, j-1), (i+1, j-1)
            if (i - 1 >= 0 && j - 1 >= 0) { springs.emplace_back(Spring(p, &point_masses[(j - 1) * num_width_points + (i - 1)], SHEARING)); }
            if (i + 1 < num_width_points && j - 1 >= 0) { springs.emplace_back(Spring(p, &point_masses[(j - 1) * num_width_points + (i + 1)], SHEARING)); }

            // Bending Constraint with point mass two to left and two above ( i - 2) (j - 2)
            if (i - 2 >= 0) { springs.emplace_back(Spring(p, &point_masses[j * num_width_points + (i - 2)], BENDING)); }
            if (j - 2 >= 0) { springs.emplace_back(Spring(p, &point_masses[(j - 2) * num_width_points + i], BENDING)); }

        }
        
    }


}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;
  // TODO (Part 2): Compute total force acting on each point mass.
  Vector3D totalForce = Vector3D();
  for (auto &a: external_accelerations) {  totalForce += mass * a;  }
  
  for (auto &p : point_masses) {
      p.forces = totalForce; 
  }

  // Calculate Forces for Point Masses Connected By Springs
  for (auto &s : springs) {
      double springForceMagnitude = 0.0;
      if (s.spring_type == STRUCTURAL && cp->enable_structural_constraints == true || (s.spring_type == SHEARING && cp->enable_shearing_constraints == true)) {
          springForceMagnitude = cp->ks * ((s.pm_a->position - s.pm_b->position).norm() - s.rest_length);
      }
      else if (s.spring_type == BENDING && cp->enable_bending_constraints == true) {
          springForceMagnitude = (cp->ks * 0.2) * ((s.pm_a->position - s.pm_b->position).norm() - s.rest_length);
      }
      Vector3D springForce = springForceMagnitude * (s.pm_a->position - s.pm_b->position).unit();
      // apply b -> a force
      s.pm_b->forces += springForce;
      // apply a -> b force
      s.pm_a->forces += springForce * (-1);
  }
  // TODO (Part 2): Use Verlet integration to compute new point mass positions
  for (auto &p : point_masses) {
     //printf("Force: (%f, %f, %f )\n", p.forces.x, p.forces.y, p.forces.z);
      if (p.pinned == true) {  continue;  }
      Vector3D newPos = p.position + (1.0 - (cp->damping / 100.0)) * (p.position - p.last_position) + (p.forces / mass) * delta_t * delta_t;
      p.last_position = p.position;
      p.position = newPos;
      

  }
  for (auto& s : springs) {

      double dist = (s.pm_a->position - s.pm_b->position).norm();
      if (dist <= s.rest_length * 1.10) { continue; }
      double clamp = dist - (s.rest_length * 1.10);
      // a pinned => apply full clamp to b
      if (s.pm_a->pinned == true && s.pm_b->pinned == false) {
          s.pm_b->position += (s.pm_a->position - s.pm_b->position).unit() * clamp;
      }
      // b pinned => apply full clamp to a
      else if (s.pm_b->pinned == true && s.pm_a->pinned == false) {
          s.pm_a->position += (s.pm_b->position - s.pm_a->position).unit() * clamp;
      }
      // neither pinned => apply half the clamp to each 
      else if (s.pm_b->pinned == false && s.pm_a->pinned == false) {
          s.pm_a->position += (s.pm_b->position - s.pm_a->position).unit() * 0.5 * clamp;
          s.pm_b->position += (s.pm_a->position - s.pm_b->position).unit() * 0.5 * clamp;
      }
  }
  
  // TODO (Part 3): Handle collisions with other primitives.
  build_spatial_map();
  for (auto& p : point_masses) { 
      this->self_collide(p, simulation_steps); 
      for (auto& primitive : *collision_objects) { primitive->collide(p); }
  }
 
  // TODO (Part 4): Handle self-collisions.


  
  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
  
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.
  for (auto &p : point_masses ) { 
      float key = hash_position(p.position);
      vector<PointMass*>* ptr = new vector<PointMass*>;
      map.try_emplace(key, new vector<PointMass*>);
      if (map[key] != ptr) { delete(ptr); }
      map[key]->push_back(&p);
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
    float key = hash_position(pm.position);
    vector<PointMass*> neighbors = *map[key];
    Vector3D correction = Vector3D(0, 0, 0);
    int count = 0;
    for (auto neighbor : neighbors) {
        double dist = (pm.position - neighbor->position).norm();
        if (neighbor == &pm || dist >= 2 * thickness)  { continue; }
        correction +=  ((2 * thickness - dist) * (pm.position - neighbor->position).unit());
        count += 1;
    }
    if (count == 0) { return; }
    correction = (correction) / (count * simulation_steps);
    pm.position += correction;
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
    double w = 3 * width / num_width_points;
    double h = 3 * height / num_height_points;
    double t = max(w, h);
    
    double x = pos.x / w;
    double y = pos.y / h;
    double z = pos.z / t;

    if (isnan(x) || isinf(x)) { x = 0; }
    if (isnan(y) || isinf(y)) { y = 0; }
    if (isnan(z) || isinf(z)){ z = 0; }

    return  (floor(x) * 157.0) + (floor(y) * 63.0) + (floor(z) * 1201);
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

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

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
