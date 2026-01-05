#include <app.h>
#include "TessellatedPipeline.h"
#include "Graphics/Triangle.h"
//#include "TessellationContext.h"
#include "TriangleNode.h"

TessellatedPipeline& TessellatedPipeline::GetInstance() {
    static TessellatedPipeline m_instance; // thread safe

    return m_instance;
}

/* --- My Tessellation Plan ---
 * To being with, I will on each unique object model initialized, I will pre-process and create a "TessellationContext" which will contain all of the unique triangles
 * of the mesh, with a hashmap for each edge which says given 2 vertex id's for a a triangle edge, which triangle id's the edge shares. 
 * The tessellation context also gets updated each frame with updated copies of my "ViewVertex" vertices, which are my vertices that have the model and view matrix already applied.
 * Along with this is the tessellation contains the screenScale, as well as threshold for whether an edge is long enough or not (could be modified in settings).
 * I also pre-process and create a LOD mesh, that contains the baseVertices of the mesh, and the LODNode pointer to the root nodes, which is essentially a list of all of
 * the different triangles in the base mesh.
 * The pre-processing creates these "triangles" which are labelled as "LODNode", which has the 3 vertex id's (to grab from the tessellation context), as well as
 * it's 2 children, and 3 neighbours (index represents the edge), as well as the splitEdge index that is pre-calculated on the pre-process, and the depth the node is, and whether
 * it is a leaf or not. 
 * From there, we then each frame update our tessellation context to get new ViewVertex values for each vertex, and also our viewvertices have flags to see if they have been early culled
 * (if so, we don't need to tessellate as they won't be seen, but we need them in the list for our triangle id's to work properly).
 * We then go through our rootNodes, and for each node we calculate the projected length of the edge given the new viewvertex values per frame, and if it's smaller than or equal to the thresh
 * hold, we set isleaf to true, and add it to the next step (maybe also have a flag to show if it's already been processed). If the edge length is larger than the threshold,
 * then we check if it's a leaf, if so we call SplitDiamond(node) on the node. We then inside of SplitDiamond, check if our neighbour also has the same SplitEdgeIndx (or same edge),
 * and if so then we can split both of them at the same time. If SplitEdgeIdx of our neighbour points somewhere else however, we call refindnode then on B, Once we finish splitting our
 * neighbour and ourself, then we do the same on our children and the neighbours children.
 * From what I can tell, we should at some point maintain a queue or stack of some sort (probably stack), and keep on adding on to the stack the neighbour, until we find a shared edge, then 
 * pop them off / remove them, and do the same for the rest of the previously enquired nodes and their chidlren.
 * 
*/


// IN ATTRIBUTES I SHOULD PROBABLY ADD A FLAG FOR IF IT SHOULD BE WIREFRAME OR NOT, OR HAVE AN OVERRIDE FOR IT NOT SURE!
constexpr bool wireframe = true;
constexpr float errorThreshold = 8.0f; // This is where our quality settings should be able to reduce this down smaller to get smaller triangles.
// Should be a pipeline specific member variable that can be changed based on setting chose ^.

