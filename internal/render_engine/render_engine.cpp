#include <span>
#include <set>
#include "render_engine.h"
#include "logger/logger.h"
#include "config.h"

// Extension helper functions
bool extensionIsAvailable(const char* required_extension, const std::vector<vk::ExtensionProperties> &available_extensions) {
        for (auto extension: available_extensions)
                if (strcmp(extension.extensionName.data(), required_extension) == 0)
                        return true;
        return false;
}
std::vector<const char*> DapperCraft::details::RenderEngine::getInstanceExtensions() {
        TRACE("\t\t⎿ Obtaining Available Instance Extensions..");
        std::vector<vk::ExtensionProperties> available_extensions = vk::enumerateInstanceExtensionProperties();
        for (auto extension: available_extensions) {
                TRACE("\t\t\t⎿ Found: %s", extension.extensionName.data());
        }
        TRACE("\t\t⎿ Obtained Available Instance Extensions");
        
        TRACE("\t\t⎿ Obtaining Required Instance Extensions...");
        std::vector<const char*> required_extensions;
        
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        
        for (auto extension: std::span(glfw_extensions, glfw_extension_count)) {
                TRACE("\t\t\t⎿ Required: %s", extension);
                required_extensions.emplace_back(extension);
        }
        for (auto extension: additional_instance_extensions) {
                TRACE("\t\t\t⎿ Required: %s", extension);
                required_extensions.emplace_back(extension);
        }
        TRACE("\t\t⎿ Obtained Required Instance Extensions");
        
        TRACE("\t\t⎿ Validating Required Instance Extensions...");
        for (auto extension: required_extensions) {
                if (extensionIsAvailable(extension, available_extensions))
                        TRACE("\t\t\t⎿ Validated: %s", extension);
                else
                        FATAL("\t\t\t⎿ Failed to Validate: %s", extension);
        }
        TRACE("\t\t⎿ Validated Required Instance Extensions");
        return required_extensions;
}


// Validation Layer helper functions
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
                return func(instance, pCreateInfo, pAllocator, pCallback);
        else
                return VK_ERROR_EXTENSION_NOT_PRESENT;
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
                func(instance, callback, pAllocator);
}
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData, void* /*pUserData*/ ) {
        std::ostringstream message;
        
        message << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>( messageTypes )) << ":\n";
        message << std::string("\t") << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
        message << std::string("\t") << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
        message << std::string("\t") << "message         = <" << pCallbackData->pMessage << ">\n";
        if (0 < pCallbackData->queueLabelCount) {
                message << std::string("\t") << "Queue Labels:\n";
                for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
                        message << std::string("\t\t") << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
        }
        if (0 < pCallbackData->cmdBufLabelCount) {
                message << std::string("\t") << "CommandBuffer Labels:\n";
                for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
                        message << std::string("\t\t") << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
        }
        if (0 < pCallbackData->objectCount) {
                message << std::string("\t") << "Objects:\n";
                for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
                        message << std::string("\t\t") << "Object " << i << "\n";
                        message << std::string("\t\t\t") << "objectType   = " << vk::to_string(static_cast<vk::ObjectType>( pCallbackData->pObjects[i].objectType )) << "\n";
                        message << std::string("\t\t\t") << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
                        if (pCallbackData->pObjects[i].pObjectName)
                                message << std::string("\t\t\t") << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
                }
        }
        
        switch (messageSeverity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                        TRACE(message.str().c_str());
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                        INFO(message.str().c_str());
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                        WARN(message.str().c_str());
                        break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                        ERROR(message.str().c_str());
                        break;
                default:
                        ERROR("Unkown severity: %s", vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>( messageSeverity )).c_str());
                        break;
        }
        return false;
}

bool validationLayerIsAvailable(const char* required_validation_layer, const std::vector<vk::LayerProperties> &available_validation_layers) {
        for (auto validation_layer: available_validation_layers)
                if (strcmp(validation_layer.layerName.data(), required_validation_layer) == 0)
                        return true;
        return false;
}
void checkValidationLayerSupport() {
        TRACE("\t\t⎿ Obtaining Available Validation Layers...");
        std::vector<vk::LayerProperties> available_validation_layers = vk::enumerateInstanceLayerProperties();
        for (auto validation_layer: available_validation_layers) {
                TRACE("\t\t\t⎿ Found: %s", validation_layer.layerName.data());
        }
        TRACE("\t\t⎿ Obtained Available Validation Layers");
        
        TRACE("\t\t⎿ Obtaining Requested Validation Layers...");
        for (auto validation_layer: requested_validation_layers) {
                TRACE("\t\t\t⎿ Requested: %s", validation_layer);
        }
        TRACE("\t\t⎿ Obtained Requested Validation Layers");
        
        TRACE("\t\t⎿ Validating Requested Validation Layers...");
        for (auto validation_layer: requested_validation_layers) {
                if (validationLayerIsAvailable(validation_layer, available_validation_layers))
                        TRACE("\t\t\t⎿ Validated: %s", validation_layer);
                else
                        FATAL("\t\t\t⎿ Failed to Validate: %s", validation_layer);
        }
        TRACE("\t\t⎿ Validated Requested Validation Layers");
}


