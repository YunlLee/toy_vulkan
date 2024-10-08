//
// Created by Yun on 2024/7/23.
//

#ifndef VERTEX_HPP
#define VERTEX_HPP

namespace toy
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static vk::VertexInputBindingDescription getBindingDescription()
        {
            vk::VertexInputBindingDescription bindingDescription;
            bindingDescription.setBinding(0)
                .setStride((uint32_t)sizeof(Vertex))
                .setInputRate(vk::VertexInputRate::eVertex);
            return bindingDescription;
        }

        static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
        {
            std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions;
            attributeDescriptions[0].setBinding(0)
                .setLocation(0)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setOffset(offsetof(Vertex,pos));

            attributeDescriptions[1].setBinding(0)
                .setLocation(1)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setOffset(offsetof(Vertex, color));

            attributeDescriptions[2].setBinding(0)
                .setLocation(2)
                .setFormat(vk::Format::eR32G32Sfloat)
                .setOffset(offsetof(Vertex, texCoord));
            return attributeDescriptions;
        }
    };

    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
}

#endif //VERTEX_HPP
