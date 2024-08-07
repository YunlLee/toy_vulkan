//
// Created by Yun on 2024/7/19.
//

#include "Application.hpp"

#include "GcVertexBuffer.hpp"
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
#include "GcDescriptorSetLayout.hpp"
#include "GcDescriptor.hpp"
#include "GcTextureImage.hpp"
#include "GcTextureSampler.hpp"

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
    descriptorSetLayout_ = std::make_shared<toy::GcDescriptorSetLayout>(device_.get());
    pipeline_ = std::make_shared<toy::GcPipeline>(device_.get(), swapchain_.get(),
        renderPass_.get(), descriptorSetLayout_.get());
    framebuffer_ = std::make_shared<toy::GcFramebuffer>(device_.get(), imageView_.get(), renderPass_.get());
    commandBuffer_ = std::make_shared<toy::GcCommandBuffer>(content_.get(), device_.get());
    texture_ = std::make_shared<toy::GcTextureImage>(device_.get(), content_.get(), commandBuffer_.get());
    textureImgView = imageView_->createTextureImageView(texture_->GetTextureImage());
    buffer_ = std::make_shared<toy::GcVertexBuffer>(content_.get(), device_.get(), commandBuffer_.get());
    textureSampler_ = std::make_shared<toy::GcTextureSampler>(content_.get(), device_.get());
    descriptor_ = std::make_shared<toy::GcDescriptor>(device_.get(), descriptorSetLayout_.get(), buffer_.get(), textureSampler_.get(), textureImgView);


    createSyncObject();

    cmdBuffers = commandBuffer_->allocateCommandBuffers(MAX_FRAMES_IN_FLIGHT);
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
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VK_D(Semaphore, device_->GetDevice(), imageAvailableSemaphores[i]);
        VK_D(Semaphore, device_->GetDevice(), renderFinishedSemaphores[i]);
        VK_D(Fence, device_->GetDevice(), inFlightFences[i]);
    }

    textureSampler_.reset();


    descriptor_.reset();
    texture_.reset();
    buffer_.reset();
    device_->GetDevice().destroyImageView(textureImgView);
    commandBuffer_.reset();
    framebuffer_.reset();
    pipeline_.reset();
    descriptorSetLayout_.reset();
    renderPass_.reset();
    imageView_.reset();
    swapchain_.reset();
    // cleanupSwapChain();
    device_.reset();
    content_.reset();
    window_.reset();
}

void Application::cleanupSwapChain()
{
    framebuffer_.reset();
    imageView_.reset();
    swapchain_.reset();
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

    vk::Buffer vertexBuffer[] = { buffer_->GetVertexBuffer() };
    vk::DeviceSize offsets[] = {0};
    cmdBuffer.bindVertexBuffers(0, vertexBuffer, offsets);

    cmdBuffer.bindIndexBuffer(buffer_->GetIndexBuffer(), 0, vk::IndexType::eUint16);

    cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
        pipeline_->GetPipelineLayout(), 0, descriptor_->GetDescriptorSet()[currentFrame], nullptr);

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
    // cmdBuffer.draw((uint32_t)toy::vertices.size(), 1, 0, 0);
    cmdBuffer.drawIndexed((uint32_t)toy::indices.size(), 1, 0, 0, 0);

    // 5. finishing up
    cmdBuffer.endRenderPass();
    cmdBuffer.end();
}

void Application::drawFrame()
{
    auto result = device_->GetDevice().waitForFences(inFlightFences[currentFrame], true, UINT64_MAX);

    // acquire image from swapchain
    auto rv = swapchain_->getNextImageIndex(imageAvailableSemaphores[currentFrame]);
    uint32_t imageIndex = rv.value;

    if(rv.result == vk::Result::eErrorOutOfDateKHR)
    {
        recreateSwapChain();
        return;
    }else if(rv.result != vk::Result::eSuccess && rv.result != vk::Result::eSuboptimalKHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer(currentFrame);
    device_->GetDevice().resetFences(inFlightFences[currentFrame]);

    // 1. reset commandbuffer
    cmdBuffers[currentFrame].reset();

    // 2. recording command
    recordCommandBuffer(cmdBuffers[currentFrame], imageIndex);

    // update uniform buffer

    // 3. submitting the command buffer
    vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput };
    vk::SubmitInfo submitInfo;
    submitInfo.setWaitSemaphoreCount(1)
        .setPWaitSemaphores(waitSemaphores)
        .setWaitDstStageMask(waitStages)
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmdBuffers[currentFrame])
        .setSignalSemaphoreCount(1)
        .setPSignalSemaphores(signalSemaphores);
    device_->GetGraphic()[0]->GetHandle().submit(submitInfo, inFlightFences[currentFrame]);

    // 4. present
    vk::PresentInfoKHR presentInfo;
    presentInfo.setWaitSemaphoreCount(1)
        .setPWaitSemaphores(signalSemaphores);
    vk::SwapchainKHR swapchain[] = {swapchain_->GetSwapchain()};
    presentInfo.setSwapchainCount(1)
        .setPSwapchains(swapchain)
        .setPImageIndices(&imageIndex)
        .setPResults(nullptr);

    auto r = device_->GetPresent()[0]->GetHandle().presentKHR(presentInfo);
    if(r == vk::Result::eErrorOutOfDateKHR)
    {
        recreateSwapChain();

    }else if(r != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::createSyncObject()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    vk::SemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.setFlags({});

    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VK_CREATE(imageAvailableSemaphores[i] = device_->GetDevice().createSemaphore(semaphoreInfo), "failed to create semaphores!");
        VK_CREATE(renderFinishedSemaphores[i] = device_->GetDevice().createSemaphore(semaphoreInfo), "failed to create semaphores!");
        VK_CREATE(inFlightFences[i] = device_->GetDevice().createFence(fenceInfo), "failed to create fence!");
    }
}

void Application::recreateSwapChain()
{
    int w, h;
    glfwGetWindowSize(window_->GetHandle(), &w, &h);
    while(w == 0 || h == 0)
    {
        glfwGetWindowSize(window_->GetHandle(), &w, &h);
        glfwWaitEvents();
    }

    device_->GetDevice().waitIdle();

    cleanupSwapChain();

    swapchain_ = std::make_shared<toy::VkSwapchain>(content_.get(), device_.get());
    imageView_ = std::make_shared<toy::GcImageView>(device_.get(), swapchain_.get());
    framebuffer_ = std::make_shared<toy::GcFramebuffer>(device_.get(), imageView_.get(), renderPass_.get());
}

void Application::updateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();

    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    toy::UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.1f));

    ubo.proj = glm::perspective(glm::radians(45.0f), (float)swapchain_->GetExtent().width / (float)swapchain_->GetExtent().height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(buffer_->GetUniformBuffersMapped()[currentFrame], &ubo, sizeof(ubo));
}


