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

    class GcRenderPass
    {
    public:
        GcRenderPass(VkDevice* device, VkSwapchain* swapchain);
        ~GcRenderPass();

        [[nodiscard]] vk::RenderPass GetRenderPass() const { return mHandle; }
    private:
        void createRenderPass();

        vk::RenderPass mHandle;

        VkDevice* device_;
        VkSwapchain* swapchain_;
    };
}

#endif //GCRENDERPASS_HPP
