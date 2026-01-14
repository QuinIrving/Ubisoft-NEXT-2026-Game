#include "TextureLoader.h"
#include "Graphics/Colour.h"
#include <ios>
#include <algorithm>
#include <Math/MathConstants.h>
#include <unordered_set>

namespace TextureLoader {
	std::unordered_map<std::string, Texture> textureMap;
}

namespace {
	enum GridDirection {
		UP,
		RIGHT,
		DOWN,
		LEFT
	};

	uint64_t MakeEdgeKey(uint32_t a, uint32_t b) {
		uint64_t key = (static_cast<uint64_t>(a) << 32) | b;
		return key;
	}

	static const GridDirection CCWPriority[4][3] = {
		{ GridDirection::LEFT, GridDirection::UP, GridDirection::RIGHT }, // UP
		{ GridDirection::UP, GridDirection::RIGHT, GridDirection::DOWN }, // RIGHT
		{ GridDirection::RIGHT, GridDirection::DOWN, GridDirection::LEFT }, // DOWN
		{ GridDirection::DOWN, GridDirection::LEFT, GridDirection::UP } // LEFT
	};

	constexpr uint32_t OUTSIDE_REGION = 0xFFFFFFFF;
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

	/*
	void GetSectorVarianceAndAverageColour(Vec2<float> offset, int boxSize, Vec3<float>& avgCol, float& variance) {
		Vec3<float> colourSum;
		Vec3<float> squaredColourSum;
		float sampleCount = 0.0;

		for (int y = 0; y < boxSize; ++y) {
			for (int x = 0; x < boxSize; ++x) {

			}
		}
	}*/
	/*
	void GetBoxedSectorVarianceAndAverageColour(Vec2<float> offset, int boxSize, Vec3<float>& avgColour, float& variance) {
		Vec3<float> colourSum;
		Vec3<float> squaredColourSum;
		float sampleCount = 0.0;

		for (int y = 0; y < boxSize; ++y) {
			for (int x = 0; x < boxSize; ++x) {

			}
		}
	}*/
	// Sobel Kernels
	const Mat4<float> Gx = Mat4<float>({ Vec4<float>(-1, -2, -1, 0), Vec4<float>(0, 0, 0, 0), Vec4<float>(1, 2, 1, 0), Vec4<float>(0, 0, 0, 1) }); // x-dir kernel
	const Mat4<float> Gy = Mat4<float>({ Vec4<float>(1, 0, -1, 0), Vec4<float>(2, 0, -2, 0), Vec4<float>(1, 0, -1, 0), Vec4<float>(0, 0, 0, 1) }); // y-dir kernel

	Colour ComputeStructureTensor(Texture& t, Vec2<int> uv) {
		Vec3<float> tx0y0 = t.texels[(uv.y - 1) * t.width + (uv.x - 1)].GetVectorizedRGB();
		Vec3<float> tx0y1 = t.texels[(uv.y) * t.width + (uv.x - 1)].GetVectorizedRGB();
		Vec3<float> tx0y2 = t.texels[(uv.y + 1) * t.width + (uv.x - 1)].GetVectorizedRGB();
		Vec3<float> tx1y0 = t.texels[(uv.y - 1) * t.width + (uv.x)].GetVectorizedRGB();
		Vec3<float> tx1y1 = t.texels[(uv.y) * t.width + (uv.x)].GetVectorizedRGB();
		Vec3<float> tx1y2 = t.texels[(uv.y + 1) * t.width + (uv.x)].GetVectorizedRGB();
		Vec3<float> tx2y0 = t.texels[(uv.y - 1) * t.width + (uv.x + 1)].GetVectorizedRGB();
		Vec3<float> tx2y1 = t.texels[(uv.y) * t.width + (uv.x + 1)].GetVectorizedRGB();
		Vec3<float> tx2y2 = t.texels[(uv.y + 1) * t.width + (uv.x + 1)].GetVectorizedRGB();

		// Jxx is avg of squared x-derivatives, magnitude of gradient on x-axis
		// Jyy is avg of squared y-derivatives, magnitude of gradient on y-axis
		// Jxy is avg of product of x and y derivatives, how much the gradients are aligned or orthogonal to each other

		Vec3<float> Sx =	tx0y0 * Gx[0][0] + tx1y0 * Gx[1][0] + tx2y0 * Gx[2][0] +
							tx0y1 * Gx[0][1] + tx1y1 * Gx[1][1] + tx2y1 * Gx[2][1] +
							tx0y2 * Gx[0][2] + tx1y2 * Gx[1][2] + tx2y2 * Gx[2][2];


		Vec3<float> Sy =	tx0y0 * Gy[0][0] + tx1y0 * Gy[1][0] + tx2y0 * Gy[2][0] +
							tx0y1 * Gy[0][1] + tx1y1 * Gy[1][1] + tx2y1 * Gy[2][1] +
							tx0y2 * Gy[0][2] + tx1y2 * Gy[1][2] + tx2y2 * Gy[2][2];


		return Vec4<float>(Sx.DotProduct(Sx), Sy.DotProduct(Sy), Sx.DotProduct(Sy), 1.0);

	}

	float ChannelToLinear(float c) {
		if (c <= 0.04045f) {
			return c / 12.92f;
		}

		return powf(((c + 0.055f) / 1.055f), 2.4f);
	}

	void sRGBToLinear(Vec3<float>& c) {
		c.x = ChannelToLinear(c.x);
		c.y = ChannelToLinear(c.y);
		c.z = ChannelToLinear(c.z);
	}

	Vec3<float> LinearToXYZ(Vec3<float> c) {
		Mat4<float> d65 = Mat4<float>({Vec4<float>(0.4124565f, 0.2126729f, 0.0193339f, 0), Vec4<float>(0.3575761f, 0.7151522f, 0.1191920f, 0), Vec4<float>(0.1804375f, 0.0721750f, 0.9503041f, 0), Vec4<float>(0, 0, 0, 1)});
		return Vec4<float>(c) * d65;
	}

	float LABf(float t) {
		return (t > 0.008856f) ? std::cbrtf(t) : (7.787f * t + 16.f / 116.f);
	}

	Vec3<float> XYZToLAB(Vec3<float> c) {
		Vec3<float> referenceWhiteD65 = { 0.95047f, 1.0f, 1.08883f };
		
		float fx = LABf(c.x / referenceWhiteD65.x);
		float fy = LABf(c.y / referenceWhiteD65.y);
		float fz = LABf(c.z / referenceWhiteD65.z);

		float luminance = 116.f * fy - 16.f;
		float a = (fx - fy) * 500.f;
		float b = (fy - fz) * 200.f;

		return { luminance, a, b };
	}

	float LabPivot(float t) {
		return (t > EPSILON) ? std::cbrtf(t) : (903.3f * t + 16.f) / 116.f;
	}

	float LabPivotInv(float t) {
		float t3 = t * t * t;
		return (t3 > EPSILON) ? t3 : (116.f * t - 16.f) / 903.3f;
	}

	Vec3<float> LABToXYZ(const Vec3<float>& LAB) {
		float fy = (LAB.x + 16.0f) / 116.0f;
		float fx = fy + (LAB.y / 500.0f);
		float fz = fy - (LAB.z / 200.0f);

		float xr = LabPivotInv(fx);
		float yr = LabPivotInv(fy);
		float zr = LabPivotInv(fz);

		Vec3<float> XYZ;
		XYZ.x = xr * 0.95047f;
		XYZ.y = yr * 1.f;
		XYZ.z = zr * 1.08883f;

		return XYZ;
	}

	Vec3<float> XYZToLinear(const Vec3<float>& XYZ) {
		Vec3<float> rgb;
		
		rgb.x = 3.2404542f * XYZ.x - 1.5371385f * XYZ.y - 0.4985314f * XYZ.z;
		rgb.y = -0.9692660f * XYZ.x + 1.8760108f * XYZ.y + 0.0415560f * XYZ.z;
		rgb.z = 0.0556434f * XYZ.x - 0.2040259f * XYZ.y + 1.0572252f * XYZ.z;
		
		return rgb;
	}

	float LinearTosRGB(float c) {
		return (c <= 0.0031308f) ? 12.92f * c : 1.055f * std::powf(c, 1.f / 2.4f) - 0.055f;
	}

	Vec3<float> LinearTosRGB(const Vec3<float>& rgb) {
		return { LinearTosRGB(rgb.x), LinearTosRGB(rgb.y) , LinearTosRGB(rgb.z) };
	}

	float GetGaussianWeight(float distance, float sigma) {
		return std::expf(-(distance * distance) / (2.f * sigma * sigma));
	}

	float GetAnisotropicGaussianWeight(float x, float y, float sigmaX, float sigmaY) {
		float d2 = (x * x) / (2.f * sigmaX * sigmaX) +
			(y * y) / (2.f * sigmaY * sigmaY);

		return std::expf(-d2);
	}

	float GetPolynomialWeight(float x, float y, float eta, float lambda) {
		float polyVal = (x + eta) - lambda * (y * y);
		return std::max<float>(polyVal * polyVal, 0.f);
	}

	float GetAnisotropicPolyWeight(float x, float y, float sigmaX, float sigmaY) {
		float d2 = (x * x) / (sigmaX * sigmaX) + (y * y) / (sigmaY * sigmaY);
		float w = 1.f - d2;
		return (w > 0.f) ? (w * w) : 0.f;
	}

