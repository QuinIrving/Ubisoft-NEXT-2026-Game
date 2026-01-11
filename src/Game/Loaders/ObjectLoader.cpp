#include <fstream>
#include <cstddef>
#include "ObjectLoader.h"
#include <Graphics/Material.h>
#include <Graphics/Pipeline.h>

namespace ObjectLoader {
	std::unordered_map<std::string, Model> objectMap;
	std::unordered_map<std::string, Texture> textureMap;
	std::unordered_map<std::string, Material> materialMap;
}

namespace 
{
	// Helper functions
	uint64_t MakeEdgeKey(uint32_t a, uint32_t b) {
		if (b < a) {
			std::swap(a, b);
		}

		uint64_t key = (static_cast<uint64_t>(a) << 32) | b;
		return key;
	}

	void trim(std::string& inputString) {
		std::size_t found = inputString.find_first_of('#');
		if (found != std::string::npos) {
			inputString.erase(found, inputString.size());
		}

		inputString.erase(inputString.find_last_not_of(" \t\r\n") + 1, inputString.size());
	}

	std::vector<std::string> extractData(std::string& input, std::string delimiter) {
		std::vector<std::string> extractedData;
		size_t lastPos = 0;
		size_t pos = input.find(delimiter, 0);

		while (pos != std::string::npos) {
			std::string data = input.substr(lastPos, pos - lastPos);
			extractedData.push_back(data);

			lastPos = pos + delimiter.length();
			pos = input.find(delimiter, lastPos);
		}

		// edge case of the last piece of data that will end up having no delimiter
		extractedData.push_back(input.substr(lastPos, std::string::npos));
		return extractedData;
	}

	void CreateVertex(std::vector<Vec3<float>>& objVertices, std::vector<Vec2<float>>& objUVs, std::vector<Vec3<float>>& objNormals, Vertex& v, std::vector<std::string> strV, int dataSize) {
		Vec3<float> pos = objVertices[std::stoi(strV[0]) - 1];
		
		switch (dataSize) {
		case 3: {
			
			Vec2<float> uv = objUVs[std::stoi(strV[1]) - 1];
			Vec3<float> norm = objNormals[std::stoi(strV[2]) - 1];
			v = Vertex(pos, { 1.f, 1.f, 1.f, 1.f }, norm, uv);
			break;
		}
		case 2: {
			// v//vn
			if (strV.size() == 3) {
				Vec3<float> norm = objNormals[std::stoi(strV[2]) - 1];
				v = Vertex(pos, { 1.f, 1.f, 1.f, 1.f }, norm);
				return;
			}

			// v/vt
			Vec2<float> uv = objUVs[std::stoi(strV[1]) - 1];

			// again need to probably calculate the normal after
			v = Vertex(pos, { 1.f, 1.f, 1.f, 1.f }, {}, uv);
			break;
		}
		case 1: {
			v = Vertex(pos, { 1, 1, 1, 1 }, {}); // Should probably calculate normal and potentially UV's. Need to get the face normal, and then average across each face that uses the normal.
			break;
		}
		default:
			throw std::runtime_error("The obj file has an illogical number of data for a vertex.");
		}
	}

