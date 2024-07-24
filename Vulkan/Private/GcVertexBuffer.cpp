//
// Created by Yun on 2024/7/23.
//

#include "GcVertexBuffer.hpp"

#include <VkQueue.hpp>

#include "VkDevice.hpp"
#include "VkContent.hpp"
#include "GcCommandBuffer.hpp"

namespace toy
{

    GcVertexBuffer::GcVertexBuffer(VkContent* content, VkDevice* device, GcCommandBuffer* commandBuffer)
        :content_(content), device_(device), commandBuffer_(commandBuffer){
        createVertexBuffer();
        createIndexBuffer();
    }

    GcVertexBuffer::~GcVertexBuffer()
    {
        VK_D(Buffer, device_->GetDevice(), mHandle);
        device_->GetDevice().freeMemory(mVertexBufferMemory);
        VK_D(Buffer, device_->GetDevice(), mIndexBuffer);
        device_->GetDevice().freeMemory(mIndexBufferMemory);
    }

    void GcVertexBuffer::createVertexBuffer()
    {
        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            stagingBuffer, stagingBufferMemory);

        void* data = device_->GetDevice().mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        device_->GetDevice().unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize,
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            mHandle, mVertexBufferMemory);

        copyBuffer(stagingBuffer, mHandle, bufferSize);

        device_->GetDevice().destroyBuffer(stagingBuffer);
        device_->GetDevice().freeMemory(stagingBufferMemory);
        LOG_T("----------------------");
        LOG_T("{0} : buffer : {1}", __FUNCTION__, (void*)mHandle);

    }

    void GcVertexBuffer::createIndexBuffer()
    {
        vk::DeviceSize size = sizeof(indices[0]) * indices.size();
        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            stagingBuffer, stagingBufferMemory);

        void* data = device_->GetDevice().mapMemory(stagingBufferMemory, 0, size);
        memcpy(data, indices.data(), (size_t)size);
        device_->GetDevice().unmapMemory(stagingBufferMemory);

        createBuffer(size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
            vk::MemoryPropertyFlagBits::eDeviceLocal, mIndexBuffer, mIndexBufferMemory);

        copyBuffer(stagingBuffer, mIndexBuffer, size);

        device_->GetDevice().destroyBuffer(stagingBuffer);
        device_->GetDevice().freeMemory(stagingBufferMemory);
    }

    void GcVertexBuffer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
            vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.setSize(size)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive);
        buffer = device_->GetDevice().createBuffer(bufferInfo);

        vk::MemoryRequirements memoryRequirements = device_->GetDevice().getBufferMemoryRequirements(buffer);
        vk::MemoryAllocateInfo allocateInfo;
        allocateInfo.setAllocationSize(memoryRequirements.size)
            .setMemoryTypeIndex(findMemoryType(memoryRequirements.memoryTypeBits, properties));
        bufferMemory = device_->GetDevice().allocateMemory(allocateInfo);

        device_->GetDevice().bindBufferMemory(buffer, bufferMemory, 0);
    }

    void GcVertexBuffer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
    {
        vk::CommandBufferAllocateInfo allocateInfo;
        allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(commandBuffer_->GetCommandPool())
            .setCommandBufferCount(1);

        auto cmdBuffer = device_->GetDevice().allocateCommandBuffers(allocateInfo)[0];

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuffer.begin(beginInfo);

        vk::BufferCopy copyRegion;
        copyRegion.setSrcOffset(0)
            .setDstOffset(0)
            .setSize(size);
        cmdBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

        cmdBuffer.end();

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBufferCount(1)
            .setPCommandBuffers(&cmdBuffer);
        device_->GetGraphic()[0]->GetHandle().submit(submitInfo);
        device_->GetGraphic()[0]->GetHandle().waitIdle();

        device_->GetDevice().freeCommandBuffers(commandBuffer_->GetCommandPool(), cmdBuffer);
    }

    /**
     * We may have more than one desirable property, so we should check if the result of the
     * bitwise AND is not just-zero, but equal to the desired properties bit field.
     * If there is a memory type suitable fot the buffer that also has all of the properties
     * we need, then we return its index, otherwise we throw an exception.
     *
     * @param typeFilter be used to specify the bit field of memory types that are suitable.
     * @param properties special features of the memory
     * @return memory index
     */
    uint32_t GcVertexBuffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties;
        memProperties = content_->GetPhysicalDevice().getMemoryProperties();

        for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if(typeFilter & (i << 1) && memProperties.memoryTypes[i].propertyFlags & properties)
            {
                return i;
            }
        }
        throw std::runtime_error("failed to create suitable memory type!");
    }


    /**
     * Allocate memory for vk::Buffer.
     */
    void GcVertexBuffer::allocateBufferMemory()
    {
        vk::MemoryRequirements memRequirements;
        memRequirements = device_->GetDevice().getBufferMemoryRequirements(mHandle);

        vk::MemoryAllocateInfo allocateInfo;
        allocateInfo.setAllocationSize(memRequirements.size);
        allocateInfo.setMemoryTypeIndex(findMemoryType(memRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        VK_CREATE(mVertexBufferMemory = device_->GetDevice().allocateMemory(allocateInfo), "failed to allocate memory.");
        LOG_T("{0} : buffer : {1}", __FUNCTION__, (void*)mVertexBufferMemory);

        // to associate this memory with the buffer
        device_->GetDevice().bindBufferMemory(mHandle, mVertexBufferMemory, 0);
        LOG_T("Binding this memroy with the buffer.");

    }

    void GcVertexBuffer::fillingVertexBuffer(vk::DeviceSize size_)
    {
        void* data = device_->GetDevice().mapMemory(mVertexBufferMemory, 0, size_);
        memcpy(data, vertices.data(), (size_t)size_);
        device_->GetDevice().unmapMemory(mVertexBufferMemory);
    }


}
