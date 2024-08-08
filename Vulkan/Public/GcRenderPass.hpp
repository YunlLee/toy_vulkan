//
// Created by Yun on 2024/7/20.
//

#ifndef GCRENDERPASS_HPP
#define GCRENDERPASS_HPP

#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class VkSwapchain;
    class VkContent;

    class GcRenderPass
    {
    public:
        GcRenderPass(VkContent* content, VkDevice* device, VkSwapchain* swapchain);
        ~GcRenderPass();

        [[nodiscard]] vk::RenderPass GetRenderPass() const { return mHandle; }

        vk::Format findDepthFormat();
        vk::Format findSupportedFormat(const std::vector<vk::Format> candidates,
            vk::ImageTiling tiling, vk::FormatFeatureFlags features);
    private:
        void createRenderPass();

        vk::RenderPass mHandle;

        VkDevice* device_;
        VkSwapchain* swapchain_;
        VkContent* content_;
    };
}

#endif //GCRENDERPASS_HPP
