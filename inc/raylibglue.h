
#pragma once

#include "nanoOBJ.h"
#include <raylib.h>

// Returns the index if it is below maxsize, else 0
size_t array_up_bound_check(size_t ind, size_t maxsize);



// Type casting
Vector3 VertexToVector3(ObjVertex vert);

// Type casting
Vector3 NormalToVector3(ObjNormal norm);

// Generates normals from vertex position data, and puts it into the data
void generate_normals(ObjData *data);

// Makes a mesh from given obj data. 
// NOTE: you can safely `deload` objdata after calling this, the mesh data is in a new array.
Mesh get_mesh_from_obj_data(ObjData data);
