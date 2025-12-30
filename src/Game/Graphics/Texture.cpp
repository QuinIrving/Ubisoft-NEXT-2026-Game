#include <algorithm>
#include "Texture.h"

// DON'T FORGET TEXTURE MODE: Wrap, Mirror, Clamp etc later
Colour Texture::SampleNearest(float u, float v) const {
	u = std::clamp(u, 0.f, 1.f);
	v = std::clamp(v, 0.f, 1.f);

	int x = static_cast<int>(u * (width - 1));
	int y = static_cast<int>(v * (height - 1));
	return texels.at(y * width + x);
}

Colour Texture::SampleBilinear(float u, float v) const {
	// TODO: implement this later.
	return Colour();
}

Texture::Texture(const std::string& texturePath) {
	/*int channels;
	unsigned char* rawData = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);

	if (rawData == NULL) {
		// throw an error;
		throw std::runtime_error("Failed to load texture: " + texturePath);
	}

	std::vector<unsigned char> data(rawData, rawData + (width * height * 4)); // 4 = rgba channels.
	texels.resize(width * height);
	stbi_image_free(rawData);

	for (int texelIdx = 0; texelIdx < width * height; texelIdx++) {
		int byteOffset = texelIdx * 4;

		texels[texelIdx] = Colour(data[byteOffset] / 255.f, // R
			data[byteOffset + 1] / 255.f, // G
			data[byteOffset + 2] / 255.f, // B
			data[byteOffset + 3] / 255.f); // A
	}*/
	height = 5;
	width = 5;

}