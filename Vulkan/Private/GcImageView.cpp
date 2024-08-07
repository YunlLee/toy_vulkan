//
// Created by Yun on 2024/7/20.
//

#include "GcImageView.hpp"

#include <VkDevice.hpp>
#include <VkSwapchain.hpp>

namespace toy
{
    GcImageView::GcImageView(VkDevice* device, VkSwapchain* swapchain)
        : device_(device), swapchain_(swapchain){
        createImageView();
    }

    GcImageView::~GcImageView()
    {
        for(auto& e : mImageViews)
        {
            device_->GetDevice().destroyImageView(e);
        }
    }

    void GcImageView::createImageView()
    {
        mImageViews.resize(swapchain_->GetImageArray().size());

        for(int i = 0; i < swapchain_->GetImageArray().size(); i++)
        {
            vk::ImageViewCreateInfo imageViewInfo;
            imageViewInfo.setImage(swapchain_->GetImageArray()[i])
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(swapchain_->GetFormat())
                .setComponents(vk::ComponentMapping{
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity})
                .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor,
                    0, 1, 0, 1});
            try
            {
                mImageViews[i] = device_->GetDevice().createImageView(imageViewInfo);
            }catch (vk::SystemError e)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
        LOG_T("{0} : Image view array: {1}", __FUNCTION__, mImageViews.size());

    }

    vk::ImageView GcImageView::createTextureImageView(vk::Image img)
    {
        vk::ImageView imgView;
        vk::ImageViewCreateInfo textureImageViewInfo;
        textureImageViewInfo.setImage(img)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(vk::Format::eR8G8B8A8Srgb)
            .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor,
                0, 1, 0, 1
            });
        VK_CREATE(imgView = device_->GetDevice().createImageView(textureImageViewInfo), "failed to create texture image view!");
        return imgView;
    }
}
