//
// Created by Yun on 2024/7/19.
//

#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include "Common.hpp"

namespace toy
{
    class Window;
    class VkContent;
    class VkDevice;
    class VkSwapchain;
    class GcImageView;
    class GcRenderPass;
    class GcPipeline;
    class GcFramebuffer;
    class GcCommandBuffer;
}


class Application
{
public:
    void run();

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanUp();

    void recordCommandBuffer(vk::CommandBuffer cmdBuffer, uint32_t imageIndex);
    void drawFrame();
    void createSyncObject();
private:
    vk::Semaphore imageAvailableSemaphore;
    vk::Semaphore renderFinishedSemaphore;
    vk::Fence inFlightFence;

    std::shared_ptr<toy::Window> window_;
    std::shared_ptr<toy::VkContent> content_;
    std::shared_ptr<toy::VkDevice> device_;
    std::shared_ptr<toy::VkSwapchain> swapchain_;
    std::shared_ptr<toy::GcImageView> imageView_;
    std::shared_ptr<toy::GcRenderPass> renderPass_;
    std::shared_ptr<toy::GcPipeline> pipeline_;
    std::shared_ptr<toy::GcFramebuffer> framebuffer_;
    std::shared_ptr<toy::GcCommandBuffer> commandBuffer_;
};

#endif //APPLICATION_HPP
