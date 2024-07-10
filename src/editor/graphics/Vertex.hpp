#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace Zeus
{
struct Vertex
{
    glm::vec2 position; //vec3
    glm::vec3 color;
    // glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        // When we create a vertex buffer, we bind it to a chosen slot before
        // rendering operations. The slot number (an index) is this binding and
        // here we describe how data in this slot is aligned in memory and how
        // it should be consumed (per vertex or per instance). Different vertex
        // buffers can be bound to different bindings. And each binding may be
        // differently positioned in memory.
        VkVertexInputBindingDescription bindingDescription{
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2>
    getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2>
            attributeDescriptions{};

        // We must specify a location (index) for each attribute (the same as in
        // a shader source code, in location layout qualifier), source of data
        // (binding from which data will be read), format (data type and number
        // of components), and offset at which data for this specific attribute
        // can be found (offset from the beginning of a data for a given vertex,
        // not from the beginning of all vertex data

        // location – Index of an attribute, the same as defined by the location
        // layout specifier in a shader source code.
        // binding – The number of the slot from which data should be read
        // (source of data like VBO in OpenGL), the same binding as in a
        // VkVertexInputBindingDescription structure and
        // vkCmdBindVertexBuffers() function (described later). format – Data
        // type and number of components per attribute. offset – Beginning of
        // data for a given attribute.
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;//VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        // attributeDescriptions[2].binding = 0;
        // attributeDescriptions[2].location = 2;
        // attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        // attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
};
}
