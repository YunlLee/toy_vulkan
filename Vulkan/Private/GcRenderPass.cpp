//
// Created by Yun on 2024/7/20.
//

#include "GcRenderPass.hpp"

#include <VkDevice.hpp>
#include <VkSwapchain.hpp>

namespace toy
{
    GcRenderPass::GcRenderPass(VkDevice* device, VkSwapchain* swapchain)
        :device_(device), swapchain_(swapchain){
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

        vk::SubpassDescription subpass;
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachmentCount(1)
            .setPColorAttachments(&colorAttachmentRef);

        vk::SubpassDependency dependency;
        dependency.setSrcSubpass(vk::SubpassExternal)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setSrcAccessMask(vk::AccessFlagBits::eNone)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.setAttachmentCount(1)
            .setPAttachments(&colorAttachment)
            .setSubpassCount(1)
            .setPSubpasses(&subpass)
            .setDependencyCount(1)
            .setPDependencies(&dependency);

        VK_CREATE(mHandle = device_->GetDevice().createRenderPass(renderPassInfo), "failed to create render pass");
        LOG_T("----------------------");
        LOG_T("{0} : Logical device: {1}", __FUNCTION__, (void*)mHandle);
    }
}