	void ProcessMTL(const std::string& path) {
		std::ifstream file{ path };

		if (!file) {
			throw std::runtime_error("Couldn't open the specified MTL file at path: " + path);
		}

		std::string line;

		bool onFirstMaterial = true;
		bool newMaterialFound = false;
		Material m;
		std::string lastMaterial;
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
			std::string data = line.substr(identifierPos + 1, line.size());

			if (prefix == "newmtl") { // Specifying a new material, can continue until next newmtl (if there are any more) in the case where we already read this material before.
				// if we find it in our hashmap, then continue going until we find another new material, keep doing this until end of file.
				// Is it already in our map? If so, lets keep moving forward reading the file by just reading each line until we get to the next material then continue our approach
				if (ObjectLoader::materialMap.find(data) != ObjectLoader::materialMap.end()) {
					while (std::getline(file, line)) {
						if (line.find("newmtl") == std::string::npos) {
							continue;
						}

						// found a newmaterial, check it as well
						int idPos = line.find_first_of(' ');
						data = line.substr(identifierPos + 1, line.size());

						if (ObjectLoader::materialMap.find(data) != ObjectLoader::materialMap.end()) {
							continue;
						}


					}
				}
				
				// only if we reached end of file with no new materials do we bypass below completely.
				if (file.eof()) {
					break;
				}

				newMaterialFound = true;

				if (onFirstMaterial) {
					onFirstMaterial = false;
					lastMaterial = data;
					continue;
				}

				ObjectLoader::materialMap[lastMaterial] = m;
				m = Material();
				lastMaterial = data;
				continue;
			}
			else if (prefix == "Ka") {
				std::vector<std::string> stringAmbient = extractData(data, " ");
				if (stringAmbient.size() != 3) {
					throw std::runtime_error("The mtl file being read contains Ka data that isn't just 3 pieces of data: " + path);
				}

				m.Ka = Vec3<float>(std::stof(stringAmbient[0]), std::stof(stringAmbient[1]), std::stof(stringAmbient[2]));
			}
			else if (prefix == "Kd") {
				std::vector<std::string> stringDiffuse = extractData(data, " ");
				if (stringDiffuse.size() != 3) {
					throw std::runtime_error("The mtl file being read contains Kd data that isn't just 3 pieces of data: " + path);
				}

				m.Kd = Vec3<float>(std::stof(stringDiffuse[0]), std::stof(stringDiffuse[1]), std::stof(stringDiffuse[2]));
			}
			else if (prefix == "Ks") {
				std::vector<std::string> stringSpecCol = extractData(data, " ");
				if (stringSpecCol.size() != 3) {
					throw std::runtime_error("The mtl file being read contains Ks data that isn't just 3 pieces of data: " + path);
				}

				m.Ks = Vec3<float>(std::stof(stringSpecCol[0]), std::stof(stringSpecCol[1]), std::stof(stringSpecCol[2]));
			}
			else if (prefix == "Tf") {
				std::vector<std::string> stringTransmissionFilter = extractData(data, " ");
				if (stringTransmissionFilter.size() != 3) {
					throw std::runtime_error("The mtl file being read contains Tf data that isn't just 3 pieces of data: " + path);
				}

				m.Tf = Vec3<float>(std::stof(stringTransmissionFilter[0]), std::stof(stringTransmissionFilter[1]), std::stof(stringTransmissionFilter[2]));
			}
			else if (prefix == "illum") {
				m.illum = static_cast<unsigned int>(std::stoi(data));
			}
			else if (prefix == "d") {
				// we're going to not assume special options like -halo
				m.d = std::stof(data);
			}
			else if (prefix == "Ns") {
				m.Ns = std::stof(data);
			}
			else if (prefix == "sharpness") {
				m.sharpness = std::stoi(data);
			}
			else if (prefix == "Ni") {
				m.Ni = std::stof(data);
			}
			else if (prefix == "map_Ka") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material

			}
			else if (prefix == "map_Kd") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material
				if (TextureLoader::textureMap.find(data) == TextureLoader::textureMap.end()) {
					TextureLoader::textureMap[data] = TextureLoader::ProcessTGA(path.substr(0, path.find_last_of('/') + 1) + data);
				}

				m.map_Kd = std::make_shared<Texture>(TextureLoader::textureMap[data]);
				continue;
			}
			else if (prefix == "map_Ks") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material

			}
			else if (prefix == "map_Ns") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material

			}
			else if (prefix == "map_d") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material

			}
			else if (prefix == "disp") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material

			}
			else if (prefix == "decal") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material

			}
			else if (prefix == "bump") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material

			}
			else if (prefix == "refl") {
				// Texture loader needs to check if we already have the texture stored, if not to generate it, store it, and then finally share it to material

			}

		}

		if (newMaterialFound) {
			ObjectLoader::materialMap[lastMaterial] = m;
		}
	}
}

