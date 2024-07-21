//
// Created by Yun on 2024/7/19.
//

#include "VkDevice.hpp"
#include "VkContent.hpp"
#include "VkQueue.hpp"

namespace toy
{
    VkDevice::VkDevice(VkContent* content, uint32_t graphicQueueCount, uint32_t presentQueueCount)
        :content_(content_){
        uint32_t count = 1;
        auto queueCount = std::clamp(count, graphicQueueCount, presentQueueCount);

        float priority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies{content->GetQueueFamilyIndices().graphicsFamily.value(),
            content->GetQueueFamilyIndices().presentFamily.value()};
        for(auto queueFamily : uniqueQueueFamilies)
        {
            vk::DeviceQueueCreateInfo queueInfo;
            queueInfo.setFlags(vk::DeviceQueueCreateFlags())
                .setPNext(nullptr)
                .setQueueCount(queueCount)
                .setQueueFamilyIndex(content->GetQueueFamilyIndices().graphicsFamily.value())
                .setPQueuePriorities(&priority);
            queueCreateInfos.push_back(queueInfo);
        }

        vk::PhysicalDeviceFeatures features{};

        vk::DeviceCreateInfo deviceInfo;
        deviceInfo.setFlags(vk::DeviceCreateFlags())
            .setPNext(nullptr)
            .setQueueCreateInfoCount(queueCount)
            .setQueueCreateInfos(queueCreateInfos)
            .setPEnabledFeatures(&features);
        if(checkDeviceExtensionSupport(content->GetPhysicalDevice()))
        {
            deviceInfo.setEnabledExtensionCount((uint32_t)deviceExtensions.size())
                .setPEnabledExtensionNames(deviceExtensions);
        }

        try
        {
            mHandle = content->GetPhysicalDevice().createDevice(deviceInfo);
        }catch (vk::SystemError e)
        {
            LOG_D("{0} : failed to create device.", __FUNCTION__);
            throw std::runtime_error(e.what());
        }

        LOG_T("----------------------");
        LOG_T("{0} : Logical device: {1}", __FUNCTION__, (void*)mHandle);

        vk::Queue gq = mHandle.getQueue(content->GetQueueFamilyIndices().graphicsFamily.value(), 0);
        for(uint32_t i = 0; i < queueCount; i++)
        {
            auto r = std::make_shared<VkQueue>(content->GetQueueFamilyIndices().graphicsFamily.value(), i, gq, false);
            mGraphicQueues.push_back(r);
        }

        vk::Queue pq = mHandle.getQueue(content->GetQueueFamilyIndices().presentFamily.value(), 0);
        for(uint32_t i = 0; i < queueCount; i++)
        {
            auto r = std::make_shared<VkQueue>(content->GetQueueFamilyIndices().presentFamily.value(), i, pq, true);
            mPresentQueues.push_back(r);
        }
    }

    VkDevice::~VkDevice()
    {
        mHandle.destroy();
    }

    bool VkDevice::checkDeviceExtensionSupport(vk::PhysicalDevice phyDevice)
    {
        auto e = phyDevice.enumerateDeviceExtensionProperties();

        std::set<std::string> requireExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for(const auto& e_ : e)
        {
            requireExtensions.erase(e_.extensionName);
        }
        bool t = requireExtensions.empty();
        return requireExtensions.empty();
    }
}
