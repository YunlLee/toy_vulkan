//
// Created by Yun on 2024/8/7.
//

#ifndef GCTEXTURESAMPLER_HPP
#define GCTEXTURESAMPLER_HPP

#include "Common.hpp"

namespace toy
{

    class VkContent;
    class VkDevice;

    class GcTextureSampler
    {
    public:
        GcTextureSampler(VkContent* content, VkDevice* device);
        ~GcTextureSampler();

        [[nodiscard]] vk::Sampler GetSampler() const { return mSampler; }
    private:

        void createTextureSampler();

        vk::Sampler mSampler;

        VkContent* content_;
        VkDevice* device_;
    };
}

#endif //GCTEXTURESAMPLER_HPP
