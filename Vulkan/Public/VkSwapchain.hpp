//
// Created by Yun on 2024/7/19.
//

#ifndef VKSWAPCHAIN_HPP
#define VKSWAPCHAIN_HPP

#include "Common.hpp"

namespace toy
{
    class VkContent;
    class VkDevice;

    struct SwapchainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    class VkSwapchain
    {
    public:
        VkSwapchain(VkContent* content, VkDevice* device);
        ~VkSwapchain();

        [[nodiscard]] vk::SwapchainKHR GetSwapchain() const { return mHandle; }
        [[nodiscard]] std::vector<vk::Image> GetImageArray() const { return mImages; }
        [[nodiscard]] vk::Extent2D GetExtent() const { return swapchainExtent; }
        [[nodiscard]] vk::Format GetFormat() const { return swapchainFormat;}

        vk::ResultValue<unsigned int> getNextImageIndex(vk::Semaphore semaphore);

        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availablesFormats);
        vk::PresentModeKHR chooseSwapSurfaceMode(const std::vector<vk::PresentModeKHR>& availablesPresentModes);
        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR & capabilities);
    private:
        SwapchainSupportDetails querySwapchainSupport();
        void createSwapchain();

    private:

        vk::SwapchainKHR mHandle;
        std::vector<vk::Image> mImages;

        SwapchainSupportDetails mSwapchainDetails;
        vk::Format swapchainFormat;
        vk::Extent2D swapchainExtent;

        VkContent* content_;
        VkDevice* device_;
    };
}

#endif //VKSWAPCHAIN_HPP