/*
I'm going to have a really rude awakening, as I do vertices per mesh, and material is distinct, but to handle the adjancency table and tessellation
I need all vertices of a model together getting tessellated, so I will need to "re-construct" my mesh definitions for finding the right material to use/displacement.
May need to keep track of indices of the triIdx from start to end (first tri to last tri), so we can check which range it's within to apply the correct material.
*/
Model ObjectLoader::Load(const std::string& path) {
	// Probably should do the quick check of our hashmap for objects to see if we should even generate this, or simply give a copy of an object.
	
	std::ifstream file{ path };

	if (!file) {
		throw std::runtime_error("Couldn't open the specified OBJ file at path: " + path + "\n");
	}

	std::string line;
	std::string delimiter = " ";
	
	std::vector<Vec3<float>> objVertices;
	std::vector<Vec2<float>> objUVs;
	std::vector<Vec3<float>> objNormals;

	std::vector<Vertex> objProcessedVertices;
	std::vector<Mesh> modelMeshes;

	bool haveSeenAFace = false;

	//Model model;
	ModelEdge edges;
	Mesh mesh;
	int32_t triIdx = 0;
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
		
		std::string data = line.substr(identifierPos + 1, line.size());

		if (prefix == "v") {
			std::vector<std::string> stringVerts = extractData(data, delimiter);
			if (stringVerts.size() != 3) {
				throw std::runtime_error("The obj file being read contains vertices that aren't just 3 pieces of data: " + path);
			}
			
			objVertices.push_back({ std::stof(stringVerts[0]) * Pipeline::SCENE_SCALE, std::stof(stringVerts[1]) * Pipeline::SCENE_SCALE, std::stof(stringVerts[2]) * Pipeline::SCENE_SCALE });
		}
		else if (prefix == "vt") {
			std::vector<std::string> stringUVs = extractData(data, delimiter);
			if (stringUVs.size() != 2) {
				throw std::runtime_error("The obj file being read contains UV texture coords that aren't just 2 pieces of data: " + path);
			}
			
			objUVs.push_back({ std::stof(stringUVs[0]), std::stof(stringUVs[1]) });
		}
		else if (prefix == "vn") {
			std::vector<std::string> stringNormals = extractData(data, delimiter);
			if (stringNormals.size() != 3) {
				throw std::runtime_error("The obj file being read contains normals that aren't just 3 pieces of data: " + path);
			}

			objNormals.push_back({ std::stof(stringNormals[0]), std::stof(stringNormals[1]), std::stof(stringNormals[2]) });
		}
		else if (prefix == "f") {
			haveSeenAFace = true;

			// if f has more than 3 vertices, then we fan them with v0,
			/*
			ex. f v0 v1 v2 v3 v4
			(v0, v1, v2)
			(v0, v2, v3)
			(v0, v3, v4)

			-----
			Specific scenarios
			f v
			f v/vt
			f v//vn
			f v/vt/vn
			*/
			std::vector<std::string> stringIndices = extractData(data, " ");
			if (stringIndices.size() < 3) {
				throw std::runtime_error("The obj file being read contains faces that have less than 3 vertices of data: " + path);
			}
			
			std::vector<std::vector<std::string>> indices;

			for (int i = 0; i < stringIndices.size(); ++i) {
				indices.push_back(extractData(stringIndices[i], "/"));
			}

			std::vector<std::string> strV0 = indices[0];
			int dataSize = strV0.size();
			if (dataSize == 3 && strV0[1] == "") {
				dataSize = 2;
			}

			Vertex v0;
			CreateVertex(objVertices, objUVs, objNormals, v0, strV0, dataSize);
			Vertex v1;
			Vertex v2;

			Vertex rv0, rv1, rv2; // Our rotated vertices.

			int i0 = std::stoi(strV0[0]) - 1; // Not going to handle the case of - vertices unless it becomes a problem, as I need to focus on other aspects of the engine.
			v0.SetMeshIndex(i0);
			v0.SetMaterialIndex(modelMeshes.size());

			// We can do our pre-process rotation here with a helper function, and then we can also add our EdgeKey to the adjacency.
			// std:vector<std::vector<int>> 
			for (int i = 2; i < indices.size(); ++i) {
				// Technically DON'T NEED TANGENT DUE TO NOT USING NORMAL MAPS SO CAN REMOVE FROM VERTICES WITHOUT WORRY!!!!
				// DO still want to calculate normal if it's not there already (such as bunny)
				//Vertex v = Vertex();
				std::vector<std::string> strV1 = indices[i - 1];
				std::vector<std::string> strV2 = indices[i];

				if (strV0.size() != strV1.size() || strV1.size() != strV2.size()) {
					throw std::runtime_error("The obj file being read contains inconsistencies in the number of face data information: " + path);
				}

				CreateVertex(objVertices, objUVs, objNormals, v1, strV1, dataSize);
				CreateVertex(objVertices, objUVs, objNormals, v2, strV2, dataSize);
				// Probably want to pre-process here.
				// v0, v1, v2, rotate as needed, then push in that order. Don't forget to also add edgekey to adjacency

				// Make 3 edge keys with our indices and add them to the adjacencyTable with our triIdx added into it,
				// Then we also set our vertex mesh index to it's specific index.
				// finally we do our rotation, and push our 3 vertices in the correct order.
				int i1 = std::stoi(strV1[0]) - 1;
				int i2 = std::stoi(strV2[0]) - 1;

				edges.adjacencyTable[MakeEdgeKey(i0, i1)].push_back(triIdx);
				edges.adjacencyTable[MakeEdgeKey(i1, i2)].push_back(triIdx);
				edges.adjacencyTable[MakeEdgeKey(i2, i0)].push_back(triIdx);

				v1.SetMeshIndex(i1);
				v1.SetMaterialIndex(modelMeshes.size());
				v2.SetMeshIndex(i2);
				v2.SetMaterialIndex(modelMeshes.size());
				

				float edge1 = (v1.GetPosition() - v0.GetPosition()).GetMagnitudeSquared();
				float edge2 = (v2.GetPosition() - v1.GetPosition()).GetMagnitudeSquared();
				float edge3 = (v0.GetPosition() - v2.GetPosition()).GetMagnitudeSquared();

				if (edge1 >= edge2 && edge1 >= edge3) {
					/*	Must do 2 rotations to get the longest edge to be v2->v0
						v0->v1 is currently the longest, so v1 maps to v0, v0 maps to v2, and v2 maps to v1
					*/

					rv0 = v1;
					rv1 = v2;
					rv2 = v0;
				}
				else if (edge2 >= edge1 && edge2 >= edge3) {
					/*	Must do 1 rotation to get the longest edge to be v2->v0
						v1->v2 is currently the longest. So: v2 maps to v0, v1 maps to v2, v0 maps to v1.
					*/

					rv0 = v2;
					rv1 = v0;
					rv2 = v1;
				}
				else if (edge3 >= edge1 && edge3 >= edge2) {
					rv0 = v0;
					rv1 = v1;
					rv2 = v2;
				}

				objProcessedVertices.insert(objProcessedVertices.end(), { rv0, rv1, rv2 });
				triIdx++;
			}

		}
		else if (prefix == "g") {
			// Means we don't need to create and push a new object, once we have, the next g means a new mesh.
			if (!haveSeenAFace) {
				continue;
			}


			// Could also extract and get the name of the mesh here, but don't think we really need it
			mesh.geometry = std::make_shared<MeshGeometry>(MeshGeometry(objProcessedVertices));
			modelMeshes.push_back(mesh);
			mesh = Mesh();
			objProcessedVertices.clear();
		}
		else if (prefix == "mtllib") {
			// read the file and check our hashmap to keep global track of all of our textures to see if we can simply skip each mtl defined.
			ProcessMTL(path.substr(0, path.find_last_of('/') + 1) + data);
			continue;
		}
		else if (prefix == "usemtl") {
			// Get the material from the hashmap and apply it to the new mesh object in g.
			mesh.material = ObjectLoader::materialMap[data];
			continue;
		}
	}

	mesh.geometry = std::make_shared<MeshGeometry>(MeshGeometry(objProcessedVertices));
	modelMeshes.push_back(mesh);

	return Model(modelMeshes, edges);
}


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