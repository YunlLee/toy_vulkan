//
// Created by Yun on 2024/7/23.
//

#ifndef VERTEX_HPP
#define VERTEX_HPP

namespace toy
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static vk::VertexInputBindingDescription getBindingDescription()
        {
            vk::VertexInputBindingDescription bindingDescription;
            bindingDescription.setBinding(0)
                .setStride((uint32_t)sizeof(Vertex))
                .setInputRate(vk::VertexInputRate::eVertex);
            return bindingDescription;
        }

        static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
        {
            std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions;
            attributeDescriptions[0].setBinding(0)
                .setLocation(0)
                .setFormat(vk::Format::eR32G32Sfloat)
                .setOffset(offsetof(Vertex,pos));
            attributeDescriptions[1].setBinding(0)
                .setLocation(1)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setOffset(offsetof(Vertex, color));
            return attributeDescriptions;
        }
    };
}

#endif //VERTEX_HPP
