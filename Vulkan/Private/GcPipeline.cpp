//
// Created by Yun on 2024/7/20.
//

#include "GcPipeline.hpp"
#include "VkDevice.hpp"
#include "VkSwapchain.hpp"
#include "GcRenderPass.hpp"

#include <Common.hpp>
#include <GcPipeline.hpp>

namespace toy
{
    GcPipeline::GcPipeline(VkDevice* device, VkSwapchain* swapchain, GcRenderPass* renderPass)
        :device_(device), swapchain_(swapchain), renderPass_(renderPass){
        createPipelineLayout();
        createGraphicsPipeline();
    }

    GcPipeline::~GcPipeline()
    {
        VK_D(Pipeline, device_->GetDevice(), mHandle);
        VK_D(PipelineLayout, device_->GetDevice(), mPipelineLayout);
    }

    vk::ShaderModule GcPipeline::createShaderModule(const std::vector<char>& code)
    {
        vk::ShaderModuleCreateInfo moduleInfo;
        moduleInfo.setCodeSize(code.size())
            .setPCode(reinterpret_cast<const uint32_t*>(code.data()));

        try
        {
           return device_->GetDevice().createShaderModule(moduleInfo);
        }catch (vk::SystemError e)
        {
            throw std::runtime_error("failed to create shader module!");
        }
    }

    void GcPipeline::createPipelineLayout()
    {
        vk::PipelineLayoutCreateInfo createInfo;
        createInfo.setSetLayoutCount(0)
            .setPSetLayouts(nullptr)
            .setPushConstantRangeCount(0)
            .setPPushConstantRanges(nullptr);

        VK_CREATE(mPipelineLayout = device_->GetDevice().createPipelineLayout(createInfo),"failed to create pipeline layout!");
        LOG_T("{0} : Pipeline layout: {1}", __FUNCTION__, (void*)mPipelineLayout);
    }

    void GcPipeline::createGraphicsPipeline()
    {
        // programmable
        // shader module
        auto vertexShaderCode = readFile(GC_RES_SHADER_DIR"triangle.vert.spv");
        auto fragmentShaderCode = readFile(GC_RES_SHADER_DIR"triangle.frag.spv");

        vk::ShaderModule mVertModule = createShaderModule(vertexShaderCode);
        vk::ShaderModule mFragModule = createShaderModule(fragmentShaderCode);

        vk::PipelineShaderStageCreateInfo vertShaderInfo;
        vertShaderInfo.setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(mVertModule)
            .setPName("main");
        vk::PipelineShaderStageCreateInfo fragShaderInfo;
        fragShaderInfo.setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(mFragModule)
            .setPName("main");
        vk::PipelineShaderStageCreateInfo shaderStages[]{ vertShaderInfo, fragShaderInfo};

        // fixed-function stage
        // pipeline dynamic state
        std::vector<vk::DynamicState> dynamicStates{ vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicState;
        dynamicState.setDynamicStateCount((uint32_t)dynamicStates.size())
            .setPDynamicStates(dynamicStates.data());


        // Vertex input
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescription = Vertex::getAttributeDescriptions();

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.setVertexBindingDescriptionCount(1)
            .setPVertexBindingDescriptions(&bindingDescription)
            .setVertexAttributeDescriptionCount((uint32_t)attributeDescription.size())
            .setPVertexAttributeDescriptions(attributeDescription.data());

        // Input assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState;
        inputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList)
            .setPrimitiveRestartEnable(false);

        // viewport & scissor
        vk::Viewport viewport{0.0f, 0.0f,
            (float)swapchain_->GetExtent().width, (float)swapchain_->GetExtent().height,
            0.0f, 1.0f};
        vk::Rect2D scissor{{0, 0}, swapchain_->GetExtent()};

        vk::PipelineViewportStateCreateInfo viewportStageInfo;
        viewportStageInfo.setViewportCount(1)
            .setPViewports(&viewport)
            .setScissorCount(1)
            .setPScissors(&scissor);

        // Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterizationStageInfo;
        rasterizationStageInfo.setDepthClampEnable(false)
            .setRasterizerDiscardEnable(false)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setLineWidth(1.0f)
            .setCullMode(vk::CullModeFlagBits::eBack)
            .setFrontFace(vk::FrontFace::eClockwise)
            .setDepthBiasEnable(false);

        // multisampling
        vk::PipelineMultisampleStateCreateInfo multisampleStateInfo;
        multisampleStateInfo.setSampleShadingEnable(false)
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setMinSampleShading(1.0f)
            .setPSampleMask(nullptr)
            .setAlphaToCoverageEnable(false)
            .setAlphaToOneEnable(false);

        // depth & stencil
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;

        // color blend
        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(false)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);

        vk::PipelineColorBlendStateCreateInfo colorBlending;
        colorBlending.setLogicOpEnable(false)
            .setLogicOp(vk::LogicOp::eCopy)
            .setAttachmentCount(1)
            .setPAttachments(&colorBlendAttachment)
            .setBlendConstants(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f});


        vk::GraphicsPipelineCreateInfo pipelineInfo;
        pipelineInfo.setStageCount(2)
            .setPStages(shaderStages)
            .setPVertexInputState(&vertexInputInfo)
            .setPInputAssemblyState(&inputAssemblyState)
            .setPViewportState(&viewportStageInfo)
            .setPRasterizationState(&rasterizationStageInfo)
            .setPMultisampleState(&multisampleStateInfo)
            .setPDepthStencilState(nullptr)
            .setPColorBlendState(&colorBlending)
            .setPDynamicState(&dynamicState);
        pipelineInfo.setLayout(mPipelineLayout)
            .setRenderPass(renderPass_->GetRenderPass())
            .setSubpass(0);

        auto r = device_->GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
        if(r.result != vk::Result::eSuccess)
        {
            throw std::runtime_error("failed to create pipeline");
        }
        mHandle = r.value;
        LOG_T("{0} : Graphics pipeline: {1}", __FUNCTION__, (void*)mHandle);

        device_->GetDevice().destroyShaderModule(mVertModule);
        device_->GetDevice().destroyShaderModule(mFragModule);
    }

}
