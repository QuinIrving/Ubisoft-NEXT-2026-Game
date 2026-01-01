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
#include "Camera.h"

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

    void ResizeWindowProjection(float width, float height);

    Pipeline(Pipeline& other) = delete;
    void operator=(const Pipeline&) = delete;

    Camera camera;

protected:
    float m_fovY = PI / 2;
    const float m_yScale = tanf(m_fovY / 2);
    float m_aspectRatio;
    const float n{ 0.1 };
    const float f{ 1000 };

    Mat4<float> CreateProjectionMatrix() {
        m_aspectRatio = static_cast<float>(APP_VIRTUAL_WIDTH) / static_cast<float>(APP_VIRTUAL_HEIGHT);

        // Row-major order.

        return Mat4<float>(
            {
            1 / (m_yScale * m_aspectRatio),   0,		    0,						0,
            0,                          1 / m_yScale,   0,						0,
            0,							0,		    -(f / (f - n)),	    -1,
            0,							0,		    -((f * n) / (f - n)),   0
            });
    }
    
    Pipeline() = default;
    ~Pipeline() {};
    

    Mat4<float> m_projectionMatrix = CreateProjectionMatrix();

    // some camera maybe here or a way to bind it for the pipeline to use.
};
