//
// Created by Yun on 2024/8/8.
//

#include "GcDepthImage.hpp"

#include <VkContent.hpp>
#include <VkQueue.hpp>

#include "VkSwapchain.hpp"
#include "VkDevice.hpp"
#include "GcCommandBuffer.hpp"

namespace toy
{
    GcDepthImage::GcDepthImage(VkContent* content, VkDevice* device, VkSwapchain* swapchain, GcCommandBuffer* cmdBuffer)
        :content_(content),device_(device), swapchain_(swapchain), commandBuffer_(cmdBuffer){
        createDepthResources();

        LOG_T("{0} depth image: {1}", __FUNCTION__, (void*)depthImage);
        LOG_T("{0} depth memory: {1}", __FUNCTION__, (void*)depthMemory);
        LOG_T("{0} depth image view: {1}", __FUNCTION__, (void*)depthImageView);
    }

    GcDepthImage::~GcDepthImage()
    {
        device_->GetDevice().freeMemory(depthMemory);
        device_->GetDevice().destroyImage(depthImage);
        device_->GetDevice().destroyImageView(depthImageView);
    }

    void GcDepthImage::createDepthResources()
    {
        vk::Format depthFormat = findDepthFormat();

        createImage(swapchain_->GetExtent().width, swapchain_->GetExtent().height, depthFormat, vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal,
            depthImage, depthMemory);

        depthImageView = createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
    }

    bool GcDepthImage::hasStencilComponent(vk::Format format)
    {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }

    vk::Format GcDepthImage::findDepthFormat()
    {
        return findSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}
            ,vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    vk::Format GcDepthImage::findSupportedFormat(const std::vector<vk::Format> candidates, vk::ImageTiling tiling,
                                                 vk::FormatFeatureFlags features)
    {
        for(vk::Format format : candidates)
        {
            vk::FormatProperties props = content_->GetPhysicalDevice().getFormatProperties(format);

            if(tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }else if(tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    void GcDepthImage::createImage(uint32_t w, uint32_t h, vk::Format f, vk::ImageTiling t,
        vk::ImageUsageFlags usage, vk::MemoryPropertyFlags property,
        vk::Image& image, vk::DeviceMemory& imageMemory)
    {
        vk::ImageCreateInfo imageInfo;
        imageInfo.setImageType(vk::ImageType::e2D)
            .setExtent({w, h, 1})
            .setMipLevels(1)
            .setArrayLayers(1)
            .setFormat(f)
            .setTiling(t)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSamples(vk::SampleCountFlagBits::e1);

        VK_CREATE(image = device_->GetDevice().createImage(imageInfo), "failed to create image!");

        vk::MemoryRequirements memRequirements = device_->GetDevice().getImageMemoryRequirements(image);
        vk::MemoryAllocateInfo memoryInfo;
        memoryInfo.setAllocationSize(memRequirements.size)
            .setMemoryTypeIndex(findMemoryType(memRequirements.memoryTypeBits, property));

        VK_CREATE(imageMemory = device_->GetDevice().allocateMemory(memoryInfo), "failed to create image memory!");

        device_->GetDevice().bindImageMemory(image, imageMemory, 0);
    }

    vk::ImageView GcDepthImage::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect)
    {
        vk::ImageView img;
        vk::ImageViewCreateInfo imgViewInfo;
        imgViewInfo.setImage(image)
            .setFormat(format)
            .setViewType(vk::ImageViewType::e2D)
            .setSubresourceRange({aspect, 0, 1, 0, 1});
        img =  device_->GetDevice().createImageView(imgViewInfo);
        return img;
    }

    uint32_t GcDepthImage::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties;
        memProperties = content_->GetPhysicalDevice().getMemoryProperties();
        for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if(typeFilter & ( i << 1) && memProperties.memoryTypes[i].propertyFlags & properties)
            {
                return i;
            }
        }
        throw std::runtime_error("Failed to create suitable memory table.");
    }

    void GcDepthImage::transitionImageLayout(vk::Image image, vk::Format format,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
    {
        vk::CommandBuffer c = beginSingleTimeCommands();
        vk::PipelineStageFlags srcStage;
        vk::PipelineStageFlags dstStage;

        vk::ImageSubresourceRange imageRange;
        imageRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1);

        vk::ImageMemoryBarrier barrier;
        barrier.setNewLayout(newLayout)
            .setOldLayout(oldLayout)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setImage(image)
            .setSubresourceRange(imageRange);

        if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.setSrcAccessMask(vk::AccessFlags())
                .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

            srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
            dstStage = vk::PipelineStageFlagBits::eTransfer;
        }else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

            srcStage = vk::PipelineStageFlagBits::eTransfer;
            dstStage = vk::PipelineStageFlagBits::eFragmentShader;
        }else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        c.pipelineBarrier(srcStage, dstStage,
            vk::DependencyFlags(), nullptr, nullptr, barrier);

        endSingleTimeCommands(c);
    }

    vk::CommandBuffer GcDepthImage::beginSingleTimeCommands()
    {
        vk::CommandBufferAllocateInfo bufferAllocateInfo;
        bufferAllocateInfo.setCommandBufferCount(1)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(commandBuffer_->GetCommandPool());
        vk::CommandBuffer c = device_->GetDevice().allocateCommandBuffers(bufferAllocateInfo)[0];

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        c.begin(beginInfo);

        return c;
    }

    void GcDepthImage::endSingleTimeCommands(vk::CommandBuffer c)
    {
        c.end();
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBufferCount(1)
            .setPCommandBuffers(&c);
        device_->GetGraphic()[0]->GetHandle().submit(submitInfo);
        device_->GetGraphic()[0]->waitIdle();

        device_->GetDevice().freeCommandBuffers(commandBuffer_->GetCommandPool(), c);
    }
}
