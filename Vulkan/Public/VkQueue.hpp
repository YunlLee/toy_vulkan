//
// Created by Yun on 2024/7/19.
//

#ifndef VKQUEUE_HPP
#define VKQUEUE_HPP
#include "Common.hpp"

namespace toy
{
    class VkQueue
    {
    public:
        VkQueue(uint32_t QueueFamilyIndex, uint32_t index, vk::Queue queue, bool canPresent);
        ~VkQueue() = default;

        void waitIdle();

        [[nodiscard]] vk::Queue GetHandle() const { return mHandle; }
        [[nodiscard]] uint32_t GetFamilyIndex() const { return mFamilyIndex; }

    private:
        vk::Queue mHandle;
        uint32_t mFamilyIndex;
        uint32_t mIndex;
        bool canPresent_;
    };
}

#endif //VKQUEUE_HPP
