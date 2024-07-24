//
// Created by Yun on 2024/7/23.
//

#ifndef GCBUFFER_HPP
#define GCBUFFER_HPP

#include <GcCommandBuffer.hpp>

#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class VkContent;
    class GcCommandBuffer;

    class GcVertexBuffer
    {
    public:
        GcVertexBuffer(VkContent* content, VkDevice* device, GcCommandBuffer* commandBuffer);
        ~GcVertexBuffer();

        [[nodiscard]] vk::Buffer GetVertexBuffer() const { return mHandle; }

    private:
        void createVertexBuffer();
        void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memor);
        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
        void allocateBufferMemory();
        void fillingVertexBuffer(vk::DeviceSize size_);
        void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

    private:
        vk::Buffer mHandle;
        vk::DeviceMemory mVertexBufferMemory;

        VkDevice* device_;
        VkContent* content_;
        GcCommandBuffer* commandBuffer_;
    };
}

#endif //GCBUFFER_HPP
