#include <app.h>
#include "RenderPipeline.h"
#include "Vertex.h"
#include "VertexOut.h"
#include "VertexPostClip.h"
#include "ViewVertex.h"
#include "ProjectionVertex.h"
#include "ScreenSpaceVertex.h"
#include "Triangle.h"
#include <algorithm>

RenderPipeline& RenderPipeline::GetInstance() {
    static RenderPipeline m_instance; // thread safe

    return m_instance;
}

namespace {
    struct DirectionalLight {
        Vec3<float> direction = Vec3<float>(-0.5f, -0.8f, -0.6f).Normalize();
        float intensity = 2.f;
        Colour colour{ 1.0f, 0.98f, 0.95f, 1.f };
    };

    DirectionalLight worldLight{};

    // Some helper functions
    ProjectionVertex InterpolateProjectionVertex(const ProjectionVertex& outVert, const ProjectionVertex& inVert) {
        // ProjectionVertex only has colour, and position as we are dealing with simply vertex colours which we shade before hand.
        Vec4<float> outPos = outVert.GetClipPosition();
        Vec4<float> inPos = inVert.GetClipPosition();

        float interp = (-outPos.z) / (inPos.z - outPos.z);
        Vec4<float> clippedPos = ((inPos - outPos) * interp) + outPos;

        return ProjectionVertex(clippedPos, outVert.GetColour());
    }

    // Need to create 2 interpolated points. So when using this function, attempt to maintain CCW ordering.
    void NearClipOneVert(std::vector<ProjectionVertex>& clipVertices, const ProjectionVertex& outVert, const ProjectionVertex& inVert1, const ProjectionVertex& inVert2) {
        ProjectionVertex clippedVert1 = InterpolateProjectionVertex(outVert, inVert1);
        ProjectionVertex clippedVert2 = InterpolateProjectionVertex(outVert, inVert2);

        clipVertices.push_back(inVert1);
        clipVertices.push_back(inVert2);
        clipVertices.push_back(clippedVert2);
        
        clipVertices.push_back(inVert1);
        clipVertices.push_back(clippedVert2);
        clipVertices.push_back(clippedVert1);
    }

    void NearClipTwoVerts(std::vector<ProjectionVertex>& clipVertices, const ProjectionVertex& outVert1, const ProjectionVertex& outVert2, const ProjectionVertex& inVert) {
        ProjectionVertex clippedVert1 = InterpolateProjectionVertex(outVert1, inVert);
        ProjectionVertex clippedVert2 = InterpolateProjectionVertex(outVert2, inVert);

        clipVertices.push_back(inVert);
        clipVertices.push_back(clippedVert1);
        clipVertices.push_back(clippedVert2);
    }

}

