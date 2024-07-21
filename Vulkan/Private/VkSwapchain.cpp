//
// Created by Yun on 2024/7/19.
//

#include "VkSwapchain.hpp"

#include <VkDevice.hpp>
#include <Window.hpp>

#include "VkContent.hpp"

namespace toy
{
    VkSwapchain::VkSwapchain(VkContent* content, VkDevice* device)
        :content_(content), device_(device){
        mSwapchainDetails = querySwapchainSupport();
        createSwapchain();
    }

    VkSwapchain::~VkSwapchain()
    {
        device_->GetDevice().destroySwapchainKHR(mHandle);
    }

    SwapchainSupportDetails VkSwapchain::querySwapchainSupport()
    {
        SwapchainSupportDetails details;

        // basic surface capabilities
        details.capabilities = content_->GetPhysicalDevice().getSurfaceCapabilitiesKHR(content_->GetSurface());

        // querying the supported surface formats
        details.formats = content_->GetPhysicalDevice().getSurfaceFormatsKHR(content_->GetSurface());

        // querying the supported presentation modes
        details.presentModes = content_->GetPhysicalDevice().getSurfacePresentModesKHR(content_->GetSurface());

        return details;
    }

    vk::SurfaceFormatKHR VkSwapchain::chooseSwapSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR>& availableFormats)
    {
        for(const auto& availableFormat : availableFormats)
        {
            if(availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
                availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    vk::PresentModeKHR VkSwapchain::chooseSwapSurfaceMode(
        const std::vector<vk::PresentModeKHR>& availablesPresentModes)
    {
        for(const auto& availablesPresentMode : availablesPresentModes)
        {
            if(availablesPresentMode == vk::PresentModeKHR::eMailbox)
            {
                return availablesPresentMode;
            }
        }
        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D VkSwapchain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }else
        {
            int w, h;
            glfwGetFramebufferSize(content_->GetWindow()->GetHandle(), &w, &h);

            vk::Extent2D actualExtent{(uint32_t)w, (uint32_t)h};

            actualExtent.width = std::clamp(actualExtent.width,
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height,
                capabilities.minImageExtent.height,
                capabilities.maxImageExtent.height);
            return actualExtent;
        }
    }

    void VkSwapchain::createSwapchain()
    {
        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(mSwapchainDetails.formats);
        swapchainFormat = surfaceFormat.format;

        vk::PresentModeKHR presentMode = chooseSwapSurfaceMode(mSwapchainDetails.presentModes);

        vk::Extent2D extent = chooseSwapExtent(mSwapchainDetails.capabilities);
        swapchainExtent = extent;

        uint32_t count = 3;
        uint32_t imageCount = std::clamp(count, mSwapchainDetails.capabilities.minImageCount, mSwapchainDetails.capabilities.maxImageCount);

        uint32_t queueFamilyIndices[] = {content_->GetQueueFamilyIndices().presentFamily.value(),
            content_->GetQueueFamilyIndices().graphicsFamily.value()};

        vk::SwapchainCreateInfoKHR swapchainInfo;
        swapchainInfo.setSurface(content_->GetSurface())
            .setMinImageCount(imageCount)
            .setImageFormat(surfaceFormat.format)
            .setImageColorSpace(surfaceFormat.colorSpace)
            .setImageExtent(extent)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setPreTransform(mSwapchainDetails.capabilities.currentTransform)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPresentMode(presentMode)
            .setClipped(true)
            .setOldSwapchain(VK_NULL_HANDLE);

        if(content_->GetQueueFamilyIndices().graphicsFamily != content_->GetQueueFamilyIndices().presentFamily)
        {
            swapchainInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
                .setQueueFamilyIndexCount(2)
                .setPQueueFamilyIndices(queueFamilyIndices);
        }else
        {
            swapchainInfo.setImageSharingMode(vk::SharingMode::eExclusive)
                .setQueueFamilyIndexCount(0)
                .setPQueueFamilyIndices(nullptr);
        }

        try
        {
            mHandle = device_->GetDevice().createSwapchainKHR(swapchainInfo);
        }catch (vk::SystemError e)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        mImages = device_->GetDevice().getSwapchainImagesKHR(mHandle);

        LOG_T("----------------------");
        LOG_T("{0} : Swapchain: {1}", __FUNCTION__, (void*)mHandle);
        LOG_T("{0} : Images array: {1}", __FUNCTION__, mImages.size());
    }

    uint32_t VkSwapchain::getNextImageIndex(vk::Semaphore semaphore)
    {
        auto r = device_->GetDevice().acquireNextImageKHR(mHandle, UINT64_MAX, semaphore, nullptr);
        return r.value;
    }
}