// Swapchain selection helper functions/structs
vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &available_formats) {
        for (const auto &available_format: available_formats)
                if (available_format.format == vk::Format::eR8G8B8A8Unorm && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                        return available_format;
        return available_formats[0];
}
vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &available_present_modes) {
        bool relaxed_available = false;
        for (const auto &available_present_mode: available_present_modes) {
                if (available_present_mode == vk::PresentModeKHR::eMailbox)
                        return available_present_mode;
                if (available_present_mode == vk::PresentModeKHR::eFifoRelaxed)
                        relaxed_available = true;
        }
        
        if (relaxed_available)
                return vk::PresentModeKHR::eFifoRelaxed;
        else
                return vk::PresentModeKHR::eFifo;
}
vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, GLFWwindow* window) {
        if (capabilities.currentExtent.width != UINT32_MAX)
                return capabilities.currentExtent;
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        vk::Extent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        
        actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        
        return actual_extent;
}
DapperCraft::details::SwapchainSupportDetails DapperCraft::details::RenderEngine::querySwapchainSupport(vk::PhysicalDevice physical_device) {
        DapperCraft::details::SwapchainSupportDetails swapchain_support_details{
                .capabilities = physical_device.getSurfaceCapabilitiesKHR(m_surface),
                .formats = physical_device.getSurfaceFormatsKHR(m_surface),
                .present_modes = physical_device.getSurfacePresentModesKHR(m_surface)
        };
        
        TRACE("\t\t\t⎿ Swapchain Formats: %d", swapchain_support_details.formats.size());
        TRACE("\t\t\t⎿ Swapchain Present Modes: %d", swapchain_support_details.present_modes.size());
        
        return swapchain_support_details;
}


