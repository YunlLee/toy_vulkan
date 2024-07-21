//
// Created by Yun on 2024/7/21.
//

#include "GcCommandBuffer.hpp"
#include <VkDevice.hpp>
#include "VkContent.hpp"

namespace toy
{
    GcCommandBuffer::GcCommandBuffer(VkContent* content, VkDevice* device)
        : content_(content), device_(device){
        createCommandPool();
    }

    GcCommandBuffer::~GcCommandBuffer()
    {
        VK_D(CommandPool, device_->GetDevice(), mCommandPool);
    }

    void GcCommandBuffer::createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = content_->GetQueueFamilyIndices();

        vk::CommandPoolCreateInfo commandPoolInfo;
        commandPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
            .setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.value());

        VK_CREATE(mCommandPool = device_->GetDevice().createCommandPool(commandPoolInfo), "failed to create command pool");
        LOG_T("----------------------");
        LOG_T("{0} : command pool: {1}", __FUNCTION__, (void*)mCommandPool);
    }

    std::vector<vk::CommandBuffer> GcCommandBuffer::allocateCommandBuffers(uint32_t count)
    {
        vk::CommandBufferAllocateInfo allocateInfo;
        allocateInfo.setCommandBufferCount(count)
            .setCommandPool(mCommandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary);

        try
        {
            return device_->GetDevice().allocateCommandBuffers(allocateInfo);
        }catch (vk::SystemError e)
        {
            throw std::runtime_error("failed to allocate command buffer!");
        }
    }
}
