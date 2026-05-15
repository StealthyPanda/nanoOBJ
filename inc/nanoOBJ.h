
#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct {
    int *vertexindices, *normalindices, *texcoordindices;
    size_t ncount;
} ObjFace;


typedef struct {
    float comps[3];
} ObjVertex;

typedef struct {
    float comps[3];
} ObjNormal;

typedef struct {
    float comps[2];
} ObjTexCoords;


typedef struct
{
    ObjVertex *vertexarray;
    ObjTexCoords *texcoordarray;
    ObjNormal *normalarray;
    ObjFace *facearray;

    size_t vertexcount, texcoordcount, normalcount, facecount;
} ObjData;



// Load Object Data from a .obj file. 
// MUST deload at the end with `deload_obj_data`
ObjData load_obj_data(const char* fpath);

// Deloads (and frees all `malloc`ed data)
void deload_obj_data(ObjData data);

// Pickles obj data into a given file handle
void pickle_obj_data(ObjData data, const char *fp, const char *pn);

