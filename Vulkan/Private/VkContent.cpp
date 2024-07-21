//
// Created by Yun on 2024/7/19.
//

#include "VkContent.hpp"
#include "Window.hpp"

namespace toy
{
    VkContent::VkContent(Window* window)
        :window_(window){
        auto extensions = retrieveExtension();
        createInstance(extensions);
        createSurface();
        pickPhysicalDevice();
    }

    VkContent::~VkContent()
    {
        mInstance.destroySurfaceKHR(mSurface);
        mInstance.destroy();
    }

    static VkBool32 vkDebugReportCallbackEXT(
        VkDebugReportFlagsEXT                       flags,
        VkDebugReportObjectTypeEXT                  objectType,
        uint64_t                                    object,
        size_t                                      location,
        int32_t                                     messageCode,
        const char*                                 pLayerPrefix,
        const char*                                 pMessage,
        void*                                       pUserData)
    {
        if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            LOG_E("{0}", pMessage);
        }
        if(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT || VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            LOG_W("{0}", pMessage);
        }
        return VK_FALSE;
    }

    std::vector<const char*> VkContent::retrieveExtension()
    {
        uint32_t glfwExtensionCount;
        auto ex = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(ex, ex + glfwExtensionCount);
        if(enableValidationLayer)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        auto v = vk::enumerateInstanceExtensionProperties(nullptr);

        auto new_end = std::remove_if(extensions.begin(), extensions.end(), [&v](const char* ext)
        {
            return std::find_if(v.begin(), v.end(),
                [&ext](const vk::ExtensionProperties& ep)
                {
                    return strcmp(ep.extensionName, ext) == 0;
                }) == v.end();
        });
        extensions.erase(new_end, extensions.end());
        return extensions;
    }

    bool VkContent::checkValidationLayerSupport()
    {
        auto l = vk::enumerateInstanceLayerProperties();
        for(const auto& layerName : validationLayer)
        {
            bool usage = false;
            for(const auto& lp : l)
            {
                if(strcmp(layerName, lp.layerName) == 0)
                {
                    usage = true;
                    break;
                }
            }
            if(!usage)
            {
                return false;
            }
        }
        return true;
    }

    void VkContent::createInstance(std::vector<const char*> extension)
    {
        vk::DebugReportCallbackCreateInfoEXT debugInfo;
        debugInfo.setFlags(vk::DebugReportFlagBitsEXT::eWarning
            | vk::DebugReportFlagBitsEXT::ePerformanceWarning
            | vk::DebugReportFlagBitsEXT::eError)
            .setPNext(nullptr)
            .setPfnCallback(vkDebugReportCallbackEXT);

        vk::ApplicationInfo appInfo;
        appInfo.setPNext(nullptr)
            .setPApplicationName("toy")
            .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
            .setPEngineName("No Engine")
            .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
            .setApiVersion(VK_MAKE_API_VERSION(0, 1, 3, 0));

        vk::InstanceCreateInfo insInfo;
        insInfo.setFlags(vk::InstanceCreateFlags() | vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
            .setPNext(&debugInfo)
            .setPApplicationInfo(&appInfo)
            .setEnabledLayerCount(0)
            .setEnabledExtensionCount((uint32_t)extension.size())
            .setPEnabledExtensionNames(extension);
        if(enableValidationLayer)
        {
            insInfo.setEnabledLayerCount((uint32_t)validationLayer.size())
                   .setPpEnabledLayerNames(validationLayer.data());
        }

        try
        {
            mInstance = vk::createInstance(insInfo);
            LOG_T("----------------------");
            LOG_T("{0} : mInstance: {1}", __FUNCTION__, (void*)mInstance);
        }catch (vk::SystemError e)
        {
            throw std::runtime_error(e.what());
        }
    }

    void VkContent::createSurface()
    {
        auto r = glfwCreateWindowSurface(mInstance, window_->GetHandle(), nullptr, (VkSurfaceKHR*)&mSurface);
        if(r != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
        LOG_T("{0} : Surface: {1}", __FUNCTION__, (void*)mSurface);
    }

    QueueFamilyIndices VkContent::findQueueFamilies(const vk::PhysicalDevice& pd_)
    {
        QueueFamilyIndices qf;
        auto q = pd_.getQueueFamilyProperties();
        for(int i = 0; i < q.size(); i++)
        {
            if (q[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                qf.graphicsFamily = i;
            }
            if(pd_.getSurfaceSupportKHR(i, mSurface))
            {
                qf.presentFamily = i;
            }
            if(qf.isComplete())
            {
                break;
            }
        }
        return qf;
    }

    vk::Bool32 VkContent::isDeviceSuitable(const vk::PhysicalDevice& pd_)
    {
        mQueueFamily = findQueueFamilies(pd_);
        if(!mQueueFamily.graphicsFamily.has_value())
        {
            return VK_FALSE;
        }
        auto f = pd_.getFeatures();
        auto p = pd_.getProperties();

        return p.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && f.geometryShader;
    }

    void VkContent::pickPhysicalDevice()
    {
        auto pd_ = mInstance.enumeratePhysicalDevices();
        for(const auto& pd : pd_)
        {
            if(isDeviceSuitable(pd))
            {
                mPhyDevice = pd;
                break;
            }
        }
        if(mPhyDevice == VK_NULL_HANDLE)
        {
            mPhyDevice = pd_[0];
        }
        LOG_T("{0} : PhysicalDevice: {1}",__FUNCTION__, mPhyDevice.getProperties().deviceName.data());
        LOG_T("{0} : GraphicsQueue: {1}",__FUNCTION__, mQueueFamily.graphicsFamily.value());
        LOG_T("{0} : PresentQueue: {1}",__FUNCTION__, mQueueFamily.presentFamily.value());
    }
}
