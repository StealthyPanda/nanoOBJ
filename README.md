
# nanoOBJ

A simple standalone library for loading `.obj` files in C. No dependencies.

This was made because the default OBJ loader in [`raylib.h`](https://github.com/raysan5/raylib) is buggy.

This library extracts the bare minimums for making a mesh: vertex positions, normals and texture coordinates from the file.

Additionally, if you wanna use this specifically for `raylib.h`, there are helper functions in `raylibglue.h`.


## Usage

All headers in `inc`, all source files in `src`.

> **Note:** This project uses the [hermes build system](https://github.com/StealthyPanda/hermes), though any build system is fine. To use it with hermes, git clone this repo to your project root, and add "nanoOBJ" as a submodule in "aslibs". The library can be treated like any standard external library.

Load file data with `load_obj_data`, and free the memory later with `deload_obj_data`. That's it.

If using `raylibglue.h`, you can create a mesh with `get_mesh_from_obj_data`.

If there are no normals, or broken normals, and want to auto-generate, you must explicitly call `generate_normals` before creating a mesh.


## Example

```C
#include <stdio.h>

#include <nanoOBJ.h>

int main() {
    
    ObjData data = load_obj_data("test.OBJ");

    printf(
        "Vertices: %llu, Texture coordinates: %llu, Normals: %llu\n", 
        data.vertexcount, 
        data.texcoordcount, 
        data.normalcount
    );

    ...do stuff with data...

    deload_obj_data(data);

    return 0;
}

```

For `raylib.h`:

```C


#include <stdio.h>

#include <raylib.h>

#include <nanoOBJ.h>
#include <raylibglue.h>

int main() {
    
    ObjData data = load_obj_data("test.OBJ");

    printf(
        "Vertices: %llu, Texture coordinates: %llu, Normals: %llu\n", 
        data.vertexcount, 
        data.texcoordcount, 
        data.normalcount
    );

    if (data.normalcount == 0) {
        printf("Missing normals, generating...\n");
        generate_normals(&data);
    }

    Mesh mesh = get_mesh_from_obj_data(data);
    deload_obj_data(data); //OK to deload here. Mesh has its own data copy.

    UploadMesh(&mesh, false);

    Model model = LoadModelFromMesh(mesh);

    ...do your raylib stuff here...

    return 0;
}

```


## Issues

Does not parse group, material or any other info in common .obj variants.

