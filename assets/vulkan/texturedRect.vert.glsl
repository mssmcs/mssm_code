#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 view3d;
    mat4 proj3d;
} ubo;

layout( push_constant ) uniform constants
{
    mat4 model;
} PushConstants;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inSize;
layout(location = 2) in vec4 fillColor;
layout(location = 3) in uint textureIndex;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out uint fragTexIndex;

void main() {

    vec4 vertex;

    switch (gl_VertexIndex) {
        case 0:
            vertex = vec4(inPosition, 0.0, 1.0);
            fragTexCoord = vec2(0, 0);
            break;
        case 1:
            vertex = vec4(inPosition.x + inSize.x, inPosition.y, 0.0, 1.0);
            fragTexCoord = vec2(1, 0);
            break;
        case 2:
            vertex = vec4(inPosition.x, inPosition.y + inSize.y, 0.0, 1.0);
            fragTexCoord = vec2(0, 1);
            break;
        case 3:
            vertex = vec4(inPosition.x + inSize.x, inPosition.y + inSize.y, 0.0, 1.0);
            fragTexCoord = vec2(1, 1);
            break;
    }

    gl_Position = ubo.proj * ubo.view * PushConstants.model * vertex;

    fragColor = fillColor;
    fragTexIndex = textureIndex;
}