	Vec4<float> GetAnisotropicSectorAverageColourAndVariance(float angle, int radius, Vec2<float> pixel, Texture& t, Vec4<float>& orientationAndAnisotropy) {
		Vec4<float> output;

		// Anisotropic Kuwahara Pass
		Vec2<float> orientation = { orientationAndAnisotropy.x, orientationAndAnisotropy.y };

		//float alpha = 25.f;
		//float alpha = 12.f;
		float alpha = 1.f;
		float anisotropy = (orientationAndAnisotropy.z - orientationAndAnisotropy.w) / (orientationAndAnisotropy.z + orientationAndAnisotropy.w + EPSILON);

		float scaleX = alpha / (anisotropy + alpha);
		float scaleY = (anisotropy + alpha) / alpha;

		Mat4<float> matAnisotropy = Mat4<float>({Vec4<float>(scaleX, 0.f, 0.f, 0.f), Vec4<float>(0.f, scaleY, 0.f, 0.f), Vec4<float>(0, 0, 1, 0), Vec4<float>(0, 0, 0, 1)}) * 
									Mat4<float>({ Vec4<float>(orientation.x, orientation.y, 0, 1), Vec4<float>(-orientation.y, orientation.x, 0, 0), Vec4<float>(0, 0, 1, 0), Vec4<float>(0, 0, 0, 1)});

		Vec3<float> colourSum;
		Vec3<float> colourSumSquared;
		float sampleCount = 0.f;

		float totalWeight = 0.f;

		anisotropy = std::clamp(anisotropy, 0.f, 0.99f);
		float sigma = radius / 3.f;
		//float sigmaX = radius / 3.f;
		//float sigmaY = sigmaX * (1.f - anisotropy);

		// polynomial way instead of gaussian:
		float eta = 0.1f;
		float lambda = 0.5f;

		for (int r = 1; r <= radius; r += 1) {
			for (float a = -(PI / 8.f); a <= PI / 8.f; a += (PI / 16.f)) {
				Vec2<float> pixelOffset = Vec2<float>(std::cosf(angle + a), std::sinf(angle + a)) * float(r);
				Vec4<float> po = Vec4<float>(pixelOffset.x, pixelOffset.y, 0, 1) * matAnisotropy;
				//Vec2<float> rotatedOffset = Vec2<float>(pixelOffset.x * orientation.x + pixelOffset.y * orientation.y, -pixelOffset.x * orientation.y + pixelOffset.y * orientation.x);
				pixelOffset.x = po.x;
				pixelOffset.y = po.y;

				int h = std::clamp<int>(int(std::roundf(pixel.y + pixelOffset.y)), 0, t.height - 1);
				int w = std::clamp<int>(int(std::roundf(pixel.x + pixelOffset.x)), 0, t.width - 1);

				Colour c = t.texels[h * t.width + w];
				Vec3<float> rgb = c.GetVectorizedRGB();

				// Since we are reading diffuse/albedo textures, typically will be in sRGB space, so will use the proper conversion:
				//sRGBToLinear(rgb);

				// 2D Gaussian function:
				// f(x, y) = e^(-(x^2 + y^2) / (2 * stand.Deviation^2))
				//rotatedOffset.x /= scaleX;
				//rotatedOffset.y /= scaleY;
				//float weight = std::exp(-(rotatedOffset.x * rotatedOffset.x + rotatedOffset.y * rotatedOffset.y) / (2.f * sigma * sigma));
				//float weight = GetAnisotropicGaussianWeight(pixelOffset.x, pixelOffset.y, sigmaX, sigmaY);
				float weight = GetGaussianWeight(pixelOffset.GetMagnitude(), sigma);

				// Polynomial way (faster):
				//float weight = GetPolynomialWeight(pixelOffset.x, pixelOffset.y, eta, lambda);

				colourSum += rgb * weight;
				colourSumSquared += rgb * rgb * weight;
				totalWeight += weight;
			}
		}

		// Calculate avg colour and variance
		Vec3<float> colourAvg = colourSum / totalWeight;
		Vec3<float> variance = (colourSumSquared / totalWeight) - (colourAvg * colourAvg);
		float luminanceVariance = variance.DotProduct(Vec3<float>(0.299f, 0.587f, 0.114f));// Uses NTSC formula to calculate luminance from RGB

		output.x = colourAvg.x;
		output.y = colourAvg.y;
		output.z = colourAvg.z;
		output.w = luminanceVariance;

		// Post-process if we want.

		return output;
	}

	Vec4<float> GetBoxedSectorAverageColourAndVariance(Vec2<int> sectorTL, int boxSize, Texture& t) {
		// The x,y,z components inform of the average colour, and the w (or 4th) component contains the variance for the pixel.
		Vec4<float> output;

		Vec3<float> colourSum;
		Vec3<float> colourSumSquared;
		float sampleCount = 0.f;

		for (int y = sectorTL.y; y < sectorTL.y + boxSize; ++y) {
			int h = std::clamp<int>(y, 0, t.height - 1);

			for (int x = sectorTL.x; x < sectorTL.x + boxSize; ++x) {
				int w = std::clamp<int>(x, 0, t.width - 1);

				Colour c = t.texels[h * t.width + w];
				Vec3<float> rgb = c.GetVectorizedRGB();

				// Since we are reading diffuse/albedo textures, typically will be in sRGB space, so will use the proper conversion:
				//sRGBToLinear(rgb);

				colourSum += rgb;
				colourSumSquared += rgb * rgb;
				sampleCount += 1.f;
			}
		}

		// Calculate avg colour and variance
		Vec3<float> colourAvg = colourSum / sampleCount;
		Vec3<float> variance = (colourSumSquared / sampleCount) - (colourAvg * colourAvg);
		float luminanceVariance = variance.DotProduct(Vec3<float>(0.299f, 0.587f, 0.114f));// Uses NTSC formula to calculate luminance from RGB

		output.x = colourAvg.x;
		output.y = colourAvg.y;
		output.z = colourAvg.z;
		output.w = luminanceVariance;

		return output;
	}

	// Circular with also a gaussian weight (or polynomial estimated) per pixel colour contribution
	Vec4<float> GetPapariSectorAverageColourAndVariance(float angle, int radius, Vec2<float> pixel, Texture& t) {
		Vec4<float> output;

		Vec3<float> colourSum;
		Vec3<float> colourSumSquared;
		float sampleCount = 0.f;

		float totalWeight = 0.f;
		float sigma = radius / 3.f;
		// polynomial way instead of gaussian:
		float eta = 0.1f;
		float lambda = 0.5f;

		for (int r = 1; r <= radius; r += 1) {
			for (float a = -(PI / 8.f); a <= PI / 8.f; a += (PI / 16.f)) {
				Vec2<float> pixelOffset = Vec2<float>(std::cosf(angle + a), std::sinf(angle + a)) * float(r);

				int h = std::clamp<int>(int(std::roundf(pixel.y + pixelOffset.y)), 0, t.height - 1);
				int w = std::clamp<int>(int(std::roundf(pixel.x + pixelOffset.x)), 0, t.width - 1);

				Colour c = t.texels[h * t.width + w];
				Vec3<float> rgb = c.GetVectorizedRGB();

				// Since we are reading diffuse/albedo textures, typically will be in sRGB space, so will use the proper conversion:
				//sRGBToLinear(rgb);

				// 2D Gaussian function:
				// f(x, y) = e^(-(x^2 + y^2) / (2 * stand.Deviation^2))
				//float weight = GetGaussianWeight(pixelOffset.GetMagnitude(), sigma);

				// Polynomial way (faster):
				float weight = GetPolynomialWeight(pixelOffset.x, pixelOffset.y, eta, lambda);

				/*colourSum += rgb;
				colourSumSquared += rgb * rgb;
				sampleCount += 1.f;*/

				colourSum += rgb * weight;
				colourSumSquared += rgb * rgb * weight;
				totalWeight += weight;
			}
		}

		// Calculate avg colour and variance
		//Vec3<float> colourAvg = colourSum / sampleCount;
		//Vec3<float> variance = (colourSumSquared / sampleCount) - (colourAvg * colourAvg);
		//float luminanceVariance = variance.DotProduct(Vec3<float>(0.299f, 0.587f, 0.114f));// Uses NTSC formula to calculate luminance from RGB

		Vec3<float> colourAvg = colourSum / totalWeight;
		Vec3<float> variance = (colourSumSquared / totalWeight) - (colourAvg * colourAvg);
		float luminanceVariance = variance.DotProduct(Vec3<float>(0.299f, 0.587f, 0.114f));// Uses NTSC formula to calculate luminance from RGB

		output.x = colourAvg.x;
		output.y = colourAvg.y;
		output.z = colourAvg.z;
		output.w = luminanceVariance;

		return output;
	}

	Vec4<float> GetDominantOrientation(Vec3<float>& structureTensor) {
		float Jxx = structureTensor.x;
		float Jyy = structureTensor.y;
		float Jxy = structureTensor.z;

		float trace = Jxx + Jyy;
		float determinant = Jxx * Jyy - Jxy * Jxy;


		float lambda1 = trace * 0.5f + std::sqrtf(trace * trace * 0.25f - determinant);
		float lambda2 = trace * 0.5f - std::sqrtf(trace * trace * 0.25f - determinant);

		float jxyStrength = std::abs(Jxy) / (std::abs(Jxx) + std::abs(Jyy) + std::abs(Jxy) + EPSILON);
		Vec2<float> v;
		if (jxyStrength > 0.f) {
			v = Vec2<float>(-Jxy, Jxx - lambda1).GetNormalized();
		}
		else {
			v = Vec2<float>(0.f, 1.f);
		}

		return Vec4<float>(v.x, v.y, lambda1, lambda2);
	}

