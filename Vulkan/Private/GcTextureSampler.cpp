//
// Created by Yun on 2024/8/7.
//

#include "GcTextureSampler.hpp"
#include "VkContent.hpp"
#include "VkDevice.hpp"

namespace toy
{
    GcTextureSampler::GcTextureSampler(VkContent* content, VkDevice* device)
        :content_(content), device_(device){
        createTextureSampler();
    }

    GcTextureSampler::~GcTextureSampler()
    {
        VK_D(Sampler, device_->GetDevice(), mSampler);
    }

    void GcTextureSampler::createTextureSampler()
    {
        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setAddressModeU(vk::SamplerAddressMode::eRepeat)
            .setAddressModeV(vk::SamplerAddressMode::eRepeat)
            .setAddressModeW(vk::SamplerAddressMode::eRepeat);
        vk::PhysicalDeviceProperties properties = content_->GetPhysicalDevice().getProperties();
        samplerInfo.setAnisotropyEnable(VK_TRUE)
            .setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
            .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
            .setUnnormalizedCoordinates(VK_FALSE)
            .setCompareEnable(VK_FALSE)
            .setCompareOp(vk::CompareOp::eAlways)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setMipLodBias(0.0f)
            .setMinLod(0.0f)
            .setMaxLod(0.0f);

        VK_CREATE(mSampler = device_->GetDevice().createSampler(samplerInfo), "failed to create texture sampler!");
    }
}
