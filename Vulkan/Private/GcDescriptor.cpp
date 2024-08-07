//
// Created by Yun on 2024/7/25.
//


#include "GcDescriptor.hpp"
#include "VkDevice.hpp"
#include "GcDescriptorSetLayout.hpp"
#include "GcVertexBuffer.hpp"
#include "GcTextureSampler.hpp"

namespace toy
{
    GcDescriptor::GcDescriptor(VkDevice* device, GcDescriptorSetLayout* layout, GcVertexBuffer* vertexBuffer, GcTextureSampler* sampler, vk::ImageView& img)
        :device_(device), descSetLayout_(layout), vertexBuffer_(vertexBuffer), textureSampler_(sampler), mTextureImageView(img){
        createDescriptorPool();
        createDescriptorSet();
    }

    GcDescriptor::~GcDescriptor()
    {
        device_->GetDevice().destroyDescriptorPool(mDescriptorPool);
    }

    void GcDescriptor::createDescriptorPool()
    {
        std::array<vk::DescriptorPoolSize, 2> poolSizes;
        poolSizes[0].setType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount((uint32_t)(MAX_FRAMES_IN_FLIGHT));
        poolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount((uint32_t)(MAX_FRAMES_IN_FLIGHT));

        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.setPoolSizeCount((uint32_t)(poolSizes.size()))
            .setPPoolSizes(poolSizes.data())
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

            vk::DescriptorImageInfo imageInfo;
            imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setImageView(mTextureImageView)
                .setSampler(textureSampler_->GetSampler());

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites;
            descriptorWrites[0].setDstSet(mDescriptorSets[i])
                .setDstBinding(0)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1)
                .setPBufferInfo(&bufferInfo);

            descriptorWrites[1].setDstSet(mDescriptorSets[i])
                .setDstBinding(1)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setDescriptorCount(1)
                .setPImageInfo(&imageInfo);

            device_->GetDevice().updateDescriptorSets(descriptorWrites, nullptr);
        }
    }
}