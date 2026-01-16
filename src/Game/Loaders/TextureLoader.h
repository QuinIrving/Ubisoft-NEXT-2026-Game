#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include <cstddef>
#include "Graphics/Texture.h"



namespace TextureLoader {
	extern std::unordered_map<std::string, Texture> textureMap;
	
	struct UVTri {
		Vec2<float> v0;
		Vec2<float> v1;
		Vec2<float> v2;

		UVTri(Vec2<float> v0, Vec2<float> v1, Vec2<float> v2) : v0(v0), v1(v1), v2(v2) {};
	};

	Texture ProcessTGA(std::string& path);
	std::vector<UVTri> GenerateTextureTopology(std::string& texturePath);
	uint64_t MakeEdgeKey(uint32_t a, uint32_t b);
	//Texture ProcessTGA_RGBA(std::string& path); // Might need down the line.
	//Texture ProcessTGA_RGB(std::string& path);
	//Texture ProcessTGA_BW(std::string& path);
}