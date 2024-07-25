//
// Created by Yun on 2024/7/25.
//


#include "GcDescriptor.hpp"
#include "VkDevice.hpp"
#include "GcDescriptorSetLayout.hpp"
#include "GcVertexBuffer.hpp"

namespace toy
{
    GcDescriptor::GcDescriptor(VkDevice* device, GcDescriptorSetLayout* layout, GcVertexBuffer* vertexBuffer)
        :device_(device), descSetLayout_(layout), vertexBuffer_(vertexBuffer){
        createDescriptorPool();
        createDescriptorSet();
    }

    GcDescriptor::~GcDescriptor()
    {
        device_->GetDevice().destroyDescriptorPool(mDescriptorPool);
    }

    void GcDescriptor::createDescriptorPool()
    {
        vk::DescriptorPoolSize poolSize;
        poolSize.setDescriptorCount((uint32_t)MAX_FRAMES_IN_FLIGHT);

        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.setPoolSizeCount(1)
            .setPPoolSizes(&poolSize)
            .setMaxSets((uint32_t)MAX_FRAMES_IN_FLIGHT);

        mDescriptorPool = device_->GetDevice().createDescriptorPool(poolInfo);
    }

    void GcDescriptor::createDescriptorSet()
    {
        std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descSetLayout_->GetDescriptorSet());
        vk::DescriptorSetAllocateInfo allocateInfo;
        allocateInfo.setDescriptorPool(mDescriptorPool)
            .setDescriptorSetCount((uint32_t)MAX_FRAMES_IN_FLIGHT)
            .setPSetLayouts(layouts.data());

        mDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

        VK_CREATE(mDescriptorSets = device_->GetDevice().allocateDescriptorSets(allocateInfo),
            "failed to allocate descripter sets!");

        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.setBuffer(vertexBuffer_->GetUniformBuffers()[i])
                .setOffset(0)
                .setRange(sizeof(UniformBufferObject));

            vk::WriteDescriptorSet descriptorWrite;
            descriptorWrite.setDstSet(mDescriptorSets[i])
                .setDstBinding(0)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1)
                .setPBufferInfo(&bufferInfo)
                .setPImageInfo(nullptr)
                .setPTexelBufferView(nullptr);
            device_->GetDevice().updateDescriptorSets(descriptorWrite, nullptr);
        }
    }
}