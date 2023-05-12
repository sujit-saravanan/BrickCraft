#include "context_manager.h"
#include "logger/logger.h"

DapperCraft::EngineContext::EngineContext(std::string_view window_title, glm::ivec2 window_dimensions) {
        TRACE("Initializing Engine Context...");
        
        TRACE("\t⎿ Initializing GLFW...");
        glfwInit();
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        TRACE("\t⎿ Initialized GLFW");
        
        
        TRACE("\t⎿ Creating GLFW Window...");
        m_window = glfwCreateWindow(window_dimensions.x, window_dimensions.y, "Vulkan", nullptr, nullptr);
        TRACE("\t⎿ Created GLFW Window");
        
        m_render_engine.init(m_window);
        
        TRACE("Completed Engine Context Initialization");
}
DapperCraft::EngineContext::~EngineContext() {
        TRACE("Destroying Engine Context...");
        
        TRACE("\t⎿ Destroying GLFW Window...");
        glfwDestroyWindow(m_window);
        TRACE("\t⎿ Destroyed GLFW Window");
        TRACE("\t⎿ Terminating GLFW...");
        glfwTerminate();
        TRACE("\t⎿ Terminated GLFW");
        
        
        
        TRACE("Destroyed Engine Context...");
}
void DapperCraft::EngineContext::draw() {

}
bool DapperCraft::EngineContext::update() {
        glfwPollEvents();
        return true;
}
void DapperCraft::EngineContext::run() {
        TRACE("Starting Engine Run Loop...");
        while (!glfwWindowShouldClose(m_window)) {
                update();
                draw();
        }
        TRACE("Ending Engine Run Loop...");
}
