//
// Created by Yun on 2024/8/1.
//

#ifndef GCTEXTUREIMAGE_HPP
#define GCTEXTUREIMAGE_HPP

#include <GcCommandBuffer.hpp>
#include <VkContent.hpp>

#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class VkContent;
    class GcCommandBuffer;

    class GcTextureImage
    {
    public:
        GcTextureImage(VkDevice* device, VkContent* content, GcCommandBuffer* commandBuffer);
        ~GcTextureImage();

        [[nodiscard]] vk::Image GetTextureImage() const { return textureImage; }
        [[nodiscard]] vk::DeviceMemory GetTextureImageMemory() const { return textureImageMemory; }

    private:
        void createTextureImage();

        void createImage(uint32_t w, uint32_t h, vk::Format f, vk::ImageTiling t,
            vk::MemoryPropertyFlags property, vk::Image& image, vk::DeviceMemory& imageMemory);

        void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory);

        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        vk::CommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(vk::CommandBuffer c);

        void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

        void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t w, uint32_t h);
    private:

        vk::Image textureImage;
        vk::DeviceMemory textureImageMemory;

        VkDevice* device_;
        VkContent* content_;
        GcCommandBuffer* commandBuffer_;
    };
}

#endif //GCTEXTUREIMAGE_HPP
