//
// Created by Yun on 2024/7/20.
//

#ifndef GCPIPELINE_HPP
#define GCPIPELINE_HPP
#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class VkSwapchain;
    class GcRenderPass;

    class GcPipeline
    {
    public:
        GcPipeline(VkDevice* device, VkSwapchain* swapchain, GcRenderPass* renderPass);
        ~GcPipeline();

        [[nodiscard]] vk::Pipeline GetPipeline() const { return mHandle; }

    private:
        void createGraphicsPipeline();
        void createPipelineLayout();
        vk::ShaderModule createShaderModule(const std::vector<char>& code);

        vk::Pipeline mHandle;
        vk::PipelineLayout mPipelineLayout;

        VkDevice* device_;
        VkSwapchain* swapchain_;
        GcRenderPass* renderPass_;
    };
}

#endif //GCPIPELINE_HPP
