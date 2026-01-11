#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include <cstddef>
#include "Graphics/Texture.h"


namespace TextureLoader {
	extern std::unordered_map<std::string, Texture> textureMap;

	Texture ProcessTGA(std::string& path);
	void GenerateTextureTopology(std::string& texturePath);
	//Texture ProcessTGA_RGBA(std::string& path); // Might need down the line.
	//Texture ProcessTGA_RGB(std::string& path);
	//Texture ProcessTGA_BW(std::string& path);
}