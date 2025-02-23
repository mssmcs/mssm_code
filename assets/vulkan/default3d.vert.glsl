#version 450
#extension GL_ARB_separate_shader_objects : enable


// https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;       // 2D view matrix or default view matrix
    mat4 proj;       // 2D projection matrix or default projection matrix
    mat4 view3d;     // 3D-specific view matrix
    mat4 proj3d;     // 3D-specific projection matrix
    vec3 lightPosition;
    vec3 lightColor;
    vec3 viewPos;    // Camera position
} ubo;

layout( push_constant ) uniform constants
{
    mat4 model;
} PushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 gp;

void main() {
    gl_Position = ubo.proj3d * ubo.view3d * PushConstants.model * vec4(inPosition, 1.0);
    gp = gl_Position.xyz;
    fragPos = vec3(PushConstants.model * vec4(inPosition, 1.0));
    fragColor = inColor;
    gl_PointSize = 2.0f;

    fragNormal = normalize(mat3(transpose(inverse(PushConstants.model))) * inNormal);
}
