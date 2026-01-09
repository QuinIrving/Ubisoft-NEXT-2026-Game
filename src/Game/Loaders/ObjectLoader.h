#pragma once
#include <string>
#include <unordered_map>
#include "Graphics/ModelAttributes.h"
#include "Objects/Model.h"
#include "Objects/Mesh.h"

// WE want to read in the obj file. If it references an .mtl file, we also read that, then we check that against our hashmap, if it hasn't already been created, we make each material,
// then we can retrieve the material proprties for a submesh when we read in the usemtl it wants to use.

/*
Function to read in .mtl file and we want some form of reference to our global material hashmap
check and read in and store the properties of each material.

On each object, we want to be able to extract the vertices (always there)
Check for and extract the vertex normals (if they're there)
Check for and extract the Texture coords (if they're there)
Generate faces (so create the 3 vertices, and store them into the pre-processed mesh if we can)
	^ if we have normals, add them in there, same with texture coords (4 possible scenarios we need to handle)
Be able to group our objects to have multiple submeshes with their own specifical materials (we need to bind a material to it)
Read in the material from our hashamp to be bound to our submesh/faces we make
Add to an overall composite object comprised of multiple vertices that all moves with one singular model matrix.
Skip sections where there is a #, should read up to that point, and don't give a shit after, and don't worry about any lines starting with it.

"./data/TestData/Test.bmp" is how the sprite, so ours will be in ./data/FOLDERNAME/file whatever path we want right

*/

namespace ObjectLoader {
	extern std::unordered_map<std::string, Model> objectMap;
	extern std::unordered_map<std::string, Texture> textureMap;
	extern std::unordered_map<std::string, Material> materialMap;

	Model Load(const std::string& path);
};


// We haven't made our ECS yet, so this is just rough, but we want to pass back a potentially composite object of multiple submeshes, Most likely we want a Mesh object, and an Object object
// Might be better names to be: Model, and Mesh.