	Texture ApplyKuwaharaFilter(Texture& t) {
		/*
		Regular Kuwahara Filter:
		FOR EACH PIXEL
		- Center a box around a pixel,
		- divide the box into 4 "sub-boxes" called sectors
		- Calculate the average colour and variance for each sector
		- Set our pixel to the average colour of the sector with the lowest variance
		variance is the stand deviation squared = the sum of [(the difference betwwen a colour and the mean colour)squared] for all colours in the sector divided by the number of colours in the sector
		=> when a pixel sits outside an edge, the sector with lowest variance will always be outside that edge
		=> when a pixel sits inside an edge, the sector with the lowest variance will be inside the edge.

		size of the sector = kernel size.

		!!!! Look into the sector edge cases (such as a pixel at the corner of an image) !!!!
		====== Papari extension ======
		- Remove square shaped kernel to a circular one
		- Improve weight influence each pixel has on the filter
		8 sectors is the ideal amount to balance output quality and good performance.

		--- Better weighting of colours ---
		Gausian weights fix the issue, provides a gradual fall-off from center of sector to its edges
		emphasizes central pixels, seems to also be radius of 2 pixels
		2D Gaussian function: f(x,y) = e^(-(x^2 + y^2) / (2 * (stand. dev ^ 2))
		compute weight via gaus formula, take the resulting weight into account when calcing weighted colour sum and weighted squared colour sum,
		the weights are then reflected in final result of variance and average colour for each sector.

		--- Fixing performance ---
		Gaussian function is expensive, so we should use the Anisotropic Kuwahara Filtering with Polynomial Weighting functions:
		[(x + zeta) - n(eta)y^2]^2

		====== Anisotropic Kuwahara filter ======
		anisotropic = one dominant direction of change.
		can adapt our circular kernel to local features of input during sampling by squeezing & rotating it. SO our sectors are an ellipsis rather than circle

		Multi-pass:
		=> First pass
		- Take underlying scene as input and return the structure of the scene
		- Based on the structure as input, we'll apply our kuwahara filter to the underlying scene
		=> second pass
		- Apply some tone mapping and other details to help it

		A.k.a
		Apply Sobel operator to scene to get the "structure tensor"
		Apply anisotropic kuwahara filter on original scene with tensor structure on (we need to have both the original image input and the structure tensor)
		Apply final pass for tone mapping, quantization, saturation etc

		Apply sobel matrix for every pixel, but instead of rapid change in intensity, extract the partial derivatives along the x/y axis representing the rate of change.
		those partial derivatives allow us to obtain a structure tensor
		Which is a math tool in image processing to describe the local structure and orientation of an image.
		J = [[Jxx, Jxy], [Jxy, Jyy]]
		Sx = [[-1, 0, 1], [-2, 0, 2], [-1, 0, 1]]
		Sy = [[-1, -2, -1], [0, 0, 0], [1, 2, 1]]
		Jxx = dot(Sx, Sx), Jyy = dot(Sy, Sy) & Jxy = dot(Sx, Sy)

		Jxx: average of the squared x-derivatives, representing magnitude of gradient on x-axis
		Jyy: average of squared y-derivatives, representing magnitude of gradient on y-axis
		Jxy: average of product of x & y derivatives, representing how much the gradients are aligned/orthogonal to each other

		Edglines have possible colours:
		- Red: strong vertical rate of change
		- Green: Strong Horizontal rate of change
		- Yellow: rate of change in both x and y axis.

		We want to know direction a pixel points (dominant direction, and orientation of tensort at a given position)
		Which eigenvalues and eigenvectors can provide.

		EigenVectors: represent directions where a transformation or a matrix results in those vectors stretching or shrinking without changing directions.
		there is a non-zero vector v satisfying:
		- A * v = lambda * v where A is a transformation matrix and lambda is the eigenvalue for that vector

		Eigenvalues represent the intensity of the streching/shrinking of the transformation
		In our case:
		- There are 2 eigenvalues for our Structure Tensor Lambda 1 and Lambda 2.
		- They represent strength of gradient in the direction of the strongest intensity change, usually indicating an edge, and the strength of the gradient in the
			direction orthogonal to the strongest intensity of change respectively
		- Each eigenvector is associated with an eigenvalue representing the strongest and weakest rate of change

		Mat: [[a, b], [b,c]]
		- It's trace is a + d
		- It determinant is a * d - b^2

		Through our structure tensor, we'd like to obtain lamda 1, and derive its corresponding eigenvector, giving us dominant direction of the local struicture to adapt our filter.
		Can do so via starting from the definition of eigen vector: A * v = lambda * v
		Given matrix: [[a, b], [b, c]] we have:
		(A - lambda * I) * v = 0 (I is identity matrix)
		Since v is non-null, only way for the product of matrix transform and non-zero vector to be null is if transformation completely squishes space,
		which can be represented by a zero determinant for that matrix transform, hence det(A - lambda * I) = 0
		- det([[a - lambda, b], [b, d - lambda]]) = 0
		- By applying formula of determinant we get:
			(a - lambda) * (d - lambda) - b^2 = 0
		Simplify: Lamda^2 - (a + d)lambda + (ad - b^2) = 0
		^ Quadratic equation to solve for lambda.

		SOlution of the quadratic equation gives us 2 possible values for lambda:
		Lambda = [(a + d) +- sqrt((a + d)^2 - 4 * (ad - b^2))] / 2
		Simplify with determinant and trace of transformation matrix:
		Lambda = [trace +- sqrt(trace^2 - 4 * determinant)] / 2
		^ Use this formula to compute both eigenvalues

		Now we pick the largest eigenvalue out of the two to derive our eigenvector
		- (A - lambda * I) * v = 0
		- [Jxx - lambda, Jxy] [vx] = [0] and [Jxy, Jyy - lambda] * [vy] [0]
		- (Jxx - lambda) * vx + Jxy * vy = 0 AND Jxy *vx + (Jyy - lambda) * vy = 0
		- vx = -Jxy * vy / (Jxx - lambda), by setting vy to 1 we get:
			- vx = -Jxy / (Jxx - lambda)
		- Multiply both components by Jxx - lambda, and obtain the final value for our eigen vector:
			- v = (-Jxy, Jxx - lambda)
		^ Apprently resort to using this eigenvector only if Jxy relative to the other components of the tensor matrix was above zero.

		Anisotropy A using those eigenvalue as:
		A = (lambda1 - lambda2) / (lambda1 + lambda2 + 1e-7)
		(1e-7 to denominator to avoid potential 0 values)

		- A ranges from 0 to 1
		- When lambda1 = lambda2, the anisotropy is 0, representing an isotropic region
		- When lambda1 > lambda2, the anisotropy tends towards 1, representing an anisotropic region.
			- Parameter "alpha" represents intensity of the anisotropy of filter. Suggests default of alpha = 1.0, however, it can make the filter very noisy at high kernel size, but
			they found that any value above 10 yielded something satisfying by 25 was a good looking output

		- As anisotropy reaches 1, the ellipsis becomes more elongated along the axis, stretching our circular kernel
		- When we have a relatively isotropic region, the scale factors are 1 resulting in our kernel remaining circular.
		We now stretch and scale our kernel based on the anisotropy, remaining task is to orient it accordingly uising eigenvector computed previously and defining a rotation matrix from it.

		Last step simply consists of applying the matrix to our sampleOffset, and now the Kuwahara filter is anisotropic.
		---
		=== Final touches ===
		Can add final post-processing to our scene:
		- quantization (I'm doing)
		- colour interpolation
		- saturation
		- tone mapping
		- adding some texture to output

		Quantization:
		floor(colour * (n - 1) + 0.5) / n - 1, where n is the total num of colours
		Quantization by:
		- calculating the LAB value of the curr pixel, setting the number of colours N (for me 16, may do 64 or 8 idk, or maybe different depending on the use-case (topology vs scene)
		- use formula established above
		- clamp the range to avoid extreme values which wouldn't yield a realistic painting effect

		Can also add two-point interpolation to blend our colours with our quantized image
		- for a darker quantized pixel, we'd opt to interpolated from black to image colour based on quantization value
		- FOr a lighted quantized pixel, interpoalted from image to white
		Emphasizes contrast b/en light and dark areas. (This looked really cool in the images I saw)

		Then added some nice ACES tone mapping (I believe I will be doing this anyway on my HDR stuff.
		Can also add some paper-like texture and blend it with colour output (PBR type idea with my stuff lol)

		*/
		
		std::vector<Colour> kuwaharaPixels;

		/*for (const Colour& c : t.texels) {

		}*/

		const int KERNEL_SIZE = 7;
		//const int KERNEL_SIZE = 5;
		//const int SECTOR_COUNT = 4;

		//std::vector<Vec3<float>> boxAvgCols;
		//std::vector<float> boxVariances;

		//boxAvgCols.reserve(SECTOR_COUNT);
		//boxVariances.reserve(SECTOR_COUNT);

		// Each time we make it so we set an offset from the current pixel as the top left of the sector, as long as it knows the box size, it should be correct.
		Vec2<float> sectorTopLeft;

		// we can make it pass back a Vec4<float> with it being first 3 the average colour, and the final float the variance
		/*for (int y = 0; y < t.height; ++y) {
			for (int x = 0; x < t.width; ++x) {
				// reset our per-pixel sector containers
				boxAvgCols.clear();
				boxVariances.clear();

				Vec4<float> sectorOutput;
				// For each pixel, we calculate the sector variance and colour.
				sectorOutput = GetBoxedSectorAverageColourAndVariance(Vec2<int>(x - KERNEL_SIZE + 1, y - KERNEL_SIZE + 1), KERNEL_SIZE, t); // Top Left Box
				boxAvgCols.push_back(sectorOutput);
				boxVariances.push_back(sectorOutput.w);

				sectorOutput = GetBoxedSectorAverageColourAndVariance(Vec2<int>(x, y - KERNEL_SIZE + 1), KERNEL_SIZE, t); // Top Right Box
				boxAvgCols.push_back(sectorOutput);
				boxVariances.push_back(sectorOutput.w);

				sectorOutput = GetBoxedSectorAverageColourAndVariance(Vec2<int>(x - KERNEL_SIZE + 1, y), KERNEL_SIZE, t); // Bottom Left Box
				boxAvgCols.push_back(sectorOutput);
				boxVariances.push_back(sectorOutput.w);

				sectorOutput = GetBoxedSectorAverageColourAndVariance(Vec2<int>(x, y), KERNEL_SIZE, t); // Bottom Right Box
				boxAvgCols.push_back(sectorOutput);
				boxVariances.push_back(sectorOutput.w);

				float minVariance = boxVariances[0];
				Vec3<float> finalColour = boxAvgCols[0];

				for (int i = 1; i < SECTOR_COUNT; ++i) {
					if (boxVariances[i] < minVariance) {
						finalColour = boxAvgCols[i];
						minVariance = boxVariances[i];
					}
				}

				kuwaharaPixels.push_back(Colour(finalColour.x, finalColour.y, finalColour.z, 1.f));
			}
		}*/

		/*const int SECTOR_COUNT = 8;

		std::vector<Vec3<float>> avgCols;
		std::vector<float> variances;

		avgCols.reserve(SECTOR_COUNT);
		variances.reserve(SECTOR_COUNT);

		for (int y = 0; y < t.height; ++y) {
			for (int x = 0; x < t.width; ++x) {
				// reset our per-pixel sector containers
				avgCols.clear();
				variances.clear();

				Vec4<float> sectorOutput;

				for (int i = 0; i < SECTOR_COUNT; ++i) {
					float angle = (float(i) * (2 * PI)) / float(SECTOR_COUNT);

					// For each pixel, we calculate the sector variance and colour.
					sectorOutput = GetPapariSectorAverageColourAndVariance(angle, KERNEL_SIZE, Vec2<float>(x, y), t); // Top Left Box
					avgCols.push_back(sectorOutput);
					variances.push_back(sectorOutput.w);
				}
				
				float minVariance = variances[0];
				Vec3<float> finalColour = avgCols[0];

				for (int i = 1; i < SECTOR_COUNT; ++i) {
					if (variances[i] < minVariance) {
						finalColour = avgCols[i];
						minVariance = variances[i];
					}
				}

				kuwaharaPixels.push_back(Colour(finalColour.x, finalColour.y, finalColour.z, 1.f));

				/*
				sectorOutput = GetBoxedSectorAverageColourAndVariance(Vec2<int>(x, y - KERNEL_SIZE + 1), KERNEL_SIZE, t); // Top Right Box
				boxAvgCols.push_back(sectorOutput);
				boxVariances.push_back(sectorOutput.w);

				sectorOutput = GetBoxedSectorAverageColourAndVariance(Vec2<int>(x - KERNEL_SIZE + 1, y), KERNEL_SIZE, t); // Bottom Left Box
				boxAvgCols.push_back(sectorOutput);
				boxVariances.push_back(sectorOutput.w);

				sectorOutput = GetBoxedSectorAverageColourAndVariance(Vec2<int>(x, y), KERNEL_SIZE, t); // Bottom Right Box
				boxAvgCols.push_back(sectorOutput);
				boxVariances.push_back(sectorOutput.w); 
				* /
			}
		}*/


		// Anisotropic method requires multiple passes.
		const int SECTOR_COUNT = 8;
		std::vector<Colour> sobelColours;
		sobelColours.reserve(t.height* t.width);

		// Let's add rgb of 0 for the first 

		for (int y = 0; y < t.height; ++y) {
			for (int x = 0; x < t.width; ++x) {
				if (y == 0 || y == t.height - 1 || x == 0 || x == t.width - 1) {
					sobelColours.push_back(Colour(0.f, 0.f, 0.f, 1.f));
					continue;
				}

				sobelColours.push_back(ComputeStructureTensor(t, Vec2<int>(x, y)));
			}
		}

		// Lets say we have some form of 2d matrix for change of basis,
		// We then do (00 - lambda)(11 - lambda) - [0][1] * [1][0] quadratic polynomial in lambda, and can only be a eigenvalue if the result of that is 0,
		// then to get the eigen vector, plug in the eigenvalue as lambda into the matrix, and solve for x,y to get an eigen vector, although technically could have no eigenvectors
		// only when determinant is imaginary numbers

		// FOr 2x2 matrix [a, b]
		//				  [b, c]
		// Trace = a + c, det = ac - b^2. (Generic formula is technically: trace = a + d, and determinant = a*x - b^2.
		// we can get the eigenvalues by solving for 0 on the polynomial general case:
		// (a - lambda) * (d-lambda) - b^2
		// We can solves this generally via the quadratic equation:
		// [trace +- sqrt(trace^2 - 4*determinant)] / 2
		std::vector<Vec4<float>> pixelOrientations;
		pixelOrientations.reserve(sobelColours.size());

		for (Colour& c : sobelColours) {
			pixelOrientations.push_back(GetDominantOrientation(c.GetVectorizedRGB()));
		}

		std::vector<Vec3<float>> avgCols;
		std::vector<float> variances;

		avgCols.reserve(SECTOR_COUNT);
		variances.reserve(SECTOR_COUNT);

		for (int y = 0; y < t.height; ++y) {
			for (int x = 0; x < t.width; ++x) {
				// reset our per-pixel sector containers
				avgCols.clear();
				variances.clear();

				Vec4<float> sectorOutput;

				for (int i = 0; i < SECTOR_COUNT; ++i) {
					float angle = (float(i) * (2 * PI)) / float(SECTOR_COUNT);

					// For each pixel, we calculate the sector variance and colour.
					sectorOutput = GetAnisotropicSectorAverageColourAndVariance(angle, KERNEL_SIZE, Vec2<float>(x, y), t, pixelOrientations[y * t.width + x]);
					avgCols.push_back(sectorOutput);
					variances.push_back(sectorOutput.w);
				}

				float minVariance = variances[0];
				Vec3<float> finalColour = avgCols[0];

				for (int i = 1; i < SECTOR_COUNT; ++i) {
					if (variances[i] < minVariance) {
						finalColour = avgCols[i];
						minVariance = variances[i];
					}
				}

				kuwaharaPixels.push_back(Colour(finalColour.x, finalColour.y, finalColour.z, 1.f));
			}
		}

		Texture kuwaharaT;
		kuwaharaT.height = t.height;
		kuwaharaT.width = t.width;
		kuwaharaT.channelType = t.channelType;

		kuwaharaT.texels = kuwaharaPixels;
		//kuwaharaT.texels = sobelColours;

		return kuwaharaT;
	}

}

