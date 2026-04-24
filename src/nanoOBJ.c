
#include "../inc/nanoOBJ.h"

#include <stdio.h>
#include <stdlib.h>


ObjVertex _obj_read_vert(const char* buffer) {
    ObjVertex holder;
    sscanf(buffer, "%f %f %f", holder.comps, holder.comps + 1, holder.comps + 2);
    return holder;
}

ObjNormal _obj_read_normal(const char* buffer) {
    ObjNormal holder;
    sscanf(buffer, "%f %f %f", holder.comps, holder.comps + 1, holder.comps + 2);
    return holder;
}

ObjTexCoords _obj_read_texcoord(const char* buffer) {
    ObjTexCoords holder;
    sscanf(buffer, "%f %f", holder.comps, holder.comps + 1);
    return holder;
}

void _obj_read_triplets(char* buffer, char* end, int *f, int *s, int *t) {
    size_t a = 0, b = 0;
    char temp[32] = { 0 };

    int holder[3] = { 0, 0, 0 };

    for (size_t j = 0; j < 3; j++)
    {
        while (
            (buffer[b] != '/') && 
            (buffer[b] != 0) && 
            (b < (end - buffer))
        ) b++;

        if (a != b) {
            
            for (size_t i = 0; i < 32; i++)
            {
                if (i < (b - a)) temp[i] = buffer[a + i];
                else temp[i] = 0;
            }
            
            holder[j] = atoi(temp);
        }

        a = b + 1; b++;
    }

    *f = holder[0];
    *s = holder[1];
    *t = holder[2];

}

ObjFace _obj_read_face(char* buffer, char* end) {
    ObjFace holder;
    
    size_t prev = 0, cursor = 0, ncount = 0;
    
    while (buffer[cursor] == ' ') cursor++;
    prev = cursor; cursor++;
    
    while ((buffer[cursor] != '\n') && (cursor < (end - buffer))) {
        
        while ((buffer[cursor] != ' ') && (cursor < (end - buffer))) cursor++;
        
        ncount++;
        
        while ((buffer[cursor] == ' ') && (cursor < (end - buffer))) cursor++;
        
    }


    holder.vertexindices   = calloc(ncount, sizeof(int));
    holder.normalindices   = calloc(ncount, sizeof(int));
    holder.texcoordindices = calloc(ncount, sizeof(int));
    size_t triplet_index = 0;

    cursor = 0;
    while (buffer[cursor] == ' ') cursor++;
    prev = cursor; cursor++;
    while ((buffer[cursor] != '\n') && (cursor < (end - buffer))) {
        
        while ((buffer[cursor] != ' ') && (cursor < (end - buffer))) 
        cursor++;
        
        // printf("Sending in triplet:|");
        // for (char* x = buffer + prev; x < buffer + cursor; x++)
        //     printf("%c", *x);
        // printf("|\n");
        
        _obj_read_triplets(
            buffer + prev,
            buffer + cursor,
            holder.vertexindices + triplet_index,
            holder.texcoordindices + triplet_index,
            holder.normalindices + triplet_index
        );
        triplet_index++;
        
        while ((buffer[cursor] == ' ') && (cursor < (end - buffer))) cursor++;
        prev = cursor; cursor++;
    }

    holder.ncount = ncount;

    return holder;

}



ObjData load_obj_data(const char* fpath) {
    FILE* fp = NULL;
    fp = fopen(fpath, "r");

    if (fp == NULL) {
        perror("Couldn't open .obj file!");
    }

    char buffer[1025] = { 0 };
    size_t nverts = 0, nfaces = 0, nnormals = 0, ntexcoords = 0;

    // First pass we just get all the counts of each of the things
    while (fgets(buffer, 1024, fp) != NULL) {
        if (buffer[0] == '#') continue;

        if (buffer[0] == 'v') {
            // This is a vertex definition
            if (buffer[1] == 't') ntexcoords++;
            else if (buffer[1] == 'n') nnormals++;
            else nverts++;
        }
        else if (buffer[0] == 'f') {
            // This is a face definition
            nfaces++;
        }

        for (size_t i = 0; i < 1025; i++)
            buffer[i] = 0;
    }

    printf(
        "The object has %llu vertices, %llu normals, %llu texcoords, %llu faces.\n",
        nverts, nnormals, ntexcoords, nfaces
    );

    ObjVertex *vertexarray = calloc(nverts, sizeof(ObjVertex));
    ObjNormal *normalarray = calloc(nnormals, sizeof(ObjNormal));
    ObjTexCoords *texcoordarray = calloc(ntexcoords, sizeof(ObjTexCoords));
    ObjFace *facesarray = calloc(nfaces, sizeof(ObjFace));
    size_t ivert = 0, inormal = 0, itexcoord = 0, iface = 0;

    rewind(fp);

    // int it = 1;
    while (fgets(buffer, 1024, fp) != NULL) {
        // printf("%d here?\n", it++);

        if (buffer[0] == '#') continue;

        if (buffer[0] == 'v') {
            // This is a vertex definition
            if (buffer[1] == 't') {
                // This is a texcoord;
                texcoordarray[itexcoord] = _obj_read_texcoord(buffer + 2);
                itexcoord++;
            }
            else if (buffer[1] == 'n') {
                // This is a normal
                normalarray[inormal] = _obj_read_normal(buffer + 2);
                inormal++;
            }
            else {
                // This is a simple vertex
                vertexarray[ivert] = _obj_read_vert(buffer + 1);
                ivert++;
            }
        }
        else if (buffer[0] == 'f') {
            // This is a face definition
            // printf("going here\n");
            size_t i = 1, j = 1;
            while (buffer[j] == ' ') j++;
            while ((buffer[i] != '\n') && (buffer[i] != 0)) i++;
            // printf("Input: |");
            // for (size_t k = j; k < i; k++)
            //     printf("%c", buffer[k]);
            // printf("|\n");
            facesarray[iface] = _obj_read_face(buffer + j, buffer + i);
            iface++;
        }

        for (size_t i = 0; i < 1025; i++)
            buffer[i] = 0;
    }


    fclose(fp);


    ObjData data = {
        .vertexarray = vertexarray, .vertexcount = nverts,
        .texcoordarray = texcoordarray, .texcoordcount = ntexcoords,
        .normalarray = normalarray, .normalcount = nnormals,
        .facearray = facesarray, .facecount = nfaces
    };

    return data;
}


void deload_obj_data(ObjData data) {
    for (size_t i = 0; i < data.facecount; i++)
    {
        free(data.facearray[i].vertexindices);
        free(data.facearray[i].texcoordindices);
        free(data.facearray[i].normalindices);
    }
    
    free(data.facearray);
    free(data.vertexarray);
    free(data.texcoordarray);
    free(data.normalarray);
}


