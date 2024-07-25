//
// Created by Yun on 2024/7/25.
//

#ifndef GCDESCRIPTORSET_HPP
#define GCDESCRIPTORSET_HPP

#include "Common.hpp"

namespace toy
{
    class VkDevice;

    class GcDescriptorSetLayout
    {
    public:
        GcDescriptorSetLayout(VkDevice* device);
        ~GcDescriptorSetLayout();

        [[nodiscard]] vk::DescriptorSetLayout GetDescriptorSet() const { return mDesSetLayout; }

    private:
        void createDescriptorSet();

        vk::DescriptorSetLayout mDesSetLayout;

        VkDevice* device_;
    };
}

#endif //GCDESCRIPTORSET_HPP