// May be able to support RGB, RGBA, and BW from here by reading the header, or send it out to helpers for each
Texture TextureLoader::ProcessTGA(std::string& path) {
	std::ifstream file{ path, std::ios::binary };

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open TGA file: " + path);
	}

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

float LabDistanceSq(const Vec3<float>& a, const Vec3<float>& b) {
	Vec3<float> d = a - b;
	return d.GetMagnitudeSquared();
}

struct LABPixel {
	float L;
	float a;
	float b;
};

struct LABBox {
	std::vector<int> indices; // indices of labPixels;
	Vec3<float> min;
	Vec3<float> max;
};

void ComputeBounds(LABBox& box, const std::vector<LABPixel>& pixels) {
	//box.min = { FLT_MAX, FLT_MAX, FLT_MAX };
	box.min = { FLT_MIN, FLT_MAX, FLT_MAX }; // This bug actually seems to help us with blending, as it reduces luminance as a factor.
	//box.min = { -FLT_MAX, FLT_MAX, FLT_MAX };
	box.max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (int i : box.indices) {
		const auto& p = pixels[i];
		box.min.x = std::min<float>(box.min.x, p.L);
		box.min.y = std::min<float>(box.min.y, p.a);
		box.min.z = std::min<float>(box.min.z, p.b);

		box.max.x = std::max<float>(box.max.x, p.L);
		box.max.y = std::max<float>(box.max.y, p.a);
		box.max.z = std::max<float>(box.max.z, p.b);
	}
}

void SplitBox(LABBox& box, LABBox& outA, LABBox& outB, const std::vector<LABPixel>& pixels) {
	Vec3<float> range = box.max - box.min;

	int axis = 0;
	float maxRange = range.x;

	if (range.y > maxRange) {	
		axis = 1; 
		maxRange = range.y;
	}
	if (range.z > maxRange) { 
		axis = 2; 
	}

	auto& idx = box.indices;

	std::nth_element(
		idx.begin(),
		idx.begin() + idx.size() / 2,
		idx.end(),
		[&](int i1, int i2) {
			const LABPixel& p1 = pixels[i1];
			const LABPixel& p2 = pixels[i2];
			return (axis == 0 ? p1.L : axis == 1 ? p1.a : p1.b) <
				(axis == 0 ? p2.L : axis == 1 ? p2.a : p2.b);
		}
	);

	size_t mid = idx.size() / 2;

	outA.indices.assign(idx.begin(), idx.begin() + mid);
	outB.indices.assign(idx.begin() + mid, idx.end());

	ComputeBounds(outA, pixels);
	ComputeBounds(outB, pixels);
}

std::vector<Vec3<float>> BuildLABPalette(
	const std::vector<LABPixel>& pixels,
	int numColours)
{
	std::vector<LABBox> boxes;

	LABBox root;
	//root.indices.reserve(pixels.size());
	root.indices.resize(pixels.size());
	for (int i = 0; i < pixels.size(); ++i) {
		root.indices.push_back(i);
	}

	ComputeBounds(root, pixels);

	boxes.push_back(root);

	while ((int)boxes.size() < numColours)
	{
		// Find box with largest volume
		int splitIndex = 0;
		float bestExtent = 0.f;

		for (int i = 0; i < (int)boxes.size(); ++i) {
			Vec3<float> e = boxes[i].max - boxes[i].min;
			//float extent = std::max<float>({ e.x * 0.5f, e.y, e.z });
			float extent = std::max<float>({ e.x, e.y, e.z });
			if (extent > bestExtent && boxes[i].indices.size() > 1) {
				bestExtent = extent;
				splitIndex = i;
			}
		}

		LABBox a, b;
		SplitBox(boxes[splitIndex], a, b, pixels);

		boxes[splitIndex] = a;
		boxes.push_back(b);
	}

	// Compute mean LAB per box
	std::vector<Vec3<float>> palette;
	palette.reserve(boxes.size());

	for (auto& box : boxes) {
		Vec3<float> sum = { 0, 0, 0 };
		for (int i : box.indices) {
			sum.x += pixels[i].L;
			sum.y += pixels[i].a;
			sum.z += pixels[i].b;
		}
		palette.push_back(sum / float(box.indices.size()));
	}

	return palette;
}

