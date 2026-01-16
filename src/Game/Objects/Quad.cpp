#include "Quad.h"
#include "Tessellation/TriangleNode.h"

Quad::Quad(float width, float height, float size, Vec4<float> colour) {
	Scale(size, size, size);
	m_mesh = QuadMesh(width, height, colour);
}

void Quad::Rotate(float x, float y, float z) {
	//m_rotation.x = std::fmodf(m_rotation.x + x, 360.f);
	//m_rotation.y = std::fmodf(m_rotation.y + y, 360.f);
	//m_rotation.z = std::fmodf(m_rotation.z + z, 360.f);
	Quaternion deltaRotation = Quaternion(x, y, z);

	m_delta *= deltaRotation;
	m_delta.Normalize();
}

void Quad::Scale(float x, float y, float z) {
	m_scale.x *= x;
	m_scale.y *= y;
	m_scale.z *= z;
}

void Quad::Translate(float x, float y, float z) {
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

Mat4<float> Quad::GetModelMatrix() const {
	return Mat4<float>::Scale(m_scale) * m_delta.GetRotationMatrix() * Mat4<float>::Translate(m_position);
}

std::vector<uint32_t>& Quad::GetVertexIds() {
	return m_mesh.GetVertexIds();
}

std::vector<Vertex>& Quad::GetVertices() {
	return m_mesh.GetVertices();
}

Mesh Quad::GetMesh() {
	Mesh m;
	m.geometry = std::make_shared<MeshGeometry>(MeshGeometry(m_mesh.GetVertices()));
	m.material = material;
	
	return m;
}

namespace {
	Vertex GetVertexFromUV(Vec2<float> uv, Vec2<float>& v00, Vec2<float>& v10, Vec2<float>& v01, Vec2<float>& v11) {
		Vec2<float> bottom = v00 + (v10 - v00) * uv.x;
		Vec2<float> top = v01 + (v11 - v01) * uv.x;

		Vec2<float> position = bottom + (top - bottom) * uv.y;
		Vertex v = Vertex(Vec3<float>(position.x, position.y, 0), { 1.f, 1.f, 1.f, 1.f }, { 0.f, 0.f, 1.f }, uv);
		v.SetMaterialIndex(0);
		return v;
	}
}

Mesh Quad::GetMesh(std::vector<TextureLoader::UVTri> uvTriangles) {
	Mesh m;
	
	std::vector<Vertex> mVs = m_mesh.GetVertices();

	for (TextureLoader::UVTri t : uvTriangles) {
		// interpolate the world position and create the geometry as necessary.
		Vec2<float> v0 = t.v0;
		Vec2<float> v1 = t.v1;
		Vec2<float> v2 = t.v2;

		mVs.push_back(GetVertexFromUV(v0, Vec2<float>(mVs[1].GetPosition().x, mVs[1].GetPosition().y), Vec2<float>(mVs[2].GetPosition().x, mVs[2].GetPosition().y),
			Vec2<float>(mVs[0].GetPosition().x, mVs[0].GetPosition().y), Vec2<float>(mVs[4].GetPosition().x, mVs[4].GetPosition().y)));

		mVs.push_back(GetVertexFromUV(v1, Vec2<float>(mVs[1].GetPosition().x, mVs[1].GetPosition().y), Vec2<float>(mVs[2].GetPosition().x, mVs[2].GetPosition().y),
			Vec2<float>(mVs[0].GetPosition().x, mVs[0].GetPosition().y), Vec2<float>(mVs[4].GetPosition().x, mVs[4].GetPosition().y)));

		mVs.push_back(GetVertexFromUV(v2, Vec2<float>(mVs[1].GetPosition().x, mVs[1].GetPosition().y), Vec2<float>(mVs[2].GetPosition().x, mVs[2].GetPosition().y),
			Vec2<float>(mVs[0].GetPosition().x, mVs[0].GetPosition().y), Vec2<float>(mVs[4].GetPosition().x, mVs[4].GetPosition().y)));
	}

	m.geometry = std::make_shared<MeshGeometry>(MeshGeometry(mVs));
	m.material = material;

	return m;
}

ModelEdge Quad::GetAdjacencyTable() {
	ModelEdge me;

	auto key = MakeEdgeKey(0, 1);
	me.adjacencyTable[key].push_back(0);


	key = MakeEdgeKey(1, 2);
	me.adjacencyTable[key].push_back(0);
	me.adjacencyTable[key].push_back(1);

	key = MakeEdgeKey(0, 2);
	me.adjacencyTable[key].push_back(0);


	key = MakeEdgeKey(1, 3);
	me.adjacencyTable[key].push_back(1);

	key = MakeEdgeKey(2, 3);
	me.adjacencyTable[key].push_back(1);
	return me;
}