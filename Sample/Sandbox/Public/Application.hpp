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
    class GcVertexBuffer;
    class GcDescriptorSetLayout;
    class GcDescriptor;
    class GcTextureImage;
    class GcTextureSampler;
}


class Application
{
public:
    void run();

    bool framebufferResized = false;
private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanUp();

    void recordCommandBuffer(vk::CommandBuffer cmdBuffer, uint32_t imageIndex);
    void drawFrame();
    void createSyncObject();

    void recreateSwapChain();
    void cleanupSwapChain();

    void updateUniformBuffer(uint32_t currentImage);
private:
    std::vector<vk::CommandBuffer> cmdBuffers;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;

    vk::ImageView textureImgView;

    uint32_t currentFrame = 0;

    std::shared_ptr<toy::Window> window_;
    std::shared_ptr<toy::VkContent> content_;
    std::shared_ptr<toy::VkDevice> device_;
    std::shared_ptr<toy::VkSwapchain> swapchain_;
    std::shared_ptr<toy::GcImageView> imageView_;
    std::shared_ptr<toy::GcRenderPass> renderPass_;
    std::shared_ptr<toy::GcDescriptorSetLayout> descriptorSetLayout_;
    std::shared_ptr<toy::GcPipeline> pipeline_;
    std::shared_ptr<toy::GcFramebuffer> framebuffer_;
    std::shared_ptr<toy::GcCommandBuffer> commandBuffer_;
    std::shared_ptr<toy::GcVertexBuffer> buffer_;
    std::shared_ptr<toy::GcDescriptor> descriptor_;
    std::shared_ptr<toy::GcTextureImage> texture_;
    std::shared_ptr<toy::GcTextureSampler> textureSampler_;
};

#endif //APPLICATION_HPP
