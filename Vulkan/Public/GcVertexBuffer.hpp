//
// Created by Yun on 2024/7/23.
//

#ifndef GCBUFFER_HPP
#define GCBUFFER_HPP

#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class VkContent;

    class GcVertexBuffer
    {
    public:
        GcVertexBuffer(VkContent* content, VkDevice* device);
        ~GcVertexBuffer();

        [[nodiscard]] vk::Buffer GetVertexBuffer() const { return mHandle; }

    private:
        void createCommandBuffer();
        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
        void allocateBufferMemory();
        void fillingVertexBuffer(vk::DeviceSize size_);

    private:
        vk::Buffer mHandle;
        vk::DeviceMemory mVertexBufferMemory;

        VkDevice* device_;
        VkContent* content_;
    };
}

#endif //GCBUFFER_HPP
