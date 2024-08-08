//
// Created by Yun on 2024/7/20.
//

#include "GcRenderPass.hpp"

#include <VkDevice.hpp>
#include <VkSwapchain.hpp>
#include <VkContent.hpp>

namespace toy
{
    GcRenderPass::GcRenderPass(VkContent* content,VkDevice* device, VkSwapchain* swapchain)
        :content_(content), device_(device), swapchain_(swapchain){
        createRenderPass();
    }

    GcRenderPass::~GcRenderPass()
    {
        VK_D(RenderPass, device_->GetDevice(), mHandle);
    }

    void GcRenderPass::createRenderPass()
    {
        vk::AttachmentDescription colorAttachment;
        colorAttachment.setFormat(swapchain_->GetFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentReference colorAttachmentRef;
        colorAttachmentRef.setAttachment(0)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        vk::AttachmentDescription depthAttachment;
        depthAttachment.setFormat(findDepthFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::AttachmentReference depthAttachmentRef;
        depthAttachmentRef.setAttachment(1)
            .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        vk::SubpassDescription subpass;
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachmentCount(1)
            .setPColorAttachments(&colorAttachmentRef)
            .setPDepthStencilAttachment(&depthAttachmentRef);

        vk::SubpassDependency dependency;
        dependency.setSrcSubpass(vk::SubpassExternal)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests)
            .setSrcAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

        std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.setAttachmentCount((uint32_t)attachments.size())
            .setPAttachments(attachments.data())
            .setSubpassCount(1)
            .setPSubpasses(&subpass)
            .setDependencyCount(1)
            .setPDependencies(&dependency);

        VK_CREATE(mHandle = device_->GetDevice().createRenderPass(renderPassInfo), "failed to create render pass");
        LOG_T("----------------------");
        LOG_T("{0} : Logical device: {1}", __FUNCTION__, (void*)mHandle);
    }

    vk::Format GcRenderPass::findDepthFormat()
    {
        return findSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}
             ,vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    vk::Format GcRenderPass::findSupportedFormat(const std::vector<vk::Format> candidates, vk::ImageTiling tiling,
        vk::FormatFeatureFlags features)
    {
        for(vk::Format format : candidates)
        {
            vk::FormatProperties props = content_->GetPhysicalDevice().getFormatProperties(format);

            if(tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }else if(tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }
}
