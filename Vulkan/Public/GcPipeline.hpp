//
// Created by Yun on 2024/7/20.
//

#ifndef GCPIPELINE_HPP
#define GCPIPELINE_HPP
#include <GcDescriptorSetLayout.hpp>

#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class VkSwapchain;
    class GcRenderPass;
    class GcDescriptorSetLayout;

    class GcPipeline
    {
    public:
        GcPipeline(VkDevice* device, VkSwapchain* swapchain, GcRenderPass* renderPass, GcDescriptorSetLayout* dsl);
        ~GcPipeline();

        [[nodiscard]] vk::Pipeline GetPipeline() const { return mHandle; }
        [[nodiscard]] vk::PipelineLayout GetPipelineLayout() const { return mPipelineLayout; }

    private:
        void createGraphicsPipeline();
        void createPipelineLayout();
        vk::ShaderModule createShaderModule(const std::vector<char>& code);

        vk::Pipeline mHandle;
        vk::PipelineLayout mPipelineLayout;

        VkDevice* device_;
        VkSwapchain* swapchain_;
        GcRenderPass* renderPass_;
        GcDescriptorSetLayout* descriptorSetLayout_;
    };
}

#endif //GCPIPELINE_HPP