void TessellatedPipeline::Render(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const ModelAttributes& modelAttributes) {
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

    // Pre-process triangle mesh (should be done on object load/read first-time, or first time creating a defined object such as a Quad. (shouldn't be every frame like I do here)
    auto map = PreProcessMesh(vertices, indices);
    int breakO;

    // ---- Object -> Model -> view transform Vertex Process ---- [DONE]
    std::vector<ViewVertex> viewVerts;
    viewVerts.reserve(indices.size());

    /*for (uint32_t i : indices) {
        const Vertex& v = vertices[i];

        // Apply MV properly with copy of v. -> new object: ViewVertex, which we use to assemble a triangle TriOut or Triangle?
        viewVerts.push_back(ProcessVertex(v, modelAttributes.modelMatrix, camera.GetViewMatrix()));
    }*/
    for (const Vertex& v : map.processedMesh) {
        // Apply MV properly with copy of v. -> new object: ViewVertex, which we use to assemble a triangle TriOut or Triangle?
        viewVerts.push_back(ProcessVertex(v, modelAttributes.modelMatrix, camera.GetViewMatrix()));
    }

    tessellateStack.reserve(map.processedMesh.size() / 3);
    tessellateStack.clear();

    // Create my base TriangleNodes via the ViewVerts given.
    for (int i = 0; i < viewVerts.size(); i += 3) {
        TriangleNode node;
        node.depth = 0;
        node.baseTriIdx = i / 3;
        //node.nodeID = ?
        node.v0 = viewVerts[i].GetViewPosition();
        node.v1 = viewVerts[i + 1].GetViewPosition();
        node.v2 = viewVerts[i + 2].GetViewPosition();

        uint32_t mIdx0 = viewVerts[i].GetMeshIndex();
        uint32_t mIdx1 = viewVerts[i + 1].GetMeshIndex();
        uint32_t mIdx2 = viewVerts[i + 2].GetMeshIndex();

        //node.neighbourBaseTriangleIdx[0] = -1;
        uint64_t key = MakeEdgeKey(mIdx0, mIdx1);
        if (map.adjacencyTable[key].size() == 1) {
            node.neighbourBaseTriangleIdx[0] = -1;
        }
        else {
            for (uint32_t a : map.adjacencyTable[key]) {
                if (a != node.baseTriIdx) {
                    node.neighbourBaseTriangleIdx[0] = a;
                    break;
                }
            }
        }

        key = MakeEdgeKey(mIdx1, mIdx2);
        if (map.adjacencyTable[key].size() == 1) {
            node.neighbourBaseTriangleIdx[1] = -1;
        }
        else {
            for (uint32_t a : map.adjacencyTable[key]) {
                if (a != node.baseTriIdx) {
                    node.neighbourBaseTriangleIdx[1] = a;
                    break;
                }
            }
        }

        key = MakeEdgeKey(mIdx2, mIdx0);
        if (map.adjacencyTable[key].size() == 1) {
            node.neighbourBaseTriangleIdx[2] = -1;
        }
        else {
            for (uint32_t a : map.adjacencyTable[key]) {
                if (a != node.baseTriIdx) {
                    node.neighbourBaseTriangleIdx[2] = a;
                    break;
                }
            }
        }


        tessellateStack.push_back(node);
    }

    // First-Pass tessellate to find how far we should maximally split depth for each triangle node.
    while (!tessellateStack.empty()) {
        TriangleNode node = tessellateStack.back();
        tessellateStack.pop_back();

        // check the nodes view edges, if any of them are larger than the threshold, then split. 
        // Longest edge for the children is the edge not having the midpoint new vertex.
    }

    
    // Second-pass Tessellate, go down to the depth of each depth level we have for each node, and interpolate all of the ViewVertex values,
    // ensure to also properly backface-cull and frustum cull early here to reduce triangle tessellation at this stage.
    int test = 2;





                // Process my viewVerts, and my tessellation at the same time, while being mindful of back face cull and viewfrustum
                //std::list<LODNode*> nodesToProcess;
                //std::map<std::string, ViewVertex> nodeVertexMap;

                //nodesToProcess.insert(nodesToProcess.end(), map.m_triangleLODTree);
                //for (LODNode& node : map.m_triangleLODTree) {
                    // Lets proccess each base node first.


                    //nodesToProcess.push_back(&node);
                //}

                //for (LODNode* node : nodesToProcess) {

                //}

                /*
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
                        continue;
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
                    if (v0.y > -v0.z * hScale && v1.y > -v1.z * hScale && v2.y > -v2.z * hScale) { continue; }
                    if (v0.y < v0.z * hScale && v1.y < v1.z * hScale && v2.y < v2.z * hScale) { continue; }

                    // REJECT LEFT/RIGHT (X-AXIS)
                    if (v0.x > -v0.z * hScale && v1.x > -v1.z * hScale && v2.x > -v2.z * hScale) { continue; }
                    if (v0.x < v0.z * hScale && v1.x < v1.z * hScale && v2.x < v2.z * hScale) { continue; }

                    // REJECT NEAR/FAR (Z-AXIS)
                    char textBuffer[128];
                    snprintf(textBuffer, sizeof(textBuffer), "(v0, v1, v2) Z: (%f, %f, %f), (n, f): (%f, %f)", v0.z, v1.z, v2.z, -this->n, -this->f);
                    App::Print(10, APP_VIRTUAL_HEIGHT - 120 - (i * 10), textBuffer, 0.5f, 1.0f, 0.5f, GLUT_BITMAP_HELVETICA_10);
                    if (v0.z > -this->n && v1.z > -this->n && v2.z > -this->n) { continue; }
                    //if (v0.z > this->n && v1.z > this->n && v2.z > this->n) { continue; } // we do this instead of  -n to allow for displacement mapping to push vertices back into plane later,
                    if (v0.z < -this->f && v1.z < -this->f && v2.z < -this->f) { continue; }

                    viewVertsAfterFrustum.insert(viewVertsAfterFrustum.end(), { viewVertsAfterCull[i], viewVertsAfterCull[i + 1], viewVertsAfterCull[i + 2] });
                }


                // ---- Screen-size Estimation [TODO] ----
                /*
                Here is how I have been reading it. THe simplistic approach is in some way estimate the edge lengths in screen space
                with some form of either applying the projection matrix to get screen size, or some form of estimate. Get the max edge
                between the 3 vertices of the triangle given, and if it's too large, then we do a midpoint subidviision to go from 
                1 tri -> 4 triangles, and do that a certain amount of times recursively. Let's see if there is a better algorithm
                that is perhaps more performant, need to see.

                ------- Reyes algorithms --------

                * /

                // ---- Dynamic tessellation [TODO] -> should output triangles, not vertices ---- (SHould take in triangles and output more triangles)
                    // Don't forget to cache LOD, and try to ensure not to have swimming/gaps in edges of shared triangles with different outer edge levels
                */

    /*
    // ---- Displacement mapping [TODO] ----
        // Re-compute normals
        // Re-test frustum
        // Kill triangles that got moved out (may need to worry about triangles that should also be moved in? not sure performance wise) (for moving in -> never resurrect dead triangles
            // instead will 1. expand object/sector bounds by max displacement offline, allow triangles near the frustrum to survive early tests, then kill triangles after displacement
            // if they exit frustum



    // ---- Vertex Shading & Lighting [TODO] ----

    
    // ---- Apply Projection ----[DONE]
    std::vector<ProjectionVertex> projectionVertices;
    projectionVertices.reserve(viewVertsAfterFrustum.size());
    
    for (const ViewVertex& v : viewVertsAfterFrustum) {
        projectionVertices.push_back(ProjectVertex(v));
    }
    int breakB = 1;

    // ---- ClipSpace Cull & near-plane clipping [TODO] -> do before perspective divide. ----


    // ---- Perspective Divide----[DONE]
    // ---- Viewport mapping ----[DONE]
        // Reject degenerate triangles/zero-area triangles [TODO]
    std::vector<ScreenSpaceVertex> finalVertices;
    finalVertices.reserve(projectionVertices.size());
    
    for (const ProjectionVertex& v : projectionVertices) {
        finalVertices.push_back(HomogenizeAndViewportMap(v));
    }
    int breakC = 1;

    // ---- Submit to API ---- [DONE]
    for (int i = 0; i < finalVertices.size(); i += 3) {
        SubmitTriangle(finalVertices[i], finalVertices[i + 1], finalVertices[i + 2], wireframe);
    }*/
    
}

