#pragma once
#include <vector>
#include "Vertex.h"
#include "VertexOut.h"
#include "VertexPostClip.h"
#include "ViewVertex.h"
#include "ProjectionVertex.h"
#include "ScreenSpaceVertex.h"
#include "ModelAttributes.h"
#include "Math/Mat4.h"

/*
    Singleton (thread safe) Pipeline class for rendering.
    Handles vertices per mesh -> vertex shader -> clipping -> tessellation -> texture mapping & lighting -> call rasterization
*/
class Pipeline {
public:
    static Pipeline& GetInstance();

    // business logic -> probably some way to bind vertex shader, not sure if needed though.
    void Render(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const ModelAttributes& modelAttributes) const;
    
    ViewVertex ProcessVertex(const Vertex& v, const Mat4<float> M, const Mat4<float> V) const;
    ProjectionVertex ProjectVertex(const ViewVertex& v) const;
    ScreenSpaceVertex HomogenizeAndViewportMap(const ProjectionVertex& v) const;
    void SubmitTriangle(const ScreenSpaceVertex& v1, const ScreenSpaceVertex& v2, const ScreenSpaceVertex& v3, bool isWireframe) const;
    
    
    //VertexOut VertexShader(const Vertex& v, const ModelAttributes& MA, const Mat4<float>& V, const Mat4<float>& P);

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

    static const Mat4<float> CreateProjectionMatrix() {
        float fovY = PI / 2; // 90 degrees in radians.
        float yScale = tanf(fovY / 2);//1.0f / tanf(fovY / 2);
        float aspectRatio = static_cast<float>(APP_VIRTUAL_WIDTH) / static_cast<float>(APP_VIRTUAL_HEIGHT);

        float n{ 1 }; // near
        float f{ 1000 }; // far. 

        // Row-major order.
        
        return Mat4<float>(
            {
            1/(yScale * aspectRatio),   0,		    0,						0,
            0,                          1/yScale,   0,						0,
            0,							0,		    -(f / (f - n)),	    -1,
            0,							0,		    -((f * n) / (f - n)),   0
            });
        /*return Mat4<float>(
            {
            yScale / aspectRatio,   0,		    0,						0,
            0,                          yScale,   0,						0,
            0,							0,		    (f) / (f - n),	    -1,
            0,							0,		    (f * n) / (f - n),   0
            });*/
    }

    static inline const Mat4<float> m_projectionMatrix = CreateProjectionMatrix();

    // some camera maybe here or a way to bind it for the pipeline to use.
};
