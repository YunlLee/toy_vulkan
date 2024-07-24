//
// Created by Yun on 2024/7/23.
//

#include "GcVertexBuffer.hpp"
#include "VkDevice.hpp"
#include "VkContent.hpp"

namespace toy
{
    GcVertexBuffer::GcVertexBuffer(VkContent* content, VkDevice* device)
        :content_(content), device_(device){
        createCommandBuffer();
    }

    GcVertexBuffer::~GcVertexBuffer()
    {
        VK_D(Buffer, device_->GetDevice(), mHandle);
        device_->GetDevice().freeMemory(mVertexBufferMemory);
    }

    void GcVertexBuffer::createCommandBuffer()
    {
        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.setSize(sizeof(vertices[0]) * vertices.size())
            .setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
            .setSharingMode(vk::SharingMode::eExclusive);

        VK_CREATE(mHandle = device_->GetDevice().createBuffer(bufferCreateInfo), "failed to create buffer!");
        LOG_T("----------------------");
        LOG_T("{0} : buffer : {1}", __FUNCTION__, (void*)mHandle);

        allocateBufferMemory();
        fillingVertexBuffer(bufferCreateInfo.size);
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