int FindNearestLAB(const Vec3<float>& lab, const std::vector<Vec3<float>>& palette)
{
	int best = 0;
	float bestDist = FLT_MAX;

	for (int i = 0; i < (int)palette.size(); ++i) {
		Vec3<float> d = lab - palette[i];
		float dist = d.DotProduct(d);
		if (dist < bestDist) {
			bestDist = dist;
			best = i;
		}
	}
	return best;
}

Texture LABQuantizeImage(Texture& t, int numColours) {
	// May also want to pass in some form of fixed LAB, and the number of possible colours to quantize into
	Texture quantizedT;
	quantizedT.height = t.height;
	quantizedT.width = t.width;
	quantizedT.channelType = t.channelType;

	std::vector<LABPixel> labPixels;
	labPixels.reserve(t.width * t.height);

	for (int h = 0; h < t.height; ++h) {
		for (int w = 0; w < t.width; ++w) {
			// Convert sRGB -> rgb if we need 
			Vec3<float> rgb = t.texels[h * t.width + w].GetVectorizedRGB();
			//sRGBToLinear(rgb);
			Vec3<float> XYZ = LinearToXYZ(rgb);
			Vec3<float> LAB = XYZToLAB(XYZ);
			labPixels.push_back({LAB.x, LAB.y, LAB.z});
		}
	}

	auto palette = BuildLABPalette(labPixels, numColours);

	std::vector<Colour> quantizedPixels;
	quantizedPixels.reserve(labPixels.size());

	for (size_t i = 0; i < labPixels.size(); ++i) {
		Vec3<float> lab = { labPixels[i].L, labPixels[i].a, labPixels[i].b };
		int idx = FindNearestLAB(lab, palette);

		Vec3<float> rgb = XYZToLinear(LABToXYZ(palette[idx]));
		//Vec3<float> rgb = LinearTosRGB(XYZToLinear(LABToXYZ(palette[idx])));
		//Vec3<float> rgb = LinearTosRGB(XYZToLinear(LABToXYZ(lab)));

		quantizedPixels.push_back(Colour(rgb.x, rgb.y, rgb.z, idx));
	}

	quantizedT.texels = quantizedPixels;
	
	return quantizedT;
}

enum EdgeType {
	HORIZONTAL, // (when an edge goes down, it has a horizontal edge that connects the two pixels)
	VERTICAL // When a pixel moves to the right, it has a vertical edge that connects them.
};

struct Edge {
	Vec2<float> pixel0;
	Vec2<float> pixel1;
	uint32_t region0ID;
	uint32_t region1ID;
	EdgeType edgeType;
};

struct HalfEdge { // Emit 2 of these, one for each
	Vec2<uint32_t> from; // vertex not pixel 0,0 = top left corner vertex on grid.
	Vec2<uint32_t> to; // vertex not pixel
	uint32_t regionID;
};

GridDirection GetEdgeDirection(const HalfEdge& e) {
	if (e.to.x > e.from.x) { return GridDirection::RIGHT; }
	else if (e.to.x < e.from.x) { return GridDirection::LEFT; }
	else if (e.to.y > e.from.y) { return GridDirection::UP; }
	
	return GridDirection::DOWN;
}

std::vector<HalfEdge> FindRegionContours(Texture& t) {
//Texture FindRegionContours(Texture& t) {

	//std::vector<Edge> edges;
	//edges.reserve(t.width * t.height); // rough guess.

	//std::vector<Colour> outlines;
	//outlines.reserve(t.width * t.height);     

	std::vector<HalfEdge> edges;
	edges.reserve(t.width * t.height); // rough guess

	for (uint32_t h = 0; h < t.height; ++h) {
		for (uint32_t w = 0; w < t.width; ++w) { // To make life simpler here for now, let's assume we won't add edges to the borders of the texture, they are implicitly there when we go to make polygons.
			// right neighbour check
			uint32_t r = std::min<uint32_t>(w + 1, t.width - 1);
			uint32_t region0ID = static_cast<uint32_t>(t.texels[h * t.width + w].a); // remember we stuffed our alpha value with the colour id.
			uint32_t region1ID = static_cast<uint32_t>(t.texels[h * t.width + r].a); 

			//Colour c = Colour(0.f, 0.f, 0.f, 1.f);

			if (region0ID != region1ID) { 
				edges.push_back({ Vec2<uint32_t>(r, h), Vec2<uint32_t>(r, h + 1), region0ID}); // CCW for left pixel
				edges.push_back({ Vec2<uint32_t>(r, h + 1), Vec2<uint32_t>(r, h), region1ID }); // CCW for right region (via reversing direction)
				
				//edges.push_back({ Vec2<float>(w, h), Vec2<float>(r, h), region0ID, region1ID, EdgeType::VERTICAL });
				//c = Colour(1.f, 1.f, 1.f, 1.f);
			}

			// bottom neighbour check
			uint32_t b = std::min<uint32_t>(h + 1, t.height - 1);
			region1ID = static_cast<uint32_t>(t.texels[b * t.width + w].a);

			if (region0ID != region1ID) {
				edges.push_back({ Vec2<uint32_t>(w + 1, b), Vec2<uint32_t>(w, b), region0ID });// CCW for left pixel
				edges.push_back({ Vec2<uint32_t>(w, b), Vec2<uint32_t>(w + 1, b), region1ID }); // CCW for right region (via reversing direction)

				//edges.push_back({ Vec2<float>(w, h), Vec2<float>(w, b), region0ID, region1ID, EdgeType::HORIZONTAL});
				//c = Colour(1.f, 1.f, 1.f, 1.f);
			}

			//outlines.push_back(c);
		}
	}

	// Need to also add in edges for the border vertices here, special case of only 1 half-edge per edge. 4 lines.
	for (uint32_t h = 0; h < t.height; ++h) {
		// add in half-edges for left, and right walls
		uint32_t regionID = static_cast<uint32_t>(t.texels[h * t.width].a);
		edges.push_back({ Vec2<uint32_t>(0, h + 1), Vec2<uint32_t>(0, h), regionID });

		regionID = static_cast<uint32_t>(t.texels[h * t.width + (t.width - 1)].a);
		edges.push_back({ Vec2<uint32_t>(t.width, h), Vec2<uint32_t>(t.width, h + 1), regionID });
	}

	for (uint32_t w = 0; w < t.width; ++w) {
		// add in half edges for top, and bottom walls.
		uint32_t regionID = static_cast<uint32_t>(t.texels[w].a);
		edges.push_back({ Vec2<uint32_t>(w, 0), Vec2<uint32_t>(w + 1, 0), regionID });

		regionID = static_cast<uint32_t>(t.texels[(t.height - 1) * t.width + w].a);
		edges.push_back({ Vec2<uint32_t>(w + 1, t.height), Vec2<uint32_t>(w, t.height), regionID });
	}

	//Texture tNew = t;
	//tNew.texels = outlines;
	//return tNew;
	return edges;
}

std::unordered_map<uint64_t, std::vector<uint32_t>> BuildAdjacencyMap(std::vector<HalfEdge>& edges) {
	std::unordered_map<uint64_t, std::vector<uint32_t>> outgoingEdges;
	outgoingEdges.reserve(edges.size() * 2);

	for (uint32_t edgeIdx = 0; edgeIdx < edges.size(); ++edgeIdx) {
		const HalfEdge& e = edges[edgeIdx];
		uint64_t gridVertexKey = MakeEdgeKey(e.from.x, e.from.y);
		outgoingEdges[gridVertexKey].push_back(edgeIdx);
	}

	return outgoingEdges;
}

struct Circuit {
	std::vector<std::weak_ptr<Vec2<uint32_t>>> vertices;
	uint32_t regionID;

	Circuit(std::vector<std::weak_ptr<Vec2<uint32_t>>> vertices, uint32_t regionID) : vertices(vertices), regionID(regionID) {};
};

uint32_t ChooseNextEdge(const std::vector<HalfEdge>& edges, const std::vector<uint32_t>& edgeCandidates, uint32_t regionID, GridDirection& prevDir, const std::vector<bool>& usedEdge) {
	// remove candidates back to front that can't be a match
	std::vector<uint32_t> realCandidates;
	for (uint32_t edgeIndex : edgeCandidates) {
		if (usedEdge[edgeIndex]) {
			continue;
		}

		const HalfEdge& e = edges[edgeIndex];

		if (e.regionID != regionID) {
			continue;
		}

		realCandidates.push_back(edgeIndex);
	}
	
	for (GridDirection desiredDir : CCWPriority[prevDir]) {
		for (uint32_t edgeIndex : realCandidates) {
			if (GetEdgeDirection(edges[edgeIndex]) == desiredDir) {
				return edgeIndex;
			}
		}
	}

	throw std::runtime_error("Somehow choosing a next edge along a closed loop resulted in failure when attempting to chain polygons edges to polygons.");
}


