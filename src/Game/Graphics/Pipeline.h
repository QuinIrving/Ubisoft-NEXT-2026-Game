#pragma once
#include <vector>
#include "Vertex.h"
#include "ModelAttributes.h"

/*
    Singleton (thread safe) Pipeline class for rendering.
    Handles vertices per mesh -> vertex shader -> clipping -> tessellation -> texture mapping & lighting -> call rasterization
*/
class Pipeline {
public:
    static Pipeline& GetInstance();

    // business logic -> probably some way to bind vertex shader, not sure if needed though.
    void Render(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const ModelAttributes& modelAttributes);
    

    //VertexOut VertexShader(const Vertex& vin, const ModelAttributes& MA, const Mat4<float>& V, const Mat4<float>& P);
    // VertexProcessor
    // TessellationShader -> Quantize tessellation levels by distance bands, snap tessellation factor to pows of 2, cache subdivided meshes per LOD range
    // DisplacementMapper
    // VertexShading & Lighting (Fragment shader)
        // Normal mapping
        // PBR lighting
    // Projection and Clipping

    Pipeline(Pipeline& other) = delete;
    void operator=(const Pipeline&) = delete;

protected:
    constexpr Pipeline() = default;
    ~Pipeline() {};
};
