
#include <stdlib.h>
#include <stddef.h>
#include <raylib.h>
#include <raymath.h>

#include <stdio.h>

#include "../inc/nanoOBJ.h"
#include "../inc/raylibglue.h"


size_t array_up_bound_check(size_t ind, size_t maxsize) {
    if (ind < maxsize) return ind;
    return 0;
}


Vector3 VertexToVector3(ObjVertex vert) {
    return (Vector3) {
        .x = vert.comps[0],
        .y = vert.comps[1],
        .z = vert.comps[2]
    };
}

Vector3 NormalToVector3(ObjNormal norm) {
    return (Vector3) {
        .x = norm.comps[0],
        .y = norm.comps[1],
        .z = norm.comps[2]
    };
}

void generate_normals(ObjData *data) {
    if ((data->vertexcount == 0) || (data->vertexarray == NULL)) {
        perror("No vertex data! Cannot generate normals.");
        return;
    }

    data->normalcount = data->vertexcount;
    data->normalarray = calloc(data->vertexcount, sizeof(ObjNormal));
    for (size_t i = 0; i < data->normalcount; i++)
    {
        data->normalarray[i].comps[0] = 0;
        data->normalarray[i].comps[1] = 0;
        data->normalarray[i].comps[2] = 0;
    }

    ObjVertex *varr = data->vertexarray;
    ObjNormal *narr = data->normalarray;

    Vector3 A, B, C, nbuff;
    for (size_t i = 0; i < data->facecount; i++)
    {
        ObjFace *face = &data->facearray[i];
        face->normalindices = calloc(face->ncount, sizeof(int));
        for (size_t i = 0; i < face->ncount; i++)
            face->normalindices[i] = face->vertexindices[i];
        

        for (size_t j = 0; j < face->ncount - 2; j++)
        {
            A = VertexToVector3(
                varr[array_up_bound_check(face->vertexindices[0] - 1, data->vertexcount)]
            );
            B = VertexToVector3(
                varr[array_up_bound_check(face->vertexindices[1 + j] - 1, data->vertexcount)]
            );
            C = VertexToVector3(
                varr[array_up_bound_check(face->vertexindices[2 + j] - 1, data->vertexcount)]
            );

            nbuff = Vector3CrossProduct(
                Vector3Subtract(B, A),
                Vector3Subtract(C, A)
            );
            // nbuff = Vector3Normalize(nbuff);

            narr[array_up_bound_check(face->vertexindices[0] - 1, data->normalcount)].comps[0] += nbuff.x;
            narr[array_up_bound_check(face->vertexindices[0] - 1, data->normalcount)].comps[1] += nbuff.y;
            narr[array_up_bound_check(face->vertexindices[0] - 1, data->normalcount)].comps[2] += nbuff.z;

            narr[array_up_bound_check(face->vertexindices[1 + j] - 1, data->normalcount)].comps[0] += nbuff.x;
            narr[array_up_bound_check(face->vertexindices[1 + j] - 1, data->normalcount)].comps[1] += nbuff.y;
            narr[array_up_bound_check(face->vertexindices[1 + j] - 1, data->normalcount)].comps[2] += nbuff.z;

            narr[array_up_bound_check(face->vertexindices[2 + j] - 1, data->normalcount)].comps[0] += nbuff.x;
            narr[array_up_bound_check(face->vertexindices[2 + j] - 1, data->normalcount)].comps[1] += nbuff.y;
            narr[array_up_bound_check(face->vertexindices[2 + j] - 1, data->normalcount)].comps[2] += nbuff.z;
        }
        
    }
    
    for (size_t i = 0; i < data->normalcount; i++)
    {
        nbuff = Vector3Normalize(NormalToVector3(narr[i]));

        narr[i].comps[0] = nbuff.x;
        narr[i].comps[1] = nbuff.y;
        narr[i].comps[2] = nbuff.z;
    }
    
}


Mesh get_mesh_from_obj_data(ObjData data) {

    Mesh mesh = { 0 };

    size_t total_vertex_array_size = 0, n_triangles = 0;

    for (size_t i = 0; i < data.facecount; i++)
    {
        ObjFace face = data.facearray[i];
        n_triangles += face.ncount - 2;
    }

    total_vertex_array_size = 3 * n_triangles;

    float *position_arr = NULL;
    if (data.vertexcount)
        position_arr = (float*) MemAlloc(total_vertex_array_size * 3 * sizeof(float));
    float *texcoord_arr = NULL;
    if (data.texcoordcount)
        texcoord_arr = (float*) MemAlloc(total_vertex_array_size * 2 * sizeof(float));
    float *normals_arr = NULL;
    if (data.normalcount)
        normals_arr = (float*) MemAlloc(total_vertex_array_size * 3 * sizeof(float));
    size_t ip = 0, itc = 0, in = 0;

    ObjVertex *varr = data.vertexarray;
    ObjTexCoords *tcarr = data.texcoordarray;
    ObjNormal *narr = data.normalarray;

    for (size_t i = 0; i < data.facecount; i++)
    {
        ObjFace face = data.facearray[i];

        for (size_t j = 0; j < face.ncount - 2; j++)
        {
            if (data.vertexcount) {
                for (size_t k = 0; k < 3; k++)
                    position_arr[ip++] = 
                        varr[array_up_bound_check(face.vertexindices[0] - 1, data.vertexcount)]
                        .comps[k];
                for (size_t k = 0; k < 3; k++)
                    position_arr[ip++] = 
                        varr[array_up_bound_check(face.vertexindices[1 + j] - 1, data.vertexcount)]
                        .comps[k];
                for (size_t k = 0; k < 3; k++)
                    position_arr[ip++] = 
                        varr[array_up_bound_check(face.vertexindices[2 + j] - 1, data.vertexcount)]
                        .comps[k];
            }
            
            if (data.texcoordcount) {
                for (size_t k = 0; k < 2; k++)
                    texcoord_arr[itc++] = 
                        tcarr[array_up_bound_check(face.texcoordindices[0] - 1, data.texcoordcount)]
                        .comps[k];
                for (size_t k = 0; k < 2; k++)
                    texcoord_arr[itc++] = 
                        tcarr[array_up_bound_check(face.texcoordindices[1 + j] - 1, data.texcoordcount)]
                        .comps[k];
                for (size_t k = 0; k < 2; k++)
                    texcoord_arr[itc++] = 
                        tcarr[array_up_bound_check(face.texcoordindices[2 + j] - 1, data.texcoordcount)]
                        .comps[k];
            }
            
            if (data.normalcount) {
                for (size_t k = 0; k < 3; k++)
                    normals_arr[in++] = 
                        narr[array_up_bound_check(face.normalindices[0] - 1, data.normalcount)]
                        .comps[k];
                for (size_t k = 0; k < 3; k++)
                    normals_arr[in++] = 
                        narr[array_up_bound_check(face.normalindices[1 + j] - 1, data.normalcount)]
                        .comps[k];
                for (size_t k = 0; k < 3; k++)
                    normals_arr[in++] = 
                        narr[array_up_bound_check(face.normalindices[2 + j] - 1, data.normalcount)]
                        .comps[k];
            }
            
        }
        
    }

    
    mesh.vertexCount = total_vertex_array_size;
    mesh.triangleCount = n_triangles;
    mesh.vertices = position_arr;
    mesh.texcoords = texcoord_arr;
    mesh.normals = normals_arr;
    mesh.indices = NULL;

    return mesh;
}



