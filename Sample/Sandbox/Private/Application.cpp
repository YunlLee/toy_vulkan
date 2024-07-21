//
// Created by Yun on 2024/7/19.
//

#include "Application.hpp"

#include <GcImageView.hpp>
#include <VkDevice.hpp>
#include <VkSwapchain.hpp>
#include <GcRenderPass.hpp>
#include <VkQueue.hpp>

#include "GcFramebuffer.hpp"
#include "GcPipeline.hpp"

#include "Window.hpp"
#include "VkContent.hpp"
#include "GcCommandBuffer.hpp"

void Application::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanUp();
}

void Application::initWindow()
{
    window_ = std::make_shared<toy::Window>(800, 600, "Toy");
}

void Application::initVulkan()
{
    content_ = std::make_shared<toy::VkContent>(window_.get());
    device_ = std::make_shared<toy::VkDevice>(content_.get(), 1, 1);
    swapchain_ = std::make_shared<toy::VkSwapchain>(content_.get(), device_.get());
    imageView_ = std::make_shared<toy::GcImageView>(device_.get(), swapchain_.get());
    renderPass_ = std::make_shared<toy::GcRenderPass>(device_.get(), swapchain_.get());
    pipeline_ = std::make_shared<toy::GcPipeline>(device_.get(), swapchain_.get(), renderPass_.get());
    framebuffer_ = std::make_shared<toy::GcFramebuffer>(device_.get(), imageView_.get(), renderPass_.get());
    commandBuffer_ = std::make_shared<toy::GcCommandBuffer>(content_.get(), device_.get());

    createSyncObject();
}

void Application::mainLoop()
{
    while(!glfwWindowShouldClose(window_->GetHandle()))
    {
        glfwPollEvents();
        drawFrame();
    }
    device_->GetDevice().waitIdle();
}

void Application::cleanUp()
{
    VK_D(Semaphore, device_->GetDevice(), imageAvailableSemaphore);
    VK_D(Semaphore, device_->GetDevice(), renderFinishedSemaphore);
    VK_D(Fence, device_->GetDevice(), inFlightFence);

    commandBuffer_.reset();
    framebuffer_.reset();
    pipeline_.reset();
    renderPass_.reset();
    imageView_.reset();
    swapchain_.reset();
    device_.reset();
    content_.reset();
    window_.reset();
}

void Application::recordCommandBuffer(vk::CommandBuffer cmdBuffer, uint32_t imageIndex)
{
    // 1. begin command buffer
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.setFlags({})
        .setPInheritanceInfo(nullptr);

    VK_CREATE(cmdBuffer.begin(commandBufferBeginInfo), "failed to begin recording mommand buffer!");

    // 2. begin render pass
    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.setRenderPass(renderPass_->GetRenderPass())
        .setFramebuffer(framebuffer_->GetFramebuffers(imageIndex))
        .setRenderArea({{0,0}, swapchain_->GetExtent()});
    vk::ClearValue clearValue{{0.1f, 0.2f, 0.3f, 1.0f}};
    renderPassBeginInfo.setClearValueCount(1)
        .setPClearValues(&clearValue);

    cmdBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    // 3. bind render graphic pipeline
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_->GetPipeline());

    /**
     * set viewport & scissor
     * Only when the viewport and scissor are set as dynamic state
    */
    vk::Viewport viewport{0, 0,
        (float)swapchain_->GetExtent().width, (float)swapchain_->GetExtent().height, 0, 1};
    cmdBuffer.setViewport(0, viewport);

    vk::Rect2D scissor{{0,0}, {swapchain_->GetExtent()}};
    cmdBuffer.setScissor(0, scissor);

    // 4. drawing
    cmdBuffer.draw(3, 1, 0, 0);

    // 5. finishing up
    cmdBuffer.endRenderPass();
    cmdBuffer.end();
}

void Application::drawFrame()
{
    auto r = device_->GetDevice().waitForFences(inFlightFence, true, UINT64_MAX);
    device_->GetDevice().resetFences(inFlightFence);
    uint32_t imageIndex = swapchain_->getNextImageIndex(imageAvailableSemaphore);

    vk::CommandBuffer cmdBuffer = commandBuffer_->allocateCommandBuffers(1)[0];
    // 1. reset commandbuffer
    cmdBuffer.reset();

    // 2. recording command
    recordCommandBuffer(cmdBuffer, imageIndex);

    // 3. submitting the command buffer
    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
    vk::Semaphore signalSemaphores[] = {renderFinishedSemaphore};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput };
    vk::SubmitInfo submitInfo;
    submitInfo.setWaitSemaphoreCount(1)
        .setPWaitSemaphores(waitSemaphores)
        .setWaitDstStageMask(waitStages)
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmdBuffer)
        .setSignalSemaphoreCount(1)
        .setPSignalSemaphores(signalSemaphores);
    device_->GetGraphic()[0]->GetHandle().submit(submitInfo, inFlightFence);

    // 4. present
    vk::PresentInfoKHR presentInfo;
    presentInfo.setWaitSemaphoreCount(1)
        .setPWaitSemaphores(signalSemaphores);
    vk::SwapchainKHR swapchain[] = {swapchain_->GetSwapchain()};
    presentInfo.setSwapchainCount(1)
        .setPSwapchains(swapchain)
        .setPImageIndices(&imageIndex)
        .setPResults(nullptr);
    r = device_->GetPresent()[0]->GetHandle().presentKHR(presentInfo);
}

void Application::createSyncObject()
{
    vk::SemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.setFlags({});

    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    VK_CREATE(imageAvailableSemaphore = device_->GetDevice().createSemaphore(semaphoreInfo), "failed to create semaphores!");
    VK_CREATE(renderFinishedSemaphore = device_->GetDevice().createSemaphore(semaphoreInfo), "failed to create semaphores!");
    VK_CREATE(inFlightFence = device_->GetDevice().createFence(fenceInfo), "failed to create fence!");
}