// IN ATTRIBUTES I SHOULD PROBABLY ADD A FLAG FOR IF IT SHOULD BE WIREFRAME OR NOT, OR HAVE AN OVERRIDE FOR IT NOT SURE!
constexpr bool wireframe = false;
void RenderPipeline::Render(const std::vector<Vertex>& vertices, const Mat4<float>& modelMatrix, const Mat4<float>& ViewMatrix, const Colour& col) const {
    // Pretend Camera and Lights [DOING]
    //Mat4<float> cameraView = Mat4<float>::GetIdentity();

    // Vertex -> MV applied to get View and World space coords new Vertex = VertexPrime
    // VertexPrime -> turned into some form of Triangle = Triangle
    // Triangle -> Tessellated, and displacement mapped to generate more triangles of Type Triangle
    // VertexShading and Lighting applies, (not sure if this constitues any change, let's say not for now)
    // Triangle has it's points with projection applied, we do some culling and clipping & finally perspective divide
    // That perspective divided thing should have new vertices and new triangle type
    // Draw triangle with new type TriNew;

    // Need to also worry about moving light into either view or keep in world space. (may even have it's own model matrix to apply to itself)

    // ---- Object -> Model -> view transform Vertex Process ---- [DONE]
    std::vector<ViewVertex> viewVerts;
    viewVerts.reserve(vertices.size());

    for (const Vertex& v : vertices) {

        // Apply MV properly with copy of v. -> new object: ViewVertex, which we use to assemble a triangle TriOut or Triangle?
        viewVerts.push_back(ProcessVertex(v, modelMatrix, ViewMatrix));
    }

    int breakA = 1;

    std::vector<ViewVertex> viewVertsAfterCull;
    viewVertsAfterCull.reserve(viewVerts.size());
    
    // ---- BACK FACE CULL [DONE] ----
    for (int i = 0; i < viewVerts.size(); i += 3) {
        Vec3<float> faceNormal = Triangle::ComputeFaceNormal(viewVerts[i].GetViewPosition(), viewVerts[i + 1].GetViewPosition(), viewVerts[i + 2].GetViewPosition());

        // Get our camera view:
        Vec3<float> viewDir = viewVerts[i].GetViewPosition().GetNormalized();
        float viewNormal = Vec3<float>::DotProduct(faceNormal, viewDir);

        if (viewNormal > 0.f) { // perhaps with an epsilon added if need be.
            //continue;
        }

        // reaching here means it's a front-face:
        viewVertsAfterCull.insert(viewVertsAfterCull.end(), {viewVerts[i], viewVerts[i + 1], viewVerts[i + 2]});
    }
    // ---- View Frustrum rejection (triangle-level) [DONE] ---- Essentially an early rejection, as not in clip-space
    std::vector<ViewVertex> viewVertsAfterFrustum;
    viewVertsAfterFrustum.reserve(viewVertsAfterCull.size());
    
    float hScale = m_yScale * m_aspectRatio;
    for (int i = 0; i < viewVertsAfterCull.size(); i += 3) {
        const Vec3<float>& v0 = viewVertsAfterCull[i].GetViewPosition();
        const Vec3<float>& v1 = viewVertsAfterCull[i + 1].GetViewPosition();
        const Vec3<float>& v2 = viewVertsAfterCull[i + 2].GetViewPosition();

        // REJECT TOP/BOTTOM (Y-AXIS)
        /*if (v0.y > -v0.z * hScale && v1.y > -v1.z * hScale && v2.y > -v2.z * hScale) { continue; }
        if (v0.y < v0.z * hScale && v1.y < v1.z * hScale && v2.y < v2.z * hScale) { continue; }

        // REJECT LEFT/RIGHT (X-AXIS)
        if (v0.x > -v0.z * hScale && v1.x > -v1.z * hScale && v2.x > -v2.z * hScale) { continue; }
        if (v0.x < v0.z * hScale && v1.x < v1.z * hScale && v2.x < v2.z * hScale) { continue; }

        // REJECT NEAR/FAR (Z-AXIS)
        char textBuffer[128];
        //snprintf(textBuffer, sizeof(textBuffer), "(v0, v1, v2) Z: (%f, %f, %f), (n, f): (%f, %f)", v0.z, v1.z, v2.z, -this->n, -this->f);
        //App::Print(10, APP_VIRTUAL_HEIGHT - 120 - (i * 10), textBuffer, 0.5f, 1.0f, 0.5f, GLUT_BITMAP_HELVETICA_10);
        if (v0.z > -this->n && v1.z > -this->n && v2.z > -this->n) { continue; }
        //if (v0.z > this->n && v1.z > this->n && v2.z > this->n) { continue; } // we do this instead of  -n to allow for displacement mapping to push vertices back into plane later,
        if (v0.z < -this->f && v1.z < -this->f && v2.z < -this->f) { continue; }*/

        viewVertsAfterFrustum.insert(viewVertsAfterFrustum.end(), { viewVertsAfterCull[i], viewVertsAfterCull[i + 1], viewVertsAfterCull[i + 2] });
    }


    // ---- Displacement mapping [TODO] ----
        // Re-compute normals
        // Re-test frustum
        // Kill triangles that got moved out (may need to worry about triangles that should also be moved in? not sure performance wise) (for moving in -> never resurrect dead triangles
            // instead will 1. expand object/sector bounds by max displacement offline, allow triangles near the frustrum to survive early tests, then kill triangles after displacement
            // if they exit frustum



    // ---- Vertex Shading & Lighting [TODO] ----
    for (ViewVertex& v : viewVertsAfterFrustum) {

        // could do a backface cull check here as well I guess? lol

        // change this to a smarter solution later when we do our PBR stuff
        //meshes[v0.GetMaterialIndex()].material;
            /*
            v0.SetColour(m.map_Kd->SampleNearest(v0.GetUV()));
            v1.SetColour(m.map_Kd->SampleNearest(v1.GetUV()));
            v2.SetColour(m.map_Kd->SampleNearest(v2.GetUV()));
            */
        //v.SetColour(modelAttributes.material->map_Kd->SampleBilinear(v.GetUV()));
        Vec3<float> N = v.GetWorldNormal().GetNormalized();
        Vec3<float> L = worldLight.direction;
        float lambertianReflectance = std::max<float>(0.2f, N.DotProduct(L));
        lambertianReflectance *= worldLight.intensity;

        Vec3<float> albedo = col.GetVectorizedRGB();
        Vec3<float> lightCol = worldLight.colour.GetVectorizedRGB();

        //Vec3<float> lightRGB = (col * lambertianReflectance).GetVectorizedRGB();
        Vec3<float> rgb = albedo * lightCol * lambertianReflectance;

        rgb *= 0.6f;
        float a = 2.51f;
        float b = 0.03f;
        float C = 2.43f;
        float d = 0.59f;
        float e = 0.14f;

        rgb = (rgb * (rgb * a + b)) / (rgb * (rgb * C + d) + e);

        rgb.x = std::clamp(rgb.x, 0.0f, 1.0f);
        rgb.y = std::clamp(rgb.y, 0.0f, 1.0f);
        rgb.z = std::clamp(rgb.z, 0.0f, 1.0f);


        v.SetColour({rgb.x, rgb.y, rgb.z, 1.f});
        int t = 0;

        ///*\
        

        //v->SetColour(m.map_Kd->SampleBilinear(v0.GetUV()));
        //*/
    }

    // ---- Apply Projection ----[DONE]
    std::vector<ProjectionVertex> projectionVertices;
    projectionVertices.reserve(viewVertsAfterFrustum.size());
    
    for (const ViewVertex& v : viewVertsAfterFrustum) {
        projectionVertices.push_back(ProjectVertex(v));
    }
    int breakB = 1;

    // ---- ClipSpace Cull & near-plane clipping [TODO] -> do before perspective divide. ----
    std::vector<ProjectionVertex> clippedVertices;
    clippedVertices.reserve(projectionVertices.size());

    for (int i = 0; i < projectionVertices.size(); i += 3) {
        const ProjectionVertex& v0 = projectionVertices[i];
        Vec4<float> pos0 = v0.GetClipPosition();

        const ProjectionVertex& v1 = projectionVertices[i + 1];
        Vec4<float> pos1 = v1.GetClipPosition();

        const ProjectionVertex& v2 = projectionVertices[i + 2];
        Vec4<float> pos2 = v2.GetClipPosition();

        // Check if triangle is fully outside planes:
        bool near0 = pos0.z < -pos0.w;
        bool near1 = pos1.z < -pos1.w;
        bool near2 = pos2.z < -pos2.w;

        // Near
        if (near0 && near1 && near2) {
            continue;
        }
        // Far
        if (pos0.z > pos0.w && pos1.z > pos1.w && pos2.z > pos2.w) {
            continue;
        }
        // Left
        if (pos0.x < -pos0.w && pos1.x < -pos1.w && pos2.x < -pos2.w) {
            continue;
        }
        // Right
        if (pos0.x > pos0.w && pos1.x > pos1.w && pos2.x > pos2.w) {
            continue;
        }
        // Bottom
        if (pos0.y < -pos0.w && pos1.y < -pos1.w && pos2.y < -pos2.w) {
            continue;
        }
        // Top
        if (pos0.y > pos0.w && pos1.y > pos1.w && pos2.y > pos2.w) {
            continue;
        }

        if (!near0 && !near1 && !near2) {
            clippedVertices.insert(clippedVertices.end(), { v0, v1, v2 });
            continue;
        }


        // Near clipping - find which vertices are partially outside of the plane at this point while maintaining CCW (paint is my saviour)
        if (near0 && near1) {
            NearClipTwoVerts(clippedVertices, v0, v1, v2);
        }
        else if (near1 && near2) {
            NearClipTwoVerts(clippedVertices, v1, v2, v0);
        }
        else if (near0 && near2) {
            NearClipTwoVerts(clippedVertices, v2, v0, v1);
        }
        else if (near0) {
            NearClipOneVert(clippedVertices, v0, v1, v2);
        }
        else if (near1) {
            NearClipOneVert(clippedVertices, v1, v2, v0);
        }
        else if (near2) {
            NearClipOneVert(clippedVertices, v2, v0, v1);
        }
    }

    // ---- Perspective Divide----[DONE]
    // ---- Viewport mapping ----[DONE]
        // Reject degenerate triangles/zero-area triangles [TODO]
    std::vector<ScreenSpaceVertex> finalVertices;
    finalVertices.reserve(clippedVertices.size());
    
    for (const ProjectionVertex& v : clippedVertices) {
        finalVertices.push_back(HomogenizeAndViewportMap(v));
    }
    int breakC = 1;

    // ---- Submit to API ---- [DONE]
    for (int i = 0; i < finalVertices.size(); i += 3) {
        SubmitTriangle(finalVertices[i], finalVertices[i + 1], finalVertices[i + 2], wireframe);
    }
    
}

