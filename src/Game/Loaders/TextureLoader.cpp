#include "TextureLoader.h"
#include "Graphics/Colour.h"
#include <ios>

namespace TextureLoader {
	std::unordered_map<std::string, Texture> textureMap;
}

enum TGAImageType {
	NO_IMAGE = 0,
	UNCOMPRESSED_TRUE_COLOR = 2,		// RGB/RGBA
	UNCOMPRESSED_GRAYSCALE = 3,			// BW
	// May be able to include RLE decompression if we really need to use compressed files.
	RLE_TRUE_COLOR = 10,				// Compressed RGB/RGBA 
	RLE_GRAYSCALE = 11					// Compressed BW
};

#pragma pack(push, 1)
struct TGAHeader {
	uint8_t idLength;
	uint8_t colourMapType;
	uint8_t dataTypeCode;
	uint16_t colourMapOrigin;
	uint16_t colourMapLength;
	uint8_t colourMapDepth;
	uint16_t xOrigin; // left
	uint16_t yOrigin; // bottom
	uint16_t width;
	uint16_t height;
	uint8_t bitsPerPixel; // 24 for Targa 24: proper 8 bit channels RGB, 32 for Targa 32: RGBA 8 bits each -> this is for RGB unmapped, 8 bits for BW.
	uint8_t imageDescriptor;
};
#pragma pack(pop)

struct TGAImage {
	int width; 
	int height; 
	int channels;
	std::vector<uint8_t> data;
};

namespace {
	// Helper functions
	/*Texture ProcessTGA_RGBA(std::string& path) {
		return Texture();
	}

	Texture ProcessTGA_RGB(std::string& path) {
		return Texture();
	}

	Texture ProcessTGA_BW(std::string& path) {
		return Texture();
	}*/

	void ReadRLE(std::ifstream& file, std::vector<uint8_t>& data, int channels) {
		size_t pixelCount = data.size() / channels;
		size_t currPixel = 0;

		std::vector<uint8_t> pixelBuffer(channels);

		while (currPixel < pixelCount) {
			uint8_t chunkHeader;
			file.read(reinterpret_cast<char*>(&chunkHeader), 1);

			// If is raw packet
			if (chunkHeader < 128) {
				int count = chunkHeader + 1;
				
				for (int i = 0; i < count; ++i) {
					file.read(reinterpret_cast<char*>(pixelBuffer.data()), channels);
					for (int c = 0; c < channels; c++) {
						data[currPixel * channels + c] = pixelBuffer[c];
						//std::memcpy(&data[currPixel * channels], pixelBuffer.data(), channels);
					}

					currPixel++;
				}
			} // If RLE packet
			else {
				int count = chunkHeader - 127;
				file.read(reinterpret_cast<char*>(pixelBuffer.data()), channels);

				count = std::min<int>(count, pixelCount - currPixel);
				for (int i = 0; i < count; i++) {
					for (int c = 0; c < channels; c++) {
						data[currPixel * channels + c] = pixelBuffer[c];
						//std::memcpy(&data[currPixel * channels], pixelBuffer.data(), channels);
					}

					currPixel++;
				}
			}
		}
	}

}

// May be able to support RGB, RGBA, and BW from here by reading the header, or send it out to helpers for each
Texture TextureLoader::ProcessTGA(std::string& path) {
	std::ifstream file{ path, std::ios::binary };

	TGAHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(TGAHeader));
	file.seekg(header.idLength, std::ios::cur);

	TGAImage image;
	image.width = header.width;
	image.height = header.height;
	image.channels = header.bitsPerPixel / 8;

	bool isGreyScale = (header.dataTypeCode == 3 || header.dataTypeCode == 11);
	bool isRGB = (header.dataTypeCode == 2 || header.dataTypeCode == 10);
	bool isRLE = (header.dataTypeCode >= 9);

	if (!isGreyScale && !isRGB) {
		throw std::runtime_error("Unsupported TGA format: " + path);
	}

	// bitsPerPixel, BW: 8, RGB: 24, RGBA: 32
	if (image.channels != 1 && image.channels != 3 && image.channels != 4) {
		throw std::runtime_error("Unsupported number of channels per pixel: " + path);
	}

	size_t imageSize = image.width * image.height * image.channels;
	image.data.resize(imageSize);

	if (isRLE) {
		ReadRLE(file, image.data, image.channels);
	}
	else {
		file.read(reinterpret_cast<char*>(image.data.data()), imageSize);
	}

	// TGA is BGR, we want it RGB
	if (isRGB && image.channels >= 3) {
		for (size_t i = 0; i < image.data.size(); i += image.channels) {
			std::swap(image.data[i], image.data[i + 2]);
		}
	}

	// Finally output it as a Texture with colours
	Texture t;
	t.height = image.height;
	t.width = image.width;
	t.texels.reserve(image.height * image.width);

	if (isGreyScale) {
		t.channelType = TextureChannels::BW;
	}
	else if (isRGB && image.channels == 3) {
		t.channelType = TextureChannels::RGB;
	}
	else {
		t.channelType = TextureChannels::RGBA;
	}

	Colour c;
	for (size_t i = 0; i < image.data.size(); i += image.channels) {
		if (t.channelType == TextureChannels::BW) {
			float col = static_cast<float>(image.data[i]) / 255.f;
			c = Colour(col, col, col);
		}
		else if (t.channelType == TextureChannels::RGB) {
			float r = static_cast<float>(image.data[i]) / 255.f;
			float g = static_cast<float>(image.data[i + 1]) / 255.f;
			float b = static_cast<float>(image.data[i + 2]) / 255.f;
			c = Colour(r, g, b);
		}
		else if (t.channelType == TextureChannels::RGBA) {
			float r = static_cast<float>(image.data[i]) / 255.f;
			float g = static_cast<float>(image.data[i + 1]) / 255.f;
			float b = static_cast<float>(image.data[i + 2]) / 255.f;
			float a = static_cast<float>(image.data[i + 3]) / 255.f;
			c = Colour(r, g, b, a);
		}

		t.texels.push_back(c);
	}

	return t;
}