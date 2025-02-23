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
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * PushConstants.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
    gl_PointSize = 2.0f;
}