// Physical Device helper functions/structs
bool DapperCraft::details::QueueFamilyIndices::isComplete() const {
        return graphics_family.has_value() && present_family.has_value();
}
DapperCraft::details::QueueFamilyIndices DapperCraft::details::RenderEngine::findQueueFamilies(vk::PhysicalDevice physical_device) {
        QueueFamilyIndices indices;
        auto queue_families = physical_device.getQueueFamilyProperties();
        
        for (int i = 0; const auto &queue_family: queue_families) {
                if (queue_family.queueCount > 0 && queue_family.queueFlags & queue_requirement_flags)
                        indices.graphics_family = i;
                
                if (queue_family.queueCount > 0 && physical_device.getSurfaceSupportKHR(i, m_surface))
                        indices.present_family = i;
                
                if (indices.isComplete())
                        break;
                
                i++;
        }
        INLINE_ASSERT(indices.isComplete(),
                TRACE("\t\t\t⎿ Found Suitable Graphics Family(%d), Found Suitable Present Family(%d)", indices.graphics_family, indices.present_family),
                TRACE("\t\t\t⎿ Failed to Find Suitable Graphics Family or Failed to Find Suitable Present Family")
        )
        
        return indices;
}
uint64_t DapperCraft::details::RenderEngine::rankPhysicalDevice(const vk::PhysicalDevice &physical_device) {
        uint64_t score = 0;
        
        auto physical_device_properties = physical_device.getProperties();
        TRACE("\t\t⎿ Found: %s", physical_device_properties.deviceName.data());
        
        // Returns 0 if no suitable queue family is found
        QueueFamilyIndices indices = findQueueFamilies(physical_device);
        if (!indices.isComplete()) {
                TRACE("\t\t⎿ Device Does Not Have Suitable Queue Family");
                return score;
        }
        
        // Returns 0 if device doesn't have required extensions
        TRACE("\t\t⎿ Obtaining Available Device Extensions");
        auto physical_device_extension_properties = physical_device.enumerateDeviceExtensionProperties();
        for (const auto &extension: physical_device_extension_properties) {
                TRACE("\t\t\t⎿ Found: %s", extension.extensionName);
        }
        TRACE("\t\t⎿ Obtained Available Device Extensions");
        
        TRACE("\t\t⎿ Validating Requested Device Extensions...");
        std::set<std::string> required_device_extensions(additional_device_extensions.begin(), additional_device_extensions.end());
        for (const auto &extension: physical_device_extension_properties) {
                if (required_device_extensions.erase(extension.extensionName) > 0) {
                        TRACE("\t\t\t⎿ Validated: %s", extension.extensionName);
                }
        }
        if (required_device_extensions.empty()) {
                TRACE("\t\t⎿ Validated Requested Device Extensions");
        } else {
                TRACE("\t\t⎿ Device Does Not Have Required Device Extensions");
                return score;
        }
        
        
        // Returns 0 if physical device swapchain is inadequate
        TRACE("\t\t⎿ Validating Swapchain Adequacy...");
        SwapchainSupportDetails swapchain_support_details = querySwapchainSupport(physical_device);
        if (!swapchain_support_details.formats.empty() && !swapchain_support_details.present_modes.empty() && swapchain_support_details.capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eStorage) {
                TRACE("\t\t⎿ Validated Swapchain Adequacy");
        } else {
                TRACE("\t\t⎿ Swapchain is Not Adequate");
                return score;
        }
        
        
        
        // Adds max image dimension to score if everything passes
        score += physical_device_properties.limits.maxImageDimension2D;
        
        // Adds 1,000,000 if device is a dedicated gpu
        if (physical_device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                score += 1'000'000;
        
        TRACE("\t\t\t⎿ Score: %u", score);
        return score;
}


void DapperCraft::details::RenderEngine::init(GLFWwindow* window) {
        TRACE("Initializing Render Engine...");
        
        TRACE("Initializing Vulkan...");
        createInstance();
        createDebugMessenger();
        createSurface(window);
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapchain(window);
        createImageViews();
        createComputePipeline();
        TRACE("Initialized Vulkan...");
        
        TRACE("Initialized Render Engine");
}
DapperCraft::details::RenderEngine::~RenderEngine() {
        TRACE("Destroying Render Engine...");
        
        TRACE("\t⎿ Destroying Vulkan Swapchain Image Views...");
        for (int i = 0; auto image_view: m_swapchain_image_views) {
                m_device.destroy(image_view);
                TRACE("\t\t⎿ Destroyed Vulkan Swapchain Image View #%d", i++);
        }
        TRACE("\t⎿ Destroyed Vulkan Swapchain Image Views");
        
        TRACE("\t⎿ Destroying Vulkan Swapchain...");
        m_device.destroy(m_swapchain);
        TRACE("\t⎿ Destroyed Vulkan Swapchain");
        
        TRACE("\t⎿ Destroying Vulkan Device...");
        m_device.destroy();
        TRACE("\t⎿ Destroyed Vulkan Device");
        
        if (enable_validation_layers) {
                TRACE("\t⎿ Destroying Validation Layers...");
                DestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
                TRACE("\t⎿ Destroyed Validation Layers");
        }
        
        TRACE("\t⎿ Destroying Vulkan Surface...");
        m_instance.destroy(m_surface);
        TRACE("\t⎿ Destroyed Vulkan Surface");
        
        TRACE("\t⎿ Destroying Vulkan Instance...");
        m_instance.destroy();
        TRACE("\t⎿ Destroyed Vulkan Instance");
        
        
        TRACE("Destroyed Render Engine");
        
}

void DapperCraft::details::RenderEngine::createInstance() {
        TRACE("\t⎿ Creating Vulkan Instance...");
        vk::ApplicationInfo application_info{
                .pApplicationName = APP_NAME,
                .applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 0),
                .pEngineName = ENGINE_NAME,
                .engineVersion = VK_MAKE_API_VERSION(0, 1, 3, 0),
                .apiVersion = VK_API_VERSION_1_3
        };
        
        std::vector<const char*> extensions = this->getInstanceExtensions();
        
        vk::InstanceCreateInfo create_info{
                .pApplicationInfo = &application_info,
                .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
                .ppEnabledExtensionNames = extensions.data()
        };
        
        if (enable_validation_layers) {
                TRACE("\t\t⎿ Validation Layers are Enabled...");
                checkValidationLayerSupport();
                create_info.enabledLayerCount = requested_validation_layers.size();
                create_info.ppEnabledLayerNames = requested_validation_layers.data();
        } else {
                TRACE("\t\t⎿ Validation Layers are Disabled..");
                create_info.enabledLayerCount = 0;
        }
        
        INLINE_ASSERT(
                vk::createInstance(&create_info, nullptr, &m_instance) == vk::Result::eSuccess,
                TRACE("\t⎿ Created Vulkan Instance"),
                FATAL("\t⎿ Failed to Create Vulkan Instance")
        )
}
void DapperCraft::details::RenderEngine::createDebugMessenger() {
        if (!enable_validation_layers) return;
        TRACE("\t⎿ Creating Debug Messenger...");
        vk::DebugUtilsMessengerCreateInfoEXT create_info{
                .messageSeverity = severity_flags,
                .messageType = message_type_flags,
                .pfnUserCallback = debugCallback,
                .pUserData = nullptr
        };
        
        INLINE_ASSERT(CreateDebugUtilsMessengerEXT(m_instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&create_info), nullptr, &m_debug_messenger) == VK_SUCCESS,
                TRACE("\t⎿ Created Debug Messenger"),
                FATAL("\t⎿ Failed to Create Debug Messenger")
        )
}
void DapperCraft::details::RenderEngine::createSurface(GLFWwindow* window) {
        TRACE("\t⎿ Creating Vulkan Surface...");
        VkSurfaceKHR raw_surface;
        INLINE_ASSERT(glfwCreateWindowSurface(m_instance, window, nullptr, &raw_surface) == VK_SUCCESS,
                TRACE("\t⎿ Created Vulkan Surface..."),
                FATAL("\t⎿ Failed to Create Vulkan Surface...")
        )
        m_surface = raw_surface;
        
}
void DapperCraft::details::RenderEngine::pickPhysicalDevice() {
        TRACE("\t⎿ Choosing Vulkan Physical Device...");
        
        TRACE("\t⎿ Obtaining Available Vulkan Physical Devices...");
        auto physical_devices = m_instance.enumeratePhysicalDevices();
        if (physical_devices.empty()) {
                TRACE("\t⎿ No Vulkan Physical Devices Found...");
        }
        uint64_t highest_score = 0;
        m_physical_device = physical_devices[0];
        
        TRACE("\t\t⎿ Obtaining Requested Device Extensions...");
        for (const auto &extension: additional_device_extensions) {
                TRACE("\t\t\t⎿ Requested: %s", extension);
        }
        TRACE("\t\t⎿ Obtained Requested Device Extensions...");
        
        for (auto physical_device: physical_devices) {
                uint64_t device_score = rankPhysicalDevice(physical_device);
                if (device_score > highest_score) {
                        highest_score = device_score;
                        m_physical_device = physical_device;
                }
        }
        TRACE("\t⎿ Obtained Available Vulkan Physical Devices...");
        
        TRACE("\t⎿ Choosing Vulkan Physical Device...");
        auto physical_device_properties = m_physical_device.getProperties();
        TRACE("\t⎿ Chose Vulkan Physical Device: %s", physical_device_properties.deviceName.data());
        
        TRACE("\t\t⎿ Determining Device Validity...");
        if (highest_score > 0) {
                TRACE("\t\t⎿ Device Meets Requirements");
        } else {
                FATAL("\t\t⎿ Device Does Not Meet Requirements");
        }
}
void DapperCraft::details::RenderEngine::createLogicalDevice() {
        TRACE("\t⎿ Creating Vulkan Logical Device...");
        TRACE("\t\t⎿ Validating Queues...");
        QueueFamilyIndices indices = findQueueFamilies(m_physical_device);
        TRACE("\t\t⎿ Validated Queues");
        
        std::vector<vk::DeviceQueueCreateInfo> device_queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};
        
        float queue_priority = 0.0f;
        for (uint32_t queue_family_index: unique_queue_families) {
                vk::DeviceQueueCreateInfo device_queue_create_info{
                        .queueFamilyIndex = queue_family_index,
                        .queueCount = 1,
                        .pQueuePriorities = &queue_priority,
                };
                device_queue_create_infos.push_back(device_queue_create_info);
        }
        
        auto device_features = vk::PhysicalDeviceFeatures();
        vk::DeviceCreateInfo device_create_info{
                .queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size()),
                .pQueueCreateInfos = device_queue_create_infos.data(),
                .enabledExtensionCount = static_cast<uint32_t>(additional_device_extensions.size()),
                .ppEnabledExtensionNames = additional_device_extensions.data(),
                .pEnabledFeatures = &device_features,
        };
        
        
        if (enable_validation_layers) {
                device_create_info.enabledLayerCount = requested_validation_layers.size();
                device_create_info.ppEnabledLayerNames = requested_validation_layers.data();
        }
        
        m_device = m_physical_device.createDevice(device_create_info);
        TRACE("\t\t⎿ Initializing Queues...");
        m_graphics_queue = m_device.getQueue(indices.graphics_family.value(), 0);
        m_present_queue = m_device.getQueue(indices.present_family.value(), 0);
        TRACE("\t\t⎿ Initialized Queues");
        
        TRACE("\t⎿ Created Vulkan Logical Device...");
}
void DapperCraft::details::RenderEngine::createSwapchain(GLFWwindow* window) {
        TRACE("\t⎿ Creating Vulkan Swapchain...");
        SwapchainSupportDetails swapchain_support_details = querySwapchainSupport(m_physical_device);
        
        auto surface_format = chooseSwapSurfaceFormat(swapchain_support_details.formats);
        auto surface_present_mode = chooseSwapPresentMode(swapchain_support_details.present_modes);
        auto surface_extent = chooseSwapExtent(swapchain_support_details.capabilities, window);
        m_swapchain_iamge_format = surface_format.format;
        m_swapchain_extent = surface_extent;
        
        uint32_t image_count = swapchain_support_details.capabilities.minImageCount + 1;
        if (swapchain_support_details.capabilities.maxImageCount > 0 && image_count > swapchain_support_details.capabilities.maxImageCount)
                image_count = swapchain_support_details.capabilities.maxImageCount;
        
        vk::SwapchainCreateInfoKHR create_info{
                .surface = m_surface,
                .minImageCount = image_count,
                .imageFormat = surface_format.format,
                .imageColorSpace = surface_format.colorSpace,
                .imageExtent = surface_extent,
                .imageArrayLayers = 1,
                .imageUsage = vk::ImageUsageFlagBits::eStorage,
                .imageSharingMode = vk::SharingMode::eExclusive
        };
        QueueFamilyIndices indices = findQueueFamilies(m_physical_device);
        uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};
        if (indices.graphics_family != indices.present_family) {
                create_info.imageSharingMode = vk::SharingMode::eConcurrent;
                create_info.queueFamilyIndexCount = 2;
                create_info.pQueueFamilyIndices = queue_family_indices;
        }
        
        create_info.preTransform = swapchain_support_details.capabilities.currentTransform;
        create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        create_info.presentMode = surface_present_mode;
        create_info.clipped = VK_TRUE;
        
        create_info.oldSwapchain = VK_NULL_HANDLE;
        
        INLINE_ASSERT(m_device.createSwapchainKHR(&create_info, nullptr, &m_swapchain) == vk::Result::eSuccess,
                TRACE("\t⎿ Created Vulkan Swapchain"),
                FATAL("\t⎿ Failed to Create Vulkan Swapchain")
        )
        
        TRACE("\t⎿ Getting Swapchain Images...");
        m_swapchain_images = m_device.getSwapchainImagesKHR(m_swapchain);
        TRACE("\t⎿ Found %d Swapchain Images", m_swapchain_images.size());
}
void DapperCraft::details::RenderEngine::createImageViews() {
        TRACE("\t⎿ Creating Vulkan Swapchain Image Views...");
        m_swapchain_image_views.resize(m_swapchain_images.size());
        for (int i = 0; const auto &swapchain_image: m_swapchain_images) {
                vk::ImageViewCreateInfo create_info{
                        .image = swapchain_image,
                        .viewType = vk::ImageViewType::e2D,
                        .format = m_swapchain_iamge_format,
                        .components = {
                                .r = vk::ComponentSwizzle::eIdentity,
                                .g = vk::ComponentSwizzle::eIdentity,
                                .b = vk::ComponentSwizzle::eIdentity,
                                .a = vk::ComponentSwizzle::eIdentity
                        },
                        .subresourceRange = {
                                .aspectMask = vk::ImageAspectFlagBits::eColor,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1
                        },
                };
                INLINE_ASSERT(m_device.createImageView(&create_info, nullptr, &m_swapchain_image_views[i++]) == vk::Result::eSuccess,
                        TRACE("\t\t⎿ Created Vulkan Swapchain Image View #%d", i - 1),
                        FATAL("\t\t⎿ Failed to Create Vulkan Swapchain Image View #%d", i - 1)
                )
        }
        TRACE("\t⎿ Created Vulkan Swapchain Image Views...");
}
void DapperCraft::details::RenderEngine::createComputePipeline() {

}
