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
layout(location = 2) in vec4 borderColor;
layout(location = 3) in vec4 fillColor;

layout(location = 0) out vec4 fragBorderColor;
layout(location = 1) out vec4 fragFillColor;
layout(location = 2) out vec2 boundary;

void main() {

    vec4 vertex;

    float borderWidth = 2.0;

    float bInterpX = inSize.x / (inSize.x - 2.0 * borderWidth);
    float bInterpY = inSize.y / (inSize.y - 2.0 * borderWidth);

    switch (gl_VertexIndex) {
        case 0:
            vertex = vec4(inPosition, 0.0, 1.0);
            boundary = vec2(-bInterpX, -bInterpY);
            break;
        case 1:
            vertex = vec4(inPosition.x + inSize.x, inPosition.y, 0.0, 1.0);
            boundary = vec2(bInterpX, -bInterpY);
            break;
        case 2:
            vertex = vec4(inPosition.x, inPosition.y + inSize.y, 0.0, 1.0);
            boundary = vec2(-bInterpX, bInterpY);
            break;
        case 3:
            vertex = vec4(inPosition.x + inSize.x, inPosition.y + inSize.y, 0.0, 1.0);
            boundary = vec2(bInterpX, bInterpY);
            break;
    }

    gl_Position = ubo.proj * ubo.view * PushConstants.model * vertex;

    fragFillColor = fillColor;
    fragBorderColor = borderColor;
}
