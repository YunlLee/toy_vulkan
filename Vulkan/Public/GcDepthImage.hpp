//
// Created by Yun on 2024/8/8.
//

#ifndef GCDEPTHIMAGE_HPP
#define GCDEPTHIMAGE_HPP

#include <GcCommandBuffer.hpp>
#include <VkSwapchain.hpp>

#include "Common.hpp"
#include "VkDevice.hpp"

namespace toy
{
    class VkContent;
    class VkDevice;
    class VkSwapchain;
    class GcCommandBuffer;
    class GcDepthImage;

    class GcDepthImage
    {
    public:
        GcDepthImage(VkContent* content,VkDevice* device, VkSwapchain* swapchain, GcCommandBuffer* commandBuffer);
        ~GcDepthImage();

        void createDepthResources();

        [[nodiscard]] vk::ImageView GetImageView() const { return depthImageView; }
    private:
        bool hasStencilComponent(vk::Format format);

        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        vk::Format findDepthFormat();

        void transitionImageLayout(vk::Image img, vk::Format f, vk::ImageLayout o, vk::ImageLayout n);
        vk::CommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(vk::CommandBuffer c);

        vk::Format findSupportedFormat(const std::vector<vk::Format> candidates,
            vk::ImageTiling tiling, vk::FormatFeatureFlags features);

        void createImage(uint32_t w, uint32_t h, vk::Format f, vk::ImageTiling t,
        vk::ImageUsageFlags usage, vk::MemoryPropertyFlags property,
        vk::Image& image, vk::DeviceMemory& imageMemory);

        vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect);

    private:
        vk::Image depthImage;
        vk::DeviceMemory depthMemory;
        vk::ImageView depthImageView;

        VkContent* content_;
        VkSwapchain* swapchain_;
        VkDevice* device_;
        GcCommandBuffer* commandBuffer_;
    };
}

#endif //GCDEPTHIMAGE_HPP
