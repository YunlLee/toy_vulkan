//
// Created by Yun on 2024/7/19.
//

#ifndef VKCONTENT_HPP
#define VKCONTENT_HPP

#include "Common.hpp"

namespace toy
{
    class Window;

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class VkContent
    {
    public:
        VkContent(Window* window);
        ~VkContent();

        [[nodiscard]] vk::PhysicalDevice GetPhysicalDevice() const { return mPhyDevice; }
        [[nodiscard]] QueueFamilyIndices GetQueueFamilyIndices() const {return mQueueFamily; }
        [[nodiscard]] vk::SurfaceKHR GetSurface() const { return mSurface; }
        [[nodiscard]] Window* GetWindow() const { return window_; }

    private:
        std::vector<const char*> retrieveExtension();
        bool checkValidationLayerSupport();
        void createInstance(std::vector<const char*> extension);
        void createSurface();
        void pickPhysicalDevice();
        QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice&);
        vk::Bool32 isDeviceSuitable(const vk::PhysicalDevice& pd_);

    private:
        vk::Instance mInstance;
        vk::SurfaceKHR mSurface;
        vk::PhysicalDevice mPhyDevice;

        QueueFamilyIndices mQueueFamily;

        Window* window_;
    };
}

#endif //VKCONTENT_HPP
