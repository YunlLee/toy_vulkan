//
// Created by Yun on 2024/7/23.
//

#ifndef GCBUFFER_HPP
#define GCBUFFER_HPP

#include <GcCommandBuffer.hpp>

#include "Common.hpp"

namespace toy
{
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f},{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    class VkDevice;
    class VkContent;
    class GcCommandBuffer;

    class GcVertexBuffer
    {
    public:
        GcVertexBuffer(VkContent* content, VkDevice* device, GcCommandBuffer* commandBuffer);
        ~GcVertexBuffer();

        [[nodiscard]] vk::Buffer GetVertexBuffer() const { return mHandle; }
        [[nodiscard]] vk::Buffer GetIndexBuffer() const { return mIndexBuffer; }
        [[nodiscard]] std::vector<vk::Buffer> GetUniformBuffers() const { return uniformBuffers; }
        [[nodiscard]] std::vector<vk::DeviceMemory> GetUniformBuffersMemory() const { return uniformBuffersMemory; }
        [[nodiscard]] std::vector<void*> GetUniformBuffersMapped() const { return uniformBuffersMapped; }

    private:
        void createVertexBuffer();
        void createIndexBuffer();
        void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memor);
        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
        void allocateBufferMemory();
        void fillingVertexBuffer(vk::DeviceSize size_);
        void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
        void createUniformBuffers();

    private:
        vk::Buffer mHandle;
        vk::DeviceMemory mVertexBufferMemory;
        vk::Buffer mIndexBuffer;
        vk::DeviceMemory mIndexBufferMemory;

        std::vector<vk::Buffer> uniformBuffers;
        std::vector<vk::DeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        VkDevice* device_;
        VkContent* content_;
        GcCommandBuffer* commandBuffer_;
    };
}

#endif //GCBUFFER_HPP
