#include "Pipeline.h"

Pipeline* Pipeline::m_instance = NULL;
std::mutex Pipeline::m_mutex;

Pipeline* Pipeline::GetInstance() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_instance == NULL) {
        m_instance = new Pipeline();
    }

    return m_instance;
}

void Pipeline::Render(const std::vector<int>& vertices, const std::vector<uint32_t>& indices, const int& modelAttributes) {

}
