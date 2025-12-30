#include "Pipeline.h"

Pipeline& Pipeline::GetInstance() {
    static Pipeline m_instance; // thread safe

    return m_instance;
}

void Pipeline::Render(const std::vector<int>& vertices, const std::vector<uint32_t>& indices, const int& modelAttributes) {
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
     */
}
