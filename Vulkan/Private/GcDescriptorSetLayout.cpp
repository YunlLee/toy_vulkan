//
// Created by Yun on 2024/7/25.
//

#include "GcDescriptorSetLayout.hpp"
#include "GcVertexBuffer.hpp"

#include <VkDevice.hpp>

namespace toy
{
    GcDescriptorSetLayout::GcDescriptorSetLayout(VkDevice* device)
        :device_(device){
        createDescriptorSet();
    }

    GcDescriptorSetLayout::~GcDescriptorSetLayout()
    {
        VK_D(DescriptorSetLayout, device_->GetDevice(), mDesSetLayout);
    }

    void GcDescriptorSetLayout::createDescriptorSet()
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding;
        uboLayoutBinding.setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eVertex)
            .setPImmutableSamplers(nullptr);

        vk::DescriptorSetLayoutBinding samplerLayoutBinding;
        samplerLayoutBinding.setBinding(1)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setPImmutableSamplers(nullptr)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment);

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.setBindingCount((uint32_t)bindings.size())
            .setPBindings(bindings.data());
        mDesSetLayout = device_->GetDevice().createDescriptorSetLayout(layoutInfo);

        LOG_T("----------------------");
        LOG_T("{0} : DescriptorSetLayout: {1}", __FUNCTION__, (void*)mDesSetLayout);
    }
}
