//
// Created by Yun on 2024/7/20.
//

#include "GcFramebuffer.hpp"
#include "VkDevice.hpp"
#include "GcRenderPass.hpp"
#include "GcImageView.hpp"
#include "GcDepthImage.hpp"

namespace toy
{
    GcFramebuffer::GcFramebuffer(VkDevice* device, GcImageView* imageView, GcRenderPass* renderPass, GcDepthImage* depthImage)
        :device_(device), imageView_(imageView), renderPass_(renderPass), depthImage_(depthImage){
        createFramebufer();
    }

    GcFramebuffer::~GcFramebuffer()
    {
        for(auto& framebuffer : mFramebuffers)
        {
            VK_D(Framebuffer, device_->GetDevice(), framebuffer);
        }
    }

    void GcFramebuffer::createFramebufer()
    {
        mFramebuffers.resize(imageView_->GetImageViews().size());

        LOG_T("----------------------");


        for(int i = 0; i < imageView_->GetImageViews().size(); i++)
        {
            std::array<vk::ImageView, 2> attachments = {
                imageView_->GetImageViews()[i],
                depthImage_->GetImageView()
            };
            vk::FramebufferCreateInfo framebufferInfo;
            framebufferInfo.setRenderPass(renderPass_->GetRenderPass())
                .setAttachmentCount((uint32_t)attachments.size())
                .setPAttachments(attachments.data())
                .setWidth(imageView_->GetImageWidth())
                .setHeight(imageView_->GetImageHeight())
                .setLayers(1);

            VK_CREATE(mFramebuffers[i] = device_->GetDevice().createFramebuffer(framebufferInfo), "failed to create framebuffer");
            LOG_T("{0} : Logical device: {1}", __FUNCTION__, (void*)mFramebuffers[i]);
        }
    }
}
