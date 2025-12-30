#pragma once
#include <vector>
#include "Colour.h"

struct Texture {
	int width;
	int height;
	std::vector<Colour> texels;

	Colour SampleNearest(float u, float v) const;
	Colour SampleBilinear(float u, float v) const;

	Texture(const std::string& texturePath);
};