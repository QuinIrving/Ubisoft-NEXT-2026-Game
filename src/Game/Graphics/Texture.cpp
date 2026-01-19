#include <algorithm>
#include "Texture.h"

// DON'T FORGET TEXTURE MODE: Wrap, Mirror, Clamp etc later
Colour Texture::SampleNearest(float u, float v) const {
	u = std::clamp(u, 0.f, 1.f);
	v = std::clamp(v, 0.f, 1.f);

	int x = static_cast<int>(u * (width - 1));
	//int y = static_cast<int>(v * (height - 1)); // This would be for top-left images, we are doing bottom left?
	int y = static_cast<int>((1.f - v) * (height - 1));
	return texels.at(y * width + x);
}

Colour Texture::SampleBilinear(float u, float v) const {
	float U = u * width - 0.5f;
	float V = (1.f - v) * height - 0.5f;// only flip because we read our texels in from top left... could use direct x style instead maybe?

	int u0 = static_cast<int>(floorf(U));
	int v0 = static_cast<int>(floorf(V));
	int u1 = u0 + 1;
	int v1 = v0 + 1;

	u0 = std::clamp(u0, 0, width - 1);
	u1 = std::clamp(u1, 0, width - 1);
	v0 = std::clamp(v0, 0, height - 1);
	v1 = std::clamp(v1, 0, height - 1);
	
	float deciU = U - u0;
	float deciV = V - v0;
	
	//      uv
	Colour c00 = texels[v0 * width + u0];
	Colour c10 = texels[v0 * width + u1];
	Colour c01 = texels[v1 * width + u0];
	Colour c11 = texels[v1 * width + u1];

	Colour cTop = c00 * (1 - deciU) + c10 * deciU;
	Colour cBot = c01 * (1 - deciU) + c11 * deciU;


	return cTop * (1 - deciV) + cBot * deciV;
}