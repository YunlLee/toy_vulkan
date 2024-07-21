//
// Created by Yun on 2024/7/19.
//

#include "VkQueue.hpp"

#include <VkContent.hpp>

#include "Common.hpp"

namespace toy
{
    VkQueue::VkQueue(uint32_t QueueFamilyIndex, uint32_t index, vk::Queue queue, bool canPresent)
        : mFamilyIndex(QueueFamilyIndex), mIndex(index), mHandle(queue), canPresent_(canPresent){
        LOG_T("Create a new queue {0} - {1} - {2}, canPresent: {3}", mFamilyIndex, index, (void*)mHandle, canPresent);
    }

    void VkQueue::waitIdle()
    {
        mHandle.waitIdle();
    }
}
