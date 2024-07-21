//
// Created by Yun on 2024/7/21.
//

#ifndef GCCOMMANDBUFFER_HPP
#define GCCOMMANDBUFFER_HPP

#include "Common.hpp"

namespace toy
{
    class VkContent;
    class VkDevice;

    class GcCommandBuffer
    {
    public:
        GcCommandBuffer(VkContent* content, VkDevice* device);
        ~GcCommandBuffer();

        std::vector<vk::CommandBuffer> allocateCommandBuffers(uint32_t count);

    private:
        void createCommandPool();

        vk::CommandPool mCommandPool;

        VkDevice* device_;
        VkContent* content_;
    };
}

#endif //GCCOMMANDBUFFER_HPP
