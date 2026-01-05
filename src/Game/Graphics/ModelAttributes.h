#pragma once
#include <optional>
#include "Math/Mat4.h"
#include "Math/Vec4.h"
#include "Texture.h"

enum class TextureSamplingMode {
	Nearest,
	Bilinear,
	// Trilinear?
};

enum class LightingModel {
	None,
	Flat,
	Gouraud,
	PBR,
	// half lambert?
};

struct Material {
	Vec4<float> baseColour{ 1.f, 1.f, 1.f, 1.f };
	// albedoTexture is also used for regular texture mapping
	std::optional<const Texture*> albedoTexture; // if we aren't doing any sharedpointer and all of that with the texture manager, else we can do a weak_ptr.

	TextureSamplingMode samplingMode = TextureSamplingMode::Nearest;
	LightingModel lightingModel = LightingModel::None;

	// BlinnPhong param
	float shininess = 32.f; // specular exponent

	// PBR params
	float metallic = 0.f; // 0<->1, where 1 is 100% metallic, and 0 is non-metal.
	float roughness = 0.5f; // 0<->1, where 0 is smooth, and 1 is rough.
	Vec3<float> emissiveColour{ 0.f, 0.f, 0.f };
	std::optional<const Texture*> metallicRoughnessMap; // Combined, may need to add in a separated version if needed.
	std::optional<const Texture*> normalMap;
	std::optional<const Texture*> emissiveMap;
	std::optional<const Texture*> ambientOcclusionMap;
};

struct ModelAttributes {
	static uint32_t currentNumObjects;

	Mat4<float> modelMatrix;
	//std::shared_ptr<Material> material; // Should setup our textures, and our materials on loadup, then each model can point to one
	Material material; // for simplicity at the start I'll do this.

	uint32_t uniqueObjectIndex; // This is so for each object created, it should be unique

	ModelAttributes() { uniqueObjectIndex = currentNumObjects; currentNumObjects++; }
};