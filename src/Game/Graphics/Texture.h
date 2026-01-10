#pragma once
#include <vector>
#include "Math/Vec2.h"
#include "Colour.h"

enum TextureChannels {
	BW, // rgb are all the same, can just read one
	RGB,
	RGBA
};

struct Texture {
	int width = 0;
	int height = 0;
	TextureChannels channelType;
	//int channels = 1; // 1: BW->(RGB = same value, A=255), 3: RGB (A = 255), 4: RGBA
	std::vector<Colour> texels;

	Colour SampleNearest(float u, float v) const;
	Colour SampleNearest(Vec2<float> UV) const { return SampleNearest(UV.x, UV.y); }
	Colour SampleBilinear(float u, float v) const;
	Colour SampleBilinear(Vec2<float> UV) const { return SampleBilinear(UV.x, UV.y); }

	//Texture(const std::string& texturePath);
	Texture() = default;
};