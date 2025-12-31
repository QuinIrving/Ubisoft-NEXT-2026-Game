#include <app.h>
#include "Pipeline.h"
#include "Vertex.h"
#include "VertexOut.h"
#include "VertexPostClip.h"
#include "ViewVertex.h"
#include "ProjectionVertex.h"
#include "ScreenSpaceVertex.h"

Pipeline& Pipeline::GetInstance() {
    static Pipeline m_instance; // thread safe

    return m_instance;
}

// IN ATTRIBUTES I SHOULD PROBABLY ADD A FLAG FOR IF IT SHOULD BE WIREFRAME OR NOT, OR HAVE AN OVERRIDE FOR IT NOT SURE!
constexpr bool wireframe = true;
void Pipeline::Render(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const ModelAttributes& modelAttributes) const {
    // Pretend Camera and Lights, and model Matrix
    Mat4<float> pretendModel = Mat4<float>::GetIdentity();
    Mat4<float> cameraView = Mat4<float>::GetIdentity();

    // Vertex -> MV applied to get View and World space coords new Vertex = VertexPrime
    // VertexPrime -> turned into some form of Triangle = Triangle
    // Triangle -> Tessellated, and displacement mapped to generate more triangles of Type Triangle
    // VertexShading and Lighting applies, (not sure if this constitues any change, let's say not for now)
    // Triangle has it's points with projection applied, we do some culling and clipping & finally perspective divide
    // That perspective divided thing should have new vertices and new triangle type
    // Draw triangle with new type TriNew;

    // Need to also worry about moving light into either view or keep in world space. (may even have it's own model matrix to apply to itself)

    // ---- Object -> Model -> view transform Vertex Process ----
    std::vector<ViewVertex> viewVerts;
    viewVerts.reserve(indices.size());

    for (uint32_t i : indices) {
        const Vertex& v = vertices[i];


        // Apply MV properly with copy of v. -> new object: ViewVertex, which we use to assemble a triangle TriOut or Triangle?
        viewVerts.push_back(ProcessVertex(v, pretendModel, cameraView));
    }

    int breakA = 1;

    // ---- BACK FACE CULL [BYPASS] ----
    // ---- View Frustrum rejection (triangle-level) [BYPASS] ----


    // ---- Screen-size Estimation [BYPASS] ----


    // ---- Dynamic tessellation [BYPASS] -> should output triangles, not vertices ---- (SHould take in triangles and output more triangles)


    // ---- Displacement mapping [BYPASS] ----
        // Re-compute normals
        // Re-test frustum
        // Kill triangles that got moved out (may need to worry about triangles that should also be moved in? not sure performance wise) (for moving in -> never resurrect dead triangles
            // instead will 1. expand object/sector bounds by max displacement offline, allow triangles near the frustrum to survive early tests, then kill triangles after displacement
            // if they exit frustum



    // ---- Vertex Shading & Lighting [BYPASS] ----


    // ---- Apply Projection ----
    std::vector<ProjectionVertex> projectionVertices;
    projectionVertices.reserve(viewVerts.size());
    
    for (const ViewVertex& v : viewVerts) {
        projectionVertices.push_back(ProjectVertex(v));
    }
    int breakB = 1;

    // ---- ClipSpace Cull & near-plane clipping [BYPASS] -> do before perspective divide. ----


    // ---- Perspective Divide----
    // ---- Viewport mapping ----
        // Reject degenerate triangles/zero-area triangles [BYPASS]
    std::vector<ScreenSpaceVertex> finalVertices;
    finalVertices.reserve(projectionVertices.size());
    
    for (const ProjectionVertex& v : projectionVertices) {
        finalVertices.push_back(HomogenizeAndViewportMap(v));
    }

    // ---- Submit to API ----
    for (int i = 0; i < finalVertices.size(); i += 3) {
        SubmitTriangle(finalVertices[i], finalVertices[i + 1], finalVertices[i + 2], wireframe);
    }
    
}

ViewVertex Pipeline::ProcessVertex(const Vertex& v, const Mat4<float> M, const Mat4<float> V) const {
    Vec4<float> worldPos = Vec4<float>(v.GetPosition(), 1) * M;
    Vec4<float> viewPos = worldPos * V;

    Mat4<float> MVNormalMatrix = (M * V).GetNormalMatrix();

    Vec4<float> worldNormal = Vec4<float>(v.GetNormal(), 0) * M.GetNormalMatrix();
    Vec4<float> viewNormal = Vec4<float>(v.GetNormal(), 0) * MVNormalMatrix;

    Vec4<float> viewTangent = Vec4<float>(v.GetTangent(), 0) * M * V;

    return ViewVertex(worldPos, viewPos, worldNormal, viewNormal, viewTangent, v.GetUV(), v.GetColour());
}

ProjectionVertex Pipeline::ProjectVertex(const ViewVertex& v) const {
    Vec4<float> projectPosition = Vec4<float>(v.GetViewPosition(), 1) * m_projectionMatrix;
    return ProjectionVertex(projectPosition, v.GetColour());
}

ScreenSpaceVertex Pipeline::HomogenizeAndViewportMap(const ProjectionVertex& v) const {
    const Vec4<float>& oldPos = v.GetClipPosition();
    float w = oldPos.w;

    float ndcX = oldPos.x / w;
    float ndcY = oldPos.y / w;
    float ndcZ = oldPos.z / w;

    float x = (ndcX + 1.f) * 0.5f;
    float y = (ndcY + 1.f) * 0.5f;
    
    Vec2<float> screenPos = {x * APP_VIRTUAL_WIDTH, y * APP_VIRTUAL_HEIGHT };
    float z = ndcZ;

    return ScreenSpaceVertex(screenPos, z, w, v.GetColour()); // We transition to Vec3 colours here, may be able to do this earlier (or entirely) due to only vert colours not per fragment
}

void Pipeline::SubmitTriangle(const ScreenSpaceVertex& v1, const ScreenSpaceVertex& v2, const ScreenSpaceVertex& v3, bool isWireframe) const {
    auto pos1 = v1.GetScreenPosition();
    auto col1 = v1.GetColour();

    auto pos2 = v2.GetScreenPosition();
    auto col2 = v2.GetColour();

    auto pos3 = v3.GetScreenPosition();
    auto col3 = v3.GetColour();
    
    /*
    App::DrawTriangle(pos1.x, pos1.y, v1.GetDepth(), 1, pos2.x, pos2.y, v2.GetDepth(), 1, pos3.x, pos3.y, v3.GetDepth(), 1,
        col1.x, col1.y, col1.z, col2.x, col2.y, col2.z, col3.x, col3.y, col3.z, isWireframe);
    */
    
    App::DrawTriangle(pos1.x, pos1.y, v1.GetDepth(), v1.GetW(), pos2.x, pos2.y, v2.GetDepth(), v2.GetW(), pos3.x, pos3.y, v3.GetDepth(), v3.GetW(),
        col1.x, col1.y, col1.z, col2.x, col2.y, col2.z, col3.x, col3.y, col3.z, isWireframe);
    
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

