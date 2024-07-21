//
// Created by Yun on 2024/7/19.
//

#ifndef VKDEVICE_HPP
#define VKDEVICE_HPP

#include "Common.hpp"

namespace toy
{
    class VkQueue;
    class VkContent;
    class VkDevice
    {
    public:
        VkDevice(VkContent* content, uint32_t , uint32_t);
        ~VkDevice();

        [[nodiscard]] vk::Device GetDevice() const { return mHandle; }
        [[nodiscard]] std::vector<std::shared_ptr<VkQueue>> GetGraphic() const { return mGraphicQueues; }
        [[nodiscard]] std::vector<std::shared_ptr<VkQueue>> GetPresent() const { return mPresentQueues; }

    private:
        bool checkDeviceExtensionSupport(vk::PhysicalDevice phyDevice);
    private:
        vk::Device mHandle;

        std::vector<std::shared_ptr<VkQueue>> mGraphicQueues;
        std::vector<std::shared_ptr<VkQueue>> mPresentQueues;

        VkContent* content_;
    };

}

#endif //VKDEVICE_HPP
