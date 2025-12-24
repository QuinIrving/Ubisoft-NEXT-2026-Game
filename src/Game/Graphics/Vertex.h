#pragma once
#include <mutex> // I believe I'm allowed. If not, switch to a non-thread safe approach
#include <vector>

/*
    Singleton (thread safe) Pipeline class for rendering.
    Handles vertices per mesh -> vertex shader -> clipping -> tessellation -> texture mapping & lighting -> call rasterization
*/
class Pipeline {
public:
    static Pipeline* GetInstance();

    // business logic -> probably some way to bind vertex shader, not sure if needed though.
    void Render(const std::vector<int>& vertices, const std::vector<uint32_t>& indices, const int& modelAttributes);
    // Vertex vertices, uint32_t indices, ModelAttributes modelAttributes, potentially a vertex shader passed in, but binding is probably smarter?

    //VertexOut VertexShader(const Vertex& vin, const ModelAttributes& MA, const Mat4<float>& V, const Mat4<float>& P);

    Pipeline(Pipeline& other) = delete;
    void operator=(const Pipeline&) = delete;

protected:
    constexpr Pipeline() = default;
    ~Pipeline() {};

private:
    static Pipeline* m_instance;
    static std::mutex m_mutex;
};
