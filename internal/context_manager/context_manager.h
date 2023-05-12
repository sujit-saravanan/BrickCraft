#pragma once
#include <glm/glm.hpp>
#include <string_view>
#include "render_engine.h"

namespace DapperCraft {
        class EngineContext {
        public: // Public constructors/destructors/overloads
                EngineContext(std::string_view window_title, glm::ivec2 window_dimensions);
                ~EngineContext();
        public: // Public methods
                void draw();
                bool update();
                void run();
        public: // Public members
        
        private: // Private methods
        
        private: // Private members
                GLFWwindow* m_window{nullptr};
                details::RenderEngine m_render_engine;
        };
}