ViewVertex RenderPipeline::ProcessVertex(const Vertex& v, const Mat4<float> M, const Mat4<float> V) const {
    Vec4<float> worldPos = Vec4<float>(v.GetPosition(), 1) * M;
    Vec4<float> viewPos = worldPos * V;

    Mat4<float> MVNormalMatrix = (M * V).GetNormalMatrix();

    Vec4<float> worldNormal = Vec4<float>(v.GetNormal(), 0) * M.GetNormalMatrix();
    Vec4<float> viewNormal = Vec4<float>(v.GetNormal(), 0) * MVNormalMatrix;

    Vec4<float> viewTangent = Vec4<float>(v.GetTangent(), 0) * M * V;

    return ViewVertex(worldPos, viewPos, worldNormal, viewNormal, viewTangent, v.GetUV(), v.GetColour(), v.GetMeshIndex(), v.GetMaterialIndex(), v.GetUniqueIndex());
}

ProjectionVertex RenderPipeline::ProjectVertex(const ViewVertex& v) const {
    Vec4<float> projectPosition = Vec4<float>(v.GetViewPosition(), 1) * m_projectionMatrix;
    return ProjectionVertex(projectPosition, v.GetColour());
}

ScreenSpaceVertex RenderPipeline::HomogenizeAndViewportMap(const ProjectionVertex& v) const {
    const Vec4<float>& oldPos = v.GetClipPosition();
    float w = oldPos.w;

    float ndcX = oldPos.x / w;
    float ndcY = oldPos.y / w;
    float ndcZ = oldPos.z / w;

    float x = (ndcX + 1.f) * 0.5f;
    float y = (1.f - ndcY) * 0.5f;
    
    Vec2<float> screenPos = {x * APP_VIRTUAL_WIDTH, y * APP_VIRTUAL_HEIGHT };
    float z = ndcZ; // 0 - 1 right now, should make it so my projection matrix goes -1 <-> 1 instead fix later.

    return ScreenSpaceVertex(screenPos, z, w, v.GetColour()); // We transition to Vec3 colours here, may be able to do this earlier (or entirely) due to only vert colours not per fragment
}

