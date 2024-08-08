//
// Created by Yun on 2024/7/20.
//

#ifndef GCFRAMEBUFFER_HPP
#define GCFRAMEBUFFER_HPP

#include <GcDepthImage.hpp>
#include <GcImageView.hpp>
#include <GcRenderPass.hpp>

#include "Common.hpp"

namespace toy
{
    class VkDevice;
    class GcImageView;
    class GcRenderPass;
    class GcDepthImage;

    class GcFramebuffer
    {
    public:
        GcFramebuffer(VkDevice* devce, GcImageView* imageView, GcRenderPass* renderPass, GcDepthImage* depthImage);
        ~GcFramebuffer();

        void createFramebufer();

        [[nodiscard]] vk::Framebuffer GetFramebuffers(uint32_t index) const { return mFramebuffers[index]; }
    private:
        std::vector<vk::Framebuffer> mFramebuffers;

        VkDevice* device_;
        GcImageView* imageView_;
        GcRenderPass* renderPass_;
        GcDepthImage* depthImage_;
    };

}

#endif //GCFRAMEBUFFER_HPP
