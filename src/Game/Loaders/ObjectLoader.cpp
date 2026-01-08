#include <fstream>
#include <cstddef>
#include "ObjectLoader.h"


namespace 
{
	// Helper functions
	void trim(std::string& inputString) {
		std::size_t found = inputString.find_first_of('#');
		if (found != std::string::npos) {
			inputString.erase(found, inputString.size());
		}

		inputString.erase(inputString.find_last_not_of(" \t\r\n") + 1, inputString.size());
	}

}

Model ObjectLoader::Load(const std::string& path) {
	// Probably should do the quick check of our hashmap for objects to see if we should even generate this, or simply give a copy of an object.
	
	std::ifstream file{ path };

	if (!file) {
		throw std::runtime_error("Couldn't open the specified OBJ file at path: " + path + "\n");
	}

	std::string line;
	
	std::vector<Vec3<float>> objVertices;
	std::vector<Vec2<float>> objTextureCoords;
	std::vector<Vec3<float>> objNormalCoords;

	std::vector<Vertex> objProcessedVertices;
	std::vector<Mesh> modelMeshes;

	bool haveSeenAFace = false;

	//Model model;
	Mesh mesh;
	while (std::getline(file, line)) {
		trim(line);
		if (line.empty()) {
			continue;
		}

		int identifierPos = line.find_first_of(' ');
		if (identifierPos == std::string::npos) {
			// something probably is going wrong here if we have no spaces.
			throw std::runtime_error("The obj file being read does not seem to have any spaces between attributes: " + line + "\n path: " + path);
		}

		std::string prefix = line.substr(0, identifierPos);
		int test = 2;
		
		if (prefix == "v") {

		}
		else if (prefix == "vt") {

		}
		else if (prefix == "vn") {

		}
		else if (prefix == "f") {
			haveSeenAFace = true;

			// if f has more than 3 vertices, then we fan them with v0,
			/*
			ex. f v0 v1 v2 v3 v4
			(v0, v1, v2)
			(v0, v2, v3)
			(v0, v3, v4)
			*/

		}
		else if (prefix == "g") {
			// Means we don't need to create and push a new object, once we have, the next g means a new mesh.
			if (!haveSeenAFace) {
				continue;
			}

			modelMeshes.push_back(mesh);
			mesh = Mesh();
		}
		else if (prefix == "mtllib") {
			// read the file and check our hashmap to keep global track of all of our textures to see if we can simply skip each mtl defined.
		}
		else if (prefix == "usemtl") {
			// Get the material from the hashmap and apply it to the new mesh object in g.
		}
		
	}


	return Model({});
}


/*
TriangleContext PreProcessMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
	// Take in a mesh in model space, and want to pre-determine the general neighbour structure, and re-arrange the vertices as necessary to fit it via rotation
	TriangleContext context;
	//context.depthLevels = std::vector<uint8_t>(indices.size() / 3, 0);
	context.processedMesh.reserve(indices.size());

	// Pre-process the triangles I guess;
	for (int i = 0; i < indices.size(); i += 3) {
		int triIdx = static_cast<int>(i / 3);

		uint32_t i0 = indices[i];
		uint32_t i1 = indices[i + 1];
		uint32_t i2 = indices[i + 2];

		uint64_t key1 = MakeEdgeKey(i0, i1);
		uint64_t key2 = MakeEdgeKey(i1, i2);
		uint64_t key3 = MakeEdgeKey(i0, i2);

		context.adjacencyTable[key1].push_back(triIdx);
		context.adjacencyTable[key2].push_back(triIdx);
		context.adjacencyTable[key3].push_back(triIdx);
	}

	for (int i = 0; i < indices.size(); i += 3) {
		int triIdx = static_cast<int>(i / 3);

		uint32_t i0 = indices[i];
		uint32_t i1 = indices[i + 1];
		uint32_t i2 = indices[i + 2];

		// triangles shouldn't have the same vertex twice.
		if (i0 == i1 || i1 == i2 || i2 == i0) {
			throw std::runtime_error("Multiple duplicate vertices on one triangle, leading to a degenerate triangle in pre-processing.");
		}

		Vertex v0 = vertices[i0];
		Vertex v1 = vertices[i1];
		Vertex v2 = vertices[i2];
		v0.SetMeshIndex(i0);
		v1.SetMeshIndex(i1);
		v2.SetMeshIndex(i2);

		Vertex rv0, rv1, rv2; // Our post-rotation vertices.


		int longestEdgeIdx; // 0: v0->v1, 1: v1->v2, 2: v2->v0

		float edge1 = (v1.GetPosition() - v0.GetPosition()).GetMagnitudeSquared();
		float edge2 = (v2.GetPosition() - v1.GetPosition()).GetMagnitudeSquared();
		float edge3 = (v0.GetPosition() - v2.GetPosition()).GetMagnitudeSquared();

		if (edge1 >= edge2 && edge1 >= edge3) {
			longestEdgeIdx = 0;
		}
		else if (edge2 >= edge1 && edge2 >= edge3) {
			longestEdgeIdx = 1;
		}
		else if (edge3 >= edge1 && edge3 >= edge2) {
			longestEdgeIdx = 2;
		}
		else {
			longestEdgeIdx = -1;
		}

		// CCW rotation. v1 obtains v0, v2 obtains v1, v0 obtains v2.
		if (longestEdgeIdx == 0) {
			// Must do 2 rotations to get the longest edge to be v2->v0
			/*
			v0->v1 is currently the longest, so v1 maps to v0, v0 maps to v2, and v2 maps to v1
			* /
rv0 = v1;
rv1 = v2;
rv2 = v0;
		}
		else if (longestEdgeIdx == 1) {
			// Must do 1 rotation to get the longest edge to be v2->v0
			/*
			v1->v2 is currently the longest. So: v2 maps to v0, v1 maps to v2, v0 maps to v1.
			* /
			rv0 = v2;
			rv1 = v0;
			rv2 = v1;
		}
		else if (longestEdgeIdx == 2) {
			rv0 = v0;
			rv1 = v1;
			rv2 = v2;
		}
		else if (longestEdgeIdx == -1) {
			// something went wrong throw an error.
			throw std::runtime_error("Longest Edge somehow was -1\n");
		}


		context.processedMesh.insert(context.processedMesh.end(), { rv0, rv1, rv2 });
	}

	return context;
}
*/

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