void RenderPipeline::SubmitTriangle(const ScreenSpaceVertex& v1, const ScreenSpaceVertex& v2, const ScreenSpaceVertex& v3, bool isWireframe) const {
    auto pos1 = v1.GetScreenPosition();
    auto col1 = v1.GetColour();

    auto pos2 = v2.GetScreenPosition();
    auto col2 = v2.GetColour();

    auto pos3 = v3.GetScreenPosition();
    auto col3 = v3.GetColour();
    
    App::DrawTriangle(pos1.x, pos1.y, v1.GetDepth(), 1, pos2.x, pos2.y, v2.GetDepth(), 1, pos3.x, pos3.y, v3.GetDepth(), 1,
        col1.x, col1.y, col1.z, col2.x, col2.y, col2.z, col3.x, col3.y, col3.z, isWireframe);
    
    /*App::DrawTriangle(pos1.x, pos1.y, v1.GetDepth(), v1.GetW(), pos2.x, pos2.y, v2.GetDepth(), v2.GetW(), pos3.x, pos3.y, v3.GetDepth(), v3.GetW(),
        col1.x, col1.y, col1.z, col2.x, col2.y, col2.z, col3.x, col3.y, col3.z, isWireframe);*/
    
}

    /* Input assembler(is this function)
     * Vertex Shader
     * Tessellation stage (Shader)?
     * Geometry Shader
     * Rasterizer, pixel shader, output merger (all done for us by App::DrawTriangle();
     * 
     * ------------- MY OLD PIPELINE ----------------
     * Take in vector vertices, indices, and modelAttributes as well as allow bindable vertex shader?
     * Grab camera view matrix, reserve space for the clip position vertices,
     * then apply the vertex shader on each vertex -> get's world position via vertex * modelMatrix, gets ViewPos with worldPos * view matrix,
     * gets normal matrix via getting the normal matrix call from the modelMatrix,
     * Normal is the vertexIn.GetNormal() (0 for 4th value (w)), * normal matrix, and then we normalize the normal vector,
     * for Tangent we Get tangent from our vertex, and also multiply it by normal matrix (again 0 for 4th value). & normalize it after
     * We then do Gram-Schmidt orthongonalization for accuracy of our tangent to calculate the best normalized value
     * We calculate the bitanget,
     * and finally output a vertex out with all of those values (which may do some extra steps)
     * 
     * Then we're in clip space and do a clipping check, if it's not within frustum we cull the triangle,
     * we do partial clipping if a point is not in near frustum and do the whole clipping algorithm
     * 
     * Then we perspective divide and create the triangle assembly with all of our computation of face normal after viewposition as well, and face world normal
     * Then we do backface culling
     * Then a viewport transformation
     * Then finally we rasterize the triangle
     * 
     * We interpolate and get the w, z, vertex normal, UV, tangent, bitangent, and do perspective correct interpolation
     * Do our HDR and Tone Mapping
     * 
     * In fragment shader (before HDR and tone mapping etc)
     * We check for an albedo texture, if there is one we sample either nearest or bilinear based on passed in sampling mode,
     * and we transform sRGB to linear space (only for albedo) to power of GAMMA_LINEAR
     * We create our TBN matrix, may also want a way to specify winding/handedness of texture to decide if we need to flip the y value of normal sample from normal map
     * then we calculate diffuse intensity, do some tone mapping
     * ACES approximated
     * and pass back our values before they are gamma corrected
     * 
     *  ------------- NEW HYBRID PIPELINE ----------------
     * Input assembler,
     * Vertex processing (triangle, model matrix, camera view-projection matrix)
     *      -> Do world transform: Multiply position, normal and tangnet by model matrix so all lighting will happen in World Space
     *      -> Backface Culling: Calculate triangles face normal, if it points away from camera, discard it
     *      -> Size estimation: Project the vertiuces to "clip space" (temporarily) to measure how many pixels wide the triangle is on the screen
     *      => OUTPUT: World space triangle + screen size metric
     * Dynamic Tessellation Stage (World space triangle, Screen size) [Potentially also the level of refinement, such as 8x8, 4x4, 2x2, 1x1 based on quality settings menu]
     *      -> Check if screen size > TargetPixel Size (eg. 15px), if YES
     *          -> Split: Calculate midpoints of edges, interpoalte and create new vertices, and LERP position, normal, tangent, and UVS
     *              -> Recusive feed the 4 new smaller triangles into the our dynamic tessellation stage
     *          -> IF NO: Triangle is small enough, no splitting, send to shader
     *      => OUTPUT: "micropolygons" in world space.
     * Displacement Mapping Stage (vertex, material, and scale)
     *      -> get the height from smapling the displacement map, move position along the normal
     *      -> (High quality method)
     *          -> to Re-calculate normals, sample displacement map 3 times to figure out "slope of texture" and adjust normal accordingly
     *      -> (Low quality faster method): Geometric normal
     *          -> Recalculate the face normal of the new triangle, so we flat shade instead
     *      [All of our stuff could be flat shaded due to how small our triangles are]
     *      [For bounding boxes, since displacement pushes geometry outside of original bounding box, must increase objects bounding box by maximum possible displacement amount,
     *      as I shouldn't cull triangles that later become visible due to displacement map]
     *      
     *      
     * Vertex Shading and Lighting (Fragment shader knock off) (triangle of 3 vertices)
     *      -> Texture fetch: Use UV to sample albedo, roughness, metallic, and normal maps
     *      -> Normal mapping:
     *          -> Unpack normal map sample from RGB to vector
     *          -> construct TBN matrix using vertex's world normal and world tangent
     *          -> transform texture normal into world space
     *      -> PBR lighting:
     *          -> Calculate light vector (Light pos - Vertex pos)
     *          -> Calculate View Vector (Cam pos - vertex pos)
     *          -> Run Cook-Torrance PBR math (diffuse + specular based on roughness/metallic)
     *      => OUTPUT: 3 vertices with final colour
     * Primitive Assembly and projection (3 lit vertices in world space)
     *      -> ClipSpace transform, multiply world position by viewprojection matrix (may only do view matrix, and let draw triangle do the rest, as it seems it does the perspective divide)
     *      -> Perspective divide
     *      -> Viewport mapping
     * 
     * 
     * 
     *
     
}*/

void RenderPipeline::ResizeWindowProjection(float width, float height) {
    float aspectRatio = width / height;
    m_aspectRatio = aspectRatio;

    m_projectionMatrix[0][0] = 1.f / (m_yScale * aspectRatio);
}