std::vector<Circuit> ChainEdgesToPolygons(std::vector<HalfEdge>& edges, std::unordered_map<uint64_t, std::vector<uint32_t>>& outgoingEdges, std::unordered_map<uint64_t, std::shared_ptr<Vec2<uint32_t>>>& vertexManager) {
	// WE are going to use the convention:
	// Region interior is on the left, and the polygon is CCW.

	std::vector<bool> usedEdge(edges.size(), false);
	//std::unordered_map<uint64_t, std::shared_ptr<Vec2<uint32_t>>> vertices;
	std::vector<Circuit> polygons;

	for (uint32_t edgeIdx = 0; edgeIdx < edges.size(); ++edgeIdx) {
		if (usedEdge[edgeIdx]) { continue; }

		const HalfEdge& startEdge = edges[edgeIdx];
		const uint64_t startEdgeKey = MakeEdgeKey(startEdge.from.x, startEdge.from.y);
		const uint32_t regionID = startEdge.regionID;

		std::vector<std::weak_ptr<Vec2<uint32_t>>> currPolygonVerts;
		uint32_t currEdge = edgeIdx;
		GridDirection prevDir = GetEdgeDirection(edges[currEdge]);

		// Begin stepping through the edges until we close it.
		while (true) {
			usedEdge[currEdge] = true;

			const HalfEdge& e = edges[currEdge];

			uint64_t key = MakeEdgeKey(e.from.x, e.from.y);
			auto& v = vertexManager[key];
			if (!v) {
				v = std::make_shared<Vec2<uint32_t>>(Vec2<uint32_t>(e.from.x, e.from.y));
				vertexManager[key] = v;
			}

			currPolygonVerts.push_back(v);

			uint64_t nextEdgeKey = MakeEdgeKey(e.to.x, e.to.y);
			if (nextEdgeKey == startEdgeKey) {
				break;
			}

			std::vector<uint32_t> edgeCandidates = outgoingEdges[nextEdgeKey];

			currEdge = ChooseNextEdge(edges, edgeCandidates, regionID, prevDir, usedEdge);
			prevDir = GetEdgeDirection(edges[currEdge]);
		}


		polygons.push_back(Circuit(currPolygonVerts, regionID));
	}

	return polygons;
}



uint64_t PerpendicularDistanceSquared(const Vec2<uint32_t>& v, const Vec2<uint32_t>& start, const Vec2<uint32_t>& end) {
	int64_t dx = static_cast<int64_t>(end.x) - static_cast<int64_t>(start.x);
	int64_t dy = static_cast<int64_t>(end.y) - static_cast<int64_t>(start.y);

	if (dx == 0 && dy == 0) {
		// start == end, which is a degenerate case asn it means they are the same point.
		int64_t ddx = static_cast<int64_t>(v.x) - static_cast<int64_t>(start.x);
		int64_t ddy = static_cast<int64_t>(v.y) - static_cast<int64_t>(start.y);

		return (ddx * ddx) + (ddy * ddy);
	}

	//Vec2<int> diff = v - start;

	// cross product squared
	int64_t num = std::abs(dy * (static_cast<int64_t>(v.x) - static_cast<int64_t>(start.x)) - dx * (static_cast<int64_t>(v.y) - static_cast<int64_t>(start.y)));
	return static_cast<uint64_t>(num * num / (dx * dx + dy * dy));
}

// Ramer-Douglas-Peucker polygon simplification
void RDPSimplifyPolygon(const std::vector<std::shared_ptr<Vec2<uint32_t>>>& vertices, uint32_t start, uint32_t end, uint64_t epsilonSquared, std::vector<bool>& keep) {
	if (end <= start + 1) { return; }

	uint64_t maxDist = 0;
	uint32_t idx = start;

	for (uint32_t i = start + 1; i < end; ++i) {
		uint64_t d = PerpendicularDistanceSquared(*vertices[i], *vertices[start], *vertices[end]);
		if (d > maxDist) {
			idx = i;
			maxDist = d;
		}
	}

	if (maxDist > epsilonSquared) {
		keep[idx] = true;

		// recursive call to keep simplifying.
		RDPSimplifyPolygon(vertices, start, idx, epsilonSquared, keep);
		RDPSimplifyPolygon(vertices, idx, end, epsilonSquared, keep);
	}
}

std::vector<std::weak_ptr<Vec2<uint32_t>>> SimplifyCircuit(std::unordered_map<uint64_t, std::shared_ptr<Vec2<uint32_t>>>& vertexManager, std::vector<std::weak_ptr<Vec2<uint32_t>>>& vertices, uint32_t epsilon) {
	std::vector<std::shared_ptr<Vec2<uint32_t>>> lockedVertices;
	
	for (std::weak_ptr<Vec2<uint32_t>>& w : vertices) {
		// Check if any of our other simplifications have removed the reference to the vertex we are holding
		if (std::shared_ptr<Vec2<uint32_t>> s = w.lock()) {
			// If not, then it means this vertex wasn't simplified away, and we can use it for simplifying our new circuit.
			lockedVertices.push_back(s);
		}
	}

	std::vector<std::weak_ptr<Vec2<uint32_t>>> finalVertices;
	
	if (lockedVertices.size() <= 2) {
		for (auto& s : lockedVertices) {
			finalVertices.push_back(s);
		}

		return finalVertices;
	}

	uint32_t first = 0;
	uint32_t last = lockedVertices.size() - 1;

	std::vector<bool> keep(lockedVertices.size(), false);
	keep[first] = true;
	keep[last] = true;

	uint64_t epsilonSquared = static_cast<uint64_t>(epsilon) * epsilon;

	RDPSimplifyPolygon(lockedVertices, first, last, epsilonSquared, keep);

	std::vector<std::weak_ptr<Vec2<uint32_t>>> simplified;
	for (size_t i = 0; i < lockedVertices.size(); ++i) {
		if (keep[i]) {
			simplified.push_back(lockedVertices[i]);
			continue;
		}

		// If we reach here, the vertex was simplified, and as such, we should tell our master shared_ptr vertex manager, that it should remove it's reference so once this function exits, 
		// all weak ptrs will also be removed.
		vertexManager.erase(MakeEdgeKey(lockedVertices[i]->x, lockedVertices[i]->y));
	}
	return simplified;
}

std::vector<Circuit> SimplifyPolygons(std::unordered_map<uint64_t, std::shared_ptr<Vec2<uint32_t>>>& vertexManager, std::vector<Circuit> circuits, uint32_t epsilon = 1) {
	std::vector<Circuit> simplifiedCircuits;

	for (Circuit& c : circuits) {
		c.vertices = SimplifyCircuit(vertexManager, c.vertices, epsilon);
	}

	return simplifiedCircuits;
}

struct Poly {
	std::vector<std::shared_ptr<Vec2<uint32_t>>> outerVertices;
	std::vector<std::shared_ptr<Vec2<uint32_t>>> holeVertices;
	uint32_t regionID;
	float area;
};

float SignedArea(const std::vector<std::shared_ptr<Vec2<uint32_t>>>& circuit) {
	float a = 0.0f;

	for (size_t idx = 0, prevIdx = circuit.size() - 1; idx < circuit.size(); prevIdx = idx++) {
		a += static_cast<float>(circuit[prevIdx]->x + circuit[idx]->x) * (circuit[prevIdx]->y - circuit[idx]->y);
	}

	return a * 0.5f;
}

std::vector<Poly> EmbedPolyHoles(std::vector<Circuit> circuits, uint64_t totalImageArea) {
	std::vector<Poly> embeddedPolygons;
	std::vector<Poly> holes;

	// we want to make sure the hole is at least 4 pixels (or whatever that percentage becomes), which means the hole isn't just random noise/static.
	const float minimumHoleSize = std::max<float>(totalImageArea * 0.00005f, 4); // 0.005%

	for (Circuit& c : circuits) {
		// At this point we can turn the weak pointers into simply shared pointers, as the only movement we might do is move a hole position, no more deletion, so we should be safe.
		std::vector<std::shared_ptr<Vec2<uint32_t>>> lockedVertices;
		for (auto& w : c.vertices) {
			if (std::shared_ptr<Vec2<uint32_t>> s = w.lock()) {
				// If not, then it means this vertex wasn't simplified away, and we can use it for simplifying our new circuit.
				lockedVertices.push_back(s);
			}
		}

		float area = SignedArea(lockedVertices);

		if (area < 0) {
			// This must mean the circuit is a hole to another polygon, with this, we also want to remove if they are too small, as that simply is noise.
			if (std::abs(area) < minimumHoleSize) {
				continue;
			}

			Poly hole;
			hole.outerVertices = lockedVertices;
			hole.regionID = c.regionID;
			hole.area = area;
			holes.push_back(hole);
		}

		Poly p;
		p.outerVertices = lockedVertices;
		p.regionID = c.regionID;
		p.area = area;
		embeddedPolygons.push_back(p);
	}

	// Now that we know which circuits are holes within another, we can simply go through each, and add them into whichever polygon they reside.
	// We will make this work for holes within a polygon that is a hole of another polygon, but looping through all circuits, and choosing the one we match with, that contains the lowest total area.
	/*for (Poly& c : holes) {
		// check the smallest area polygon that contains it, as we could technically have holes within holes. (or technically speaking poly's inside of poly's inside of poly's but whatever)
		float largestArea = -1.f;
		uint32_t polyIdx;
		
		for (Poly& p : embeddedPolygons) {
			// Do the simple point in the poly for each polyogn to find the smallest area polygon we are inside,



		}

		// then if we found a home, we should also ensure we keep the hole inside of the polygon, as we simplified which could push the vertices out of it

	}*/

	return embeddedPolygons;
}

struct Tri {
	Vec2<float> v0, v1, v2;
	uint32_t regionID;
};


// 1 If the point is to the right of the segment, 0 if the point is to the left of the segment 
int PointDirectionFromLineSegment(Vec2<float> lineA, Vec2<float> lineB, Vec2<float> point) {
	float dir = (lineA - lineB).CrossProduct(lineA - point);
	return (dir < 0) ? 1 : 0;
}

