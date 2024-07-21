//
// Created by Yun on 2024/7/20.
//

#ifndef GCIMAGEVIEW_HPP
#define GCIMAGEVIEW_HPP

#include <VkSwapchain.hpp>

#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class VkSwapchain;

    class GcImageView
    {
    public:
        GcImageView(VkDevice* device, VkSwapchain* swapchain);
        ~GcImageView();

        [[nodiscard]] std::vector<vk::ImageView> GetImageViews() { return mImageViews; }
        [[nodiscard]] uint32_t GetImageWidth() const { return swapchain_->GetExtent().width; }
        [[nodiscard]] uint32_t GetImageHeight() const { return swapchain_->GetExtent().height; }

    private:
        void createImageView();

    private:
        std::vector<vk::ImageView> mImageViews;

        VkSwapchain* swapchain_;
        VkDevice* device_;
    };
}

#endif //GCIMAGEVIEW_HPP
