#pragma once
const char* ENGINE_NAME = "She could never be brickmapping... Holy shi-";
const char* APP_NAME = "BrickCraft";

const std::vector<const char*> additional_instance_extensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
};
const std::vector<const char*> additional_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

const bool enable_validation_layers = true;
const std::vector<const char*> requested_validation_layers = {
        "VK_LAYER_KHRONOS_validation",
};
vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
vk::DebugUtilsMessageTypeFlagsEXT message_type_flags (vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

const auto queue_requirement_flags = vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eGraphics;