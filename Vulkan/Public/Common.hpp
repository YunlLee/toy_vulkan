//
// Created by Yun on 2024/7/19.
//

#ifndef COMMON_HPP
#define COMMON_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

// Cpp std
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <algorithm>
#include <cstring>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <fstream>
#include <array>


//
#include "VkLog.hpp"
#include "Vertex.hpp"

const std::vector<const char*> validationLayer{
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
    const bool enableValidationLayer = false;
#else
    const bool enableValidationLayer = true;
#endif

#define GC_RES_SHADER_DIR GC_DEFINE_RES_ROOT_DIR"Shader/"

#define VK_CREATE(condation, message) try{\
        condation;\
    }\
    catch (vk::SystemError e){\
        std::runtime_error(message);\
    }

#define VK_D(type, device, handle) if(handle != nullptr){device.destroy##type(handle);}

static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

const std::vector<toy::Vertex> vertices = {
    {{0.0f, -0.5f},{1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

#endif //COMMON_HPP
