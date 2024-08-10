//
// Created by Yun on 2024/8/1.
//


#include "GcTextureImage.hpp"

#include <VkQueue.hpp>

#include "VkDevice.hpp"
#include "GcCommandBuffer.hpp"

namespace toy
{
    GcTextureImage::GcTextureImage(VkDevice* device, VkContent* content, GcCommandBuffer* commandBuffer)
        :device_(device), content_(content), commandBuffer_(commandBuffer){
        createTextureImage();
    }

    GcTextureImage::~GcTextureImage()
    {
        device_->GetDevice().destroyImage(textureImage);
        device_->GetDevice().freeMemory(textureImageMemory);
    }

    void GcTextureImage::createTextureImage()
    {
        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;

        int texWidth, texHeight, texChannels;
        // stbi_uc* pixels = stbi_load(GC_RES_TEXTURE_DIR"texture01.jpg",
        //         &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        stbi_uc* pixels = stbi_load(GC_RES_VIKINGROOM_TEX_DIR"viking_room.png",
                &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        vk::DeviceSize imageSize = texWidth * texHeight * 4;

        if(!pixels)
        {
            throw std::runtime_error("failed to load textures image!");
        }

        createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            stagingBuffer, stagingBufferMemory);

        void* data = device_->GetDevice().mapMemory(stagingBufferMemory, 0, imageSize);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        device_->GetDevice().unmapMemory(stagingBufferMemory);
        stbi_image_free(pixels);

        createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
            vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);

        transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal);

        copyBufferToImage(stagingBuffer, textureImage, (uint32_t)texWidth, (uint32_t)texHeight);

        transitionImageLayout(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal);

        device_->GetDevice().destroyBuffer(stagingBuffer);
        device_->GetDevice().freeMemory(stagingBufferMemory);
    }

    void GcTextureImage::createImage(uint32_t w, uint32_t h, vk::Format f, vk::ImageTiling t,
        vk::MemoryPropertyFlags property, vk::Image& image, vk::DeviceMemory& imageMemory)
    {
        vk::ImageCreateInfo imageInfo;
        imageInfo.setImageType(vk::ImageType::e2D)
            .setExtent({w, h, 1})
            .setMipLevels(1)
            .setArrayLayers(1)
            .setFormat(f)
            .setTiling(t)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst)
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

    void GcTextureImage::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                                      vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& memory)
    {
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.setSize(size)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive);
        buffer = device_->GetDevice().createBuffer(bufferInfo);

        vk::MemoryRequirements requirements = device_->GetDevice().getBufferMemoryRequirements(buffer);
        vk::MemoryAllocateInfo memoryInfo;
        memoryInfo.setAllocationSize(requirements.size)
            .setMemoryTypeIndex(findMemoryType(requirements.memoryTypeBits, properties));
        memory = device_->GetDevice().allocateMemory(memoryInfo);

        device_->GetDevice().bindBufferMemory(buffer, memory, 0);
    }

    uint32_t GcTextureImage::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
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

    vk::CommandBuffer GcTextureImage::beginSingleTimeCommands()
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

    void GcTextureImage::endSingleTimeCommands(vk::CommandBuffer c)
    {
        c.end();

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBufferCount(1)
            .setPCommandBuffers(&c);
        device_->GetGraphic()[0]->GetHandle().submit(submitInfo);
        device_->GetGraphic()[0]->GetHandle().waitIdle();

        device_->GetDevice().freeCommandBuffers(commandBuffer_->GetCommandPool(), c);
    }

    void GcTextureImage::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout)
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

    void GcTextureImage::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t w, uint32_t h)
    {
        vk::CommandBuffer c = beginSingleTimeCommands();
        vk::BufferImageCopy region;
        region.setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageSubresource({vk::ImageAspectFlagBits::eColor, 0, 0, 1})
            .setImageOffset({0, 0, 0})
            .setImageExtent({w, h, 1});

        c.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
        endSingleTimeCommands(c);
    }
}
