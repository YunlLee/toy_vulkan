add_library(Vulkan
        Private/Window.cpp
        Private/VkContent.cpp
        Private/VkLog.cpp
        Private/VkDevice.cpp
        Private/VkQueue.cpp
        Private/VkSwapchain.cpp
        Private/GcImageView.cpp
        Private/GcPipeline.cpp
        Private/GcRenderPass.cpp
        Private/GcFramebuffer.cpp
        Private/GcCommandBuffer.cpp
        Private/GcVertexBuffer.cpp
        Private/GcDescriptorSetLayout.cpp
        Private/GcDescriptor.cpp

        External/image/stb_image.cpp
        Private/GcTextureImage.cpp
        Private/GcTextureSampler.cpp
)

target_include_directories(Vulkan PUBLIC External)
target_include_directories(Vulkan PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../ThirtyLib/Vulkan/Include)

add_subdirectory(External/glfw)
add_subdirectory(External/glm)

target_link_libraries(Vulkan PUBLIC glfw)
target_link_libraries(Vulkan PUBLIC vulkan-1.lib)