bool DoLineSegmentsIntersect(Vec2<float> a1, Vec2<float> b1, Vec2<float> a2, Vec2<float> b2) {
	Vec2<float> a1b1 = b1 - a1;
	Vec2<float> lineB1B2 = b2 - b1;
	Vec2<float> lineB1A2 = a2 - b1;

	bool intersect = false;
	
	if (a1b1.CrossProduct(lineB1B2) * a1b1.CrossProduct(lineB1A2) >= 0) {
		// The two line segments don't pass the first check of intersection
		return false;
	}

	Vec2<float> a2b2 = b2 - a2;
	Vec2<float> lineB2B1 = b1 - b2;
	Vec2<float> lineB2A1 = a1 - b2;

	if (a2b2.CrossProduct(lineB2B1) * a2b2.CrossProduct(lineB2A1) >= 0) {
		// The two line segments don't pass the second check of intersection
		return false;
	}

	return true;
}

bool IsQuadrilateralConvex(Vec2<float> p1, Vec2<float> p2, Vec2<float> p3, Vec2<float> p4) {
	std::vector<Vec2<float>> edges;

	edges.push_back(p2 - p1);
	edges.push_back(p3 - p2);
	edges.push_back(p4 - p3);
	edges.push_back(p1 - p4);

	bool firstSignNegative;

	for (int i = 0; i < edges.size(); ++i) {
		int nextEdgeIdx = (i + 1) % edges.size();
		
		float cross = edges[i].CrossProduct(edges[nextEdgeIdx]);
		if (i == 0) {
			firstSignNegative = cross < 0;
			continue;
		}

		// Check to ensure that all the rest of the edges have the same sign, if so then convex
		bool isCurrEdgeNegative = cross < 0;

		if (isCurrEdgeNegative != firstSignNegative) {
			return false;
		}
	}

	return true;
}

std::vector<Tri> TriangulatePolygons(std::vector<Poly> polygons, Texture& t) {
	// we simply want to CDT all of our polygon vertices (perhaps with us also adding in a point towards the center of each polygon for additional detail if it doesn't work all that well
	// finally, when we triangulate, we will then divide the final positions by the image height and width to get proper UV coords for our quad
	// we are also constraining CDT to have constraint edges along the borders of the quad (0,0),  (1,0), (1,1), (0, 1)... we want to include proper edges for if there are certian points on the
	// border as well that aren't corners, so we need to ensure we do proper constraint building afterwards.

	/*
	Since we have constraint edges, not only will we create constraint edges around the quad, but will also add them to the circuit of the polygon itself, if there are duplicates, 
	I don't believe that should technically be a problem. We also again may add a vertex in the middle of each polygon just to add extra detail if necessary
	*/

	// First, let's begin by unboxing our vertices in our polygons, move them into the correcr 0->1 (relative to only 1 either width or height whichever is larger) float range, and 
	// keep track of the constraint edges we want (can also add in corner points at this point if need be, as well as finish up adding border constraints.
	std::vector<Vec2<float>> allPoints;
	std::vector<Vec2<uint64_t>> allConstraintEdges;
	//std::vector<Vec2<int>> cornerPointsToAdd = { {0, 0}, {0, t.height}, {t.width, 0 }, {t.width, t.height} }; // If we find one of these when unboxing our points, we can remove it as it is there
	//std::unordered_set<uint64_t> isVertexAdded; // Since we share pointers of vertices, we don't want duplicate points in our triangulation as it will break it;
	std::unordered_map<uint64_t, std::vector<uint64_t>> isConstraintAdded; // Similarly we want to keep track if we already added the constraint here, we keep track of the to->from vertex, and make duplicate
	// 2 entries for both ways to make it simple for us as the other polygon will have an edge going the opposite direction (although not certain when closing borders up
	std::unordered_map<uint64_t, uint64_t> vertexIdx;

	float largestScale = static_cast<float>(std::max<int>(t.width, t.height)); // This will be used to divide all points x, and y, and then for final UV's we can multiply by it, and then divide
	// by it's respective scale (width : x, or height : y).
	float invLargestScale = 1.f / largestScale;

	// I can also add in a vertex in the center of each polygon as I loop through this.
	for (Poly& p : polygons) {
		// We need our p.area from shoelace theorem to calculate centroid coordinates.
		// To find the centroid coords, average the positions of the edges weighted by the area they enclose:
		// Cx = (1/(6*area)) * Sum of edges: (xi + xi+1) * (xi*yi+1 - xi+1*yi) 
		// Cy = (1/(6*area)) * Sum of edges: (yi + yi+1) * (xi*yi+1 - xi+1*yi)
		// The +1 simply denotes the other vertex on the edge.
		
		int polyStartIdx = allPoints.size(); // useful for us to grab the correct index when making an edge between the start and the end.
		float centroidX = 0.f;
		float centroidY = 0.f;

		for (int i = 0; i < p.outerVertices.size(); ++i) {
			auto& v = p.outerVertices[i];
			uint64_t vKey = MakeEdgeKey(v->x, v->y);

			// If the vertex is not already added to the "point cloud" array, then add it in
			if (vertexIdx.find(vKey) == vertexIdx.end()) {
				allPoints.push_back(Vec2<float>(std::min<float>(v->x * invLargestScale, 1.f), std::min<float>(v->y * invLargestScale, 1.f)));
				vertexIdx[vKey] = allPoints.size() - 1; // as we just pushed it in.
			}

			int vertIdx = vertexIdx[vKey];

			auto& vNext = p.outerVertices[(i + 1) % p.outerVertices.size()];
			uint64_t vNextKey = MakeEdgeKey(vNext->x, vNext->y);
			
			int nextVertIdx;

			// First see if the next vertex is already added:
			if (vertexIdx.find(vNextKey) != vertexIdx.end()) {
				nextVertIdx = vertexIdx[vNextKey];
			}
			else {
				// we can safely assume if it's the start idx, it would already have been added, as that's processed before the end vertex.
				nextVertIdx = allPoints.size();
			}
			

			// Even though there is a duplicated edge, we still need add it to our centroid calculation, as it is a per-polygon thing.
			// CENTROID ADDITION STUFF -> SKIP FOR NOW.


			// Check if we already have added the constraint edge.
			if (isConstraintAdded.find(vKey) != isConstraintAdded.end()) {
				bool edgeExists = false;
				// Check if the specific other vertex for our edge is within the other 
				for (uint64_t& edgeKey : isConstraintAdded[vKey]) {
					if (edgeKey == vNextKey) {
						edgeExists = true;
						break;
					}
				}

				if (edgeExists) {
					continue;
				}
			}

			// at this point, the edge is not already, so we can add it.
			isConstraintAdded[vKey].push_back(vNextKey);
			isConstraintAdded[vNextKey].push_back(vKey);
			allConstraintEdges.push_back(Vec2<uint64_t>(vKey, vNextKey));
		}

		// Finally check if the centroid position is already in the list, we do this by first checking if it's decimal has enough zeros at the start, if not then we should never have added it.
		// else wise, we then need to check if a vertex at that roughly two int positions exists, and only then do we cull it. (Should generally be safe, but we should keep track of centroids, as
		// they can technically converge on each other (there is that worry of a hole which the centroid is within, and then the hole has a centroid as well on same position)?
		// But let's not worry unless there is a major issue.

	}

	// Finally, add in the missing corner points, and add in the necessary edges to the border
	if (vertexIdx.find(MakeEdgeKey(0, 0)) == vertexIdx.end()) {
		vertexIdx[MakeEdgeKey(0, 0)] = allPoints.size();
		allPoints.push_back({ 0.f, 0.f });
	}

	if (vertexIdx.find(MakeEdgeKey(0, t.height)) == vertexIdx.end()) {
		vertexIdx[MakeEdgeKey(0, t.height)] = allPoints.size();
		allPoints.push_back({ 0.f, std::min<float>(t.height * invLargestScale, 1.f) });
	}

	if (vertexIdx.find(MakeEdgeKey(t.width, 0)) == vertexIdx.end()) {
		vertexIdx[MakeEdgeKey(t.width, 0)] = allPoints.size();
		allPoints.push_back({ std::min<float>(t.width * invLargestScale, 1.f), 0.f });
	}

	if (vertexIdx.find(MakeEdgeKey(t.width, t.height)) == vertexIdx.end()) {
		vertexIdx[MakeEdgeKey(t.width, t.height)] = allPoints.size();
		allPoints.push_back({ std::min<float>(t.width * invLargestScale, 1.f), std::min<float>(t.height * invLargestScale, 1.f) });
	}

	Vec2<uint32_t> startPoint = { 0, 0 };
	uint64_t startKey;
	//bool haveAtLeastOneEdge = false;

	// Constrain All Quad Left Edges:
	for (uint32_t h = 1; h <= t.height; ++h) {
		uint64_t newVertKey = MakeEdgeKey(0, h);
		if (vertexIdx.find(newVertKey) == vertexIdx.end()) {
			continue;
		}

		startKey = MakeEdgeKey(startPoint.x, startPoint.y);

		// at this point we found a vertex on the border, double check there is an edge between the start point and this new vertex, if not then create one.
		if (isConstraintAdded.find(newVertKey) != isConstraintAdded.end()) {
			bool edgeExists = false;
			// Check if the specific other vertex for our edge is within the other 
			for (uint64_t& edgeKey : isConstraintAdded[newVertKey]) {
				if (edgeKey == startKey) {
					edgeExists = true;
					break;
				}
			}

			if (edgeExists) {
				startPoint = Vec2<uint32_t>(0, h);
				//haveAtLeastOneEdge = true;
				continue;
			}
		}

		// If we reach here, we should create an edge between these two points and continue.
		allConstraintEdges.push_back(Vec2<uint64_t>(startKey, newVertKey));
		isConstraintAdded[startKey].push_back(newVertKey);
		isConstraintAdded[newVertKey].push_back(startKey);

		startPoint = { 0, h };
		//haveAtLeastOneEdge = true;
	}

	
	startPoint = Vec2<uint32_t>(t.width, 0);

	// Constrain All Quad Right Edges:
	for (uint32_t h = 1; h <= t.height; ++h) {
		uint64_t newVertKey = MakeEdgeKey(t.width, h);
		if (vertexIdx.find(newVertKey) == vertexIdx.end()) {
			continue;
		}

		startKey = MakeEdgeKey(startPoint.x, startPoint.y);

		// at this point we found a vertex on the border, double check there is an edge between the start point and this new vertex, if not then create one.
		if (isConstraintAdded.find(newVertKey) != isConstraintAdded.end()) {
			bool edgeExists = false;
			// Check if the specific other vertex for our edge is within the other 
			for (uint64_t& edgeKey : isConstraintAdded[newVertKey]) {
				if (edgeKey == startKey) {
					edgeExists = true;
					break;
				}
			}

			if (edgeExists) {
				startPoint = Vec2<uint32_t>(t.width, h);
				continue;
			}
		}

		// If we reach here, we should create an edge between these two points and continue.
		allConstraintEdges.push_back(Vec2<uint64_t>(startKey, newVertKey));
		isConstraintAdded[startKey].push_back(newVertKey);
		isConstraintAdded[newVertKey].push_back(startKey);

		startPoint = { static_cast<uint32_t>(t.width), h };
	}

	startPoint = Vec2<uint32_t>(0, 0);
	// Constrain All Quad Top Edges:
	for (uint32_t w = 1; w <= t.width; ++w) {
		uint64_t newVertKey = MakeEdgeKey(w, 0);
		if (vertexIdx.find(newVertKey) == vertexIdx.end()) {
			continue;
		}

		startKey = MakeEdgeKey(startPoint.x, startPoint.y);

		// at this point we found a vertex on the border, double check there is an edge between the start point and this new vertex, if not then create one.
		if (isConstraintAdded.find(newVertKey) != isConstraintAdded.end()) {
			bool edgeExists = false;
			// Check if the specific other vertex for our edge is within the other 
			for (uint64_t& edgeKey : isConstraintAdded[newVertKey]) {
				if (edgeKey == startKey) {
					edgeExists = true;
					break;
				}
			}

			if (edgeExists) {
				startPoint = Vec2<uint32_t>(w, 0);
				continue;
			}
		}

		// If we reach here, we should create an edge between these two points and continue.
		allConstraintEdges.push_back(Vec2<uint64_t>(startKey, newVertKey));
		isConstraintAdded[startKey].push_back(newVertKey);
		isConstraintAdded[newVertKey].push_back(startKey);

		startPoint = { w, 0 };
	}

	startPoint = Vec2<uint32_t>(0, t.height);
	// Constrain All Quad Bottom Edges:
	for (uint32_t w = 1; w <= t.width; ++w) {
		uint64_t newVertKey = MakeEdgeKey(w, t.height);
		if (vertexIdx.find(newVertKey) == vertexIdx.end()) {
			continue;
		}

		startKey = MakeEdgeKey(startPoint.x, startPoint.y);

		// at this point we found a vertex on the border, double check there is an edge between the start point and this new vertex, if not then create one.
		if (isConstraintAdded.find(newVertKey) != isConstraintAdded.end()) {
			bool edgeExists = false;
			// Check if the specific other vertex for our edge is within the other 
			for (uint64_t& edgeKey : isConstraintAdded[newVertKey]) {
				if (edgeKey == startKey) {
					edgeExists = true;
					break;
				}
			}

			if (edgeExists) {
				startPoint = Vec2<uint32_t>(w, t.height);
				continue;
			}
		}

		// If we reach here, we should create an edge between these two points and continue.
		allConstraintEdges.push_back(Vec2<uint64_t>(startKey, newVertKey));
		isConstraintAdded[startKey].push_back(newVertKey);
		isConstraintAdded[newVertKey].push_back(startKey);

		startPoint = { w, static_cast<uint32_t>(t.height) };
	}

	// Regular Delaunay Triangulation
		// Have to keep track of my constraint edges, and add in the proper border constraints to fill in what's missing as necessary for usage in the next part.

	/*
	Make big triangle that contains point set (really big)
	Add points 1 by 1,
		- What triangle is it in?
		- break that triangle into 3 connected to your point
		- Swap edges to make sure magic delaunay condition isn't violated (may have a lot of outward propagation)
	- At the end, kill anything to do with the big triangle.

	---
	For every triangle you are adding, ensure the D is the first vertex in your structure for adjacencies and stuff. So if we have ABC, and insert D, it should be DAB, DCA, DBC
	Part A: Scale point cloud to 0,0 to 1,1 (so we should automatically turn our points into scaled floats, kind of like we expected.
		Can still easily do the per-vertex check, and the edge check, due to use iterating through our points)
	0-1, and other 0->8 so we divide both by the same largest size one I guess? a.k.a if our image height and width are different then there could be some annoyances, but not too much,
		as we can simply re multiply it out by what we divide and then divide again by the correct dimension?
	Don't want to handle elipses
	Step B: Again opptional, but can sort points by proximity, divide domain into bins and reorder points in a continuous bin sequence (could simply be my polygons themselves?)
	denote points via trace through that grid, not sure if we actually want  to do that, but if it's really slow, then sure lol

	Step C: Make big triangle (-100, -100,), (0, 100), and (100, -100) for the triangle if we mapped to 0->1 all coords
	Step D: Loop through points and find containing triangle, same old same old. Do this via computing normals of each edge of the triangle and then compute the dot product of the point with each edge normal, if all of those dot products are the same sign, then P is contained wihin that triangle
	It also considers it inside if it's on the boundary
	So technically also contained if 2 with same sign and 1 with a 0 value
	Step D2: Triangle search algorithm.
	Should constantly move in direction of point of interest via first taking the dot product of all of the edges outward normal of the triangle, and if it is positive, go in that direction. ANd keep going until you reach the point p
	Step E:
	Check magic Delaunay condition
	Tells you if you have to swap diagonals or not in triangles.
	Step F:
	Where condition fails, swap diagonals.

	Data structure:
	- Array for which vertices are part of which triangle,
	- Array for which trianlges are adjacent to triangle you are talking about.

	Keep track of vertex ordering, and triangle adjacency, and update adjacencies of those grey triangles too
	Step G: Continue with the stack of triangles you are adding (use a stack), and see if you broke Delaunayness on the adjacent triangles, if yes swap diags and repreat F + G until stack is empty
	Step H: Delete Big triangle
	Step I: Map back to original domain
	*/

	// Let's attempt it here:

	// Assume our big triangle will be (-100, -100,), (0, 100), and (100, -100) ( should I swap for CCW?)
	std::vector<Vec2<float>> triangleVerts;
	//s


	// Now at this point we handle Constrained Delaunay Triangulation
	/*
	
	*/

	return {};
}


