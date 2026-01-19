#pragma once
#include <vector>
#include <Graphics/Vertex.h>
#include <Graphics/ModelAttributes.h>
#include "QuadMesh.h"
#include "Model.h"
#include <Loaders/TextureLoader.h>

class Quad {
public:
	Quad(float width = 1.f, float height = 1.f, float size = 1.f, Colour colour = {1.f, 1.f, 1.f, 1.f}); // Perhaps colour should be something in our material idea

	// Want an ECS so need just making this as a placeholder for now to test my work until refactor time again
	void Rotate(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Translate(float x, float y, float z);

	//const ModelAttributes& GetModelAttributes();
	Mat4<float> GetModelMatrix() const;
	std::vector<uint32_t>& GetVertexIds();
	std::vector<Vertex>& GetVertices();

	Vec3<float> GetTranslation() { return m_position; }
	//Vec3<float> GetRotation() { return m_rotation; }
	Vec3<float> GetScale() { return m_scale; }
	Vec3<float> GetLocalHalfExtent() { return Vec3<float>(m_width / 2.f, m_height / 2.f, 0.005f); }
	Mat4<float> GetRotationMatrix() { return m_delta.GetRotationMatrix(); }
	Colour GetColour() { return col; }

	Material material;
	//ModelAttributes m_attributes;

	Mesh GetMesh();
	Mesh GetMesh(std::vector<TextureLoader::UVTri> uvTriangles);
	ModelEdge GetAdjacencyTable();

private:
	Vec3<float> m_position{ 0., 0., 0. };
	//Vec3<float> m_rotation{ 0., 0., 0. };
	Vec3<float> m_scale{ .5, .5, .5 };
	Quaternion m_delta;
	QuadMesh m_mesh;
	float m_width;
	float m_height;
	Colour col;
};