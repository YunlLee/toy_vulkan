//
// Created by Yun on 2024/7/25.
//

#ifndef GCDESCRIPTOR_HPP
#define GCDESCRIPTOR_HPP
#include <GcDescriptorSetLayout.hpp>
#include <GcTextureSampler.hpp>
#include <GcVertexBuffer.hpp>

#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class GcDescriptorSetLayout;
    class GcVertexBuffer;
    class GcTextureSampler;

    class GcDescriptor
    {
    public:
        GcDescriptor(VkDevice* device, GcDescriptorSetLayout* layout, GcVertexBuffer* vertexBuffer,
            GcTextureSampler* textureSampler, vk::ImageView& textureImgView);
        ~GcDescriptor();

        [[nodiscard]] std::vector<vk::DescriptorSet> GetDescriptorSet() { return mDescriptorSets; }

    private:
        void createDescriptorPool();
        void createDescriptorSet();

    private:
        vk::DescriptorPool mDescriptorPool;
        std::vector<vk::DescriptorSet> mDescriptorSets;

        vk::ImageView mTextureImageView;

        VkDevice* device_;
        GcDescriptorSetLayout* descSetLayout_;
        GcVertexBuffer* vertexBuffer_;
        GcTextureSampler* textureSampler_;
    };
}

#endif //GCDESCRIPTOR_HPP
