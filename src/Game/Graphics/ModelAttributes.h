#pragma once
#include <optional>
#include <memory>
#include "Math/Mat4.h"
#include "Math/Vec4.h"
#include "Texture.h"
#include "Objects/Mesh.h"

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
	// half lambert? Could be cool
};

/*
struct Material {
	
	Vec3<float> Ka; // Ambient colour, colour when lit by ambient light only
	Vec3<float> Kd; // Diffuse colour, base colour of material, like albedo map
	Vec3<float> Ks; // Specular colour, colour of specular highlights
	Vec3<float> Tf; // Transmission filter - Colour filter applied to light passing through this transparent object
	unsigned int illum; // Determines lighting equation to use ~ 0: colour only & no lighting, 1: Diffuse only, 2: Diffuse & Specular, 3-10 Various other types (reflection, refraction, etc)
	float d; // Disolve/Opacity 0.0 (transparent) -> 1.0 (opaque)
	float Ns; // Specular exponent - Shininess value, higher means sharper highlights, lower mean high roughness.
	int sharpness; // Reflection sharpness
	float Ni; // Optical density (Index of Refraction)
	
	std::shared_ptr<Texture> map_Ka; // Ambient texture, either same as albedo or not used
	std::shared_ptr<Texture> map_Kd; // Diffuse texture - Albedo/base colour map
	std::shared_ptr<Texture> map_Ks; // Specular texture, can inform metallic map (metal = white, non-metal = black)
	std::shared_ptr<Texture> map_Ns; // Specular exponent/shininess map, Roughness map but inverted, Dark areas in the map = rough, bright = smooth
	std::shared_ptr<Texture> map_d; // Opacity/Alpha map. White = opaque, black = transparent
	std::shared_ptr<Texture> disp; // Displacement/height map
	std::shared_ptr<Texture> decal; // Decal stencil/overlay texture
	std::shared_ptr<Texture> bump; // Normal map or bump map
	std::shared_ptr<Texture> refl; // reflection map, environment cubemap for reflections

	/*
	Illum models:

	0		Color on and Ambient off
	1		Color on and Ambient on
	2		Highlight on
	3		Reflection on and Ray trace on
	4		Transparency: Glass on
				Reflection: Ray trace on
	5		Reflection: Fresnel on and Ray trace on
	6		Transparency: Refraction on
				Reflection: Fresnel off and Ray trace on
	7		Transparency: Refraction on
				Reflection: Fresnel on and Ray trace on
	8		Reflection on and Ray trace off
	9		Transparency: Glass on
				Reflection: Ray trace off
	10		Casts shadows onto invisible surfaces
	*/
	
	// on texture maps there are parameters:
	/*
	* -s (scale): UV scale multiplier (u, v, w)
	* -o (offset): UV offset (u, v, w)
	* -mm (mipmap) base and gain values for mipmap filtering
	* -bm (bump multiplier) Strength of bump map effect
	* 
	* /
};*/


struct ModelAttributes {
	//static uint32_t currentNumObjects;

	uint32_t uniqueObjectIndex;
	Mat4<float> modelMatrix;
	std::shared_ptr<Material> material;
	//std::vector<Mesh>& meshes = {};
	//std::shared_ptr<Material> material; // Should setup our textures, and our materials on loadup, then each model can point to one
	//Material material; // for simplicity at the start I'll do this.

	 // This is so for each object created, it should be unique

	//ModelAttributes() { uniqueObjectIndex = currentNumObjects; currentNumObjects++; }
};