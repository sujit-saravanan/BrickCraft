#pragma once
#include <cstdint>
#include "vkpch.h"
#include <GLFW/glfw3.h>
#include <optional>


namespace DapperCraft {
        class EngineContext;
}

namespace DapperCraft::details {
        struct QueueFamilyIndices {
                std::optional<uint32_t> graphics_family;
                std::optional<uint32_t> present_family;
                
                [[nodiscard]] bool isComplete() const;
        };
        struct SwapchainSupportDetails {
                vk::SurfaceCapabilitiesKHR capabilities;
                std::vector<vk::SurfaceFormatKHR> formats;
                std::vector<vk::PresentModeKHR> present_modes;
                
        };
        
        class RenderEngine {
        public: // Public constructors/destructors/overloads
                RenderEngine() = default;
                ~RenderEngine();
                
        public: // Public methods
                void init(GLFWwindow* window);
                
        public: // Public members
        
        private: // Private methods
                static std::vector<const char*> getInstanceExtensions();
                QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
                uint64_t rankPhysicalDevice(const vk::PhysicalDevice &physical_device);
                SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice physical_device);
                
                void createInstance();
                void createDebugMessenger();
                void createSurface(GLFWwindow* window);
                void pickPhysicalDevice();
                void createLogicalDevice();
                void createSwapchain(GLFWwindow* window);
                void createImageViews();
                void createComputePipeline();
                
        private: // Private members
                vk::Instance m_instance;
                VkDebugUtilsMessengerEXT m_debug_messenger{};
                vk::PhysicalDevice m_physical_device{};
                vk::Device m_device{};
                vk::Queue m_graphics_queue{};
                vk::Queue m_present_queue{};
                vk::SurfaceKHR m_surface{};
                vk::SwapchainKHR m_swapchain{};
                std::vector<vk::Image> m_swapchain_images{};
                vk::Format m_swapchain_iamge_format{};
                vk::Extent2D m_swapchain_extent{};
                std::vector<vk::ImageView> m_swapchain_image_views{};
        };
}