Texture TextureLoader::GenerateTextureTopology(std::string& texturePath) {
	// First read in the general TGA texture to get the pixel data
	Texture t = ProcessTGA(texturePath);
	// set some value for the texture

	// Now apply the Anisotropic Kuwahara Filter on the texture to try to denoise it (and keep edge lines which are important)
	// Anisotropic Kuwahara filter (with polynomial weights) [TODO]
	Texture kuwaharaT = ApplyKuwaharaFilter(t);
	

	//  Do LAB conversion, Quantize it with our set of colours (16) [WE SHOULD HAVE A DIRECT INITIALIZED COLOUR LIST PALETTE FOR OURSELVES), and tile the pixels for SIMD
	Texture quantizedT = LABQuantizeImage(kuwaharaT, 2);

	// Our alpha value for our colour of the quantized texture ppoints to a specific label per pixel for which colour index it is, that way we can find more of the same
			// we use this to find and create a pixel-exact segmentation graph, for our region boundaries (topology edges)

	// WE will process each pixel for it's id, and check each up/down, left/right pixel to see if we are the same colour id, if not then it must be a part of a border segment
	// that we keep track of with an edge struct. We can then later use that to combine and create the 
	std::vector<HalfEdge> edges = FindRegionContours(quantizedT);
	std::unordered_map<uint64_t, std::vector<uint32_t>> outgoingEdges = BuildAdjacencyMap(edges);
	std::unordered_map<uint64_t, std::shared_ptr<Vec2<uint32_t>>> vertexManager;
	std::vector<Circuit> circuitPolygons = ChainEdgesToPolygons(edges, outgoingEdges, vertexManager);
	std::vector<Circuit> simplifiedCircuits = SimplifyPolygons(vertexManager, circuitPolygons, 1);
	std::vector<Poly> polygons = EmbedPolyHoles(simplifiedCircuits, quantizedT.height * quantizedT.width);
	std::vector<Tri> triangles = TriangulatePolygons(polygons, quantizedT);
	int abc = 123;


			// Then we will turn our boundaries into continuous contours via contour tracing (Moore-neighbour tracing) or (Suzuki Abe)
				// OLD!: Process the quantized tiles with SIMD for if they all are the same quantized colour, if so then keep going until we find edge points (where it stops), same with the other colours.
				// OLD!: We then merge the edge points into the polygon with the largest area


	// Apply Ramer Douglas Peucker simplification, ^ may have to be before the merging of edge points


	// We now convert our contours to constraints: We have Quad boundary, and internal constraint edges (contours), we have it as a planar straight line graph (PSLG)

	// Finally triangulate using constrained triangulation:
	// Constrained Delaunay Triangulation

	// We can also assign region id's to triangles, allowing for region shading, flat colour, edge strokes, and NPR effects.


	// In the actual pipeline probably specifically for quads, spheres, cubes, and cylinders (generated instead of OBJ loaded) we will generate vertices based on interpolated distance from UV on
	// View position and, then we can properly apply and sample in our textures (again for all of our samples, since they are vertices they share them with other triangles, so to get the most
	// out of our number of vertices, we will sample ~15% towards the center of the triangle relative to the vertex we are sampling to ensure each triangle has different colours, and doesn't blend
	// between two distinc colours (edges), and we can also apply displacement and generally be fine ( don't forget to re-calculate normals), and to apply vertex lighting.


	// May also want to look into triplanar mapping for my scenarios.

	//return kuwaharaT;
	return quantizedT;
	//return edges;
}