ViewVertex TessellatedPipeline::ProcessVertex(const Vertex& v, const Mat4<float> M, const Mat4<float> V) const {
    Vec4<float> worldPos = Vec4<float>(v.GetPosition(), 1) * M;
    Vec4<float> viewPos = worldPos * V;

    Mat4<float> MVNormalMatrix = (M * V).GetNormalMatrix();

    Vec4<float> worldNormal = Vec4<float>(v.GetNormal(), 0) * M.GetNormalMatrix();
    Vec4<float> viewNormal = Vec4<float>(v.GetNormal(), 0) * MVNormalMatrix;

    Vec4<float> viewTangent = Vec4<float>(v.GetTangent(), 0) * M * V;

    return ViewVertex(worldPos, viewPos, worldNormal, viewNormal, viewTangent, v.GetUV(), v.GetColour(), v.GetMeshIndex(), v.GetUniqueIndex());
}

ProjectionVertex TessellatedPipeline::ProjectVertex(const ViewVertex& v) const {
    Vec4<float> projectPosition = Vec4<float>(v.GetViewPosition(), 1) * m_projectionMatrix;
    return ProjectionVertex(projectPosition, v.GetColour());
}

ScreenSpaceVertex TessellatedPipeline::HomogenizeAndViewportMap(const ProjectionVertex& v) const {
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

void TessellatedPipeline::SubmitTriangle(const ScreenSpaceVertex& v1, const ScreenSpaceVertex& v2, const ScreenSpaceVertex& v3, bool isWireframe) const {
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

void TessellatedPipeline::ResizeWindowProjection(float width, float height) {
    float aspectRatio = width / height;
    m_aspectRatio = aspectRatio;

    m_projectionMatrix[0][0] = 1.f / (m_yScale * aspectRatio);
}

