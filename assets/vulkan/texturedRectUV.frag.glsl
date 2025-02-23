#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in uint fragTexIndex;

layout(location = 0) out vec4 outColor;
layout(set=0, binding = 1) uniform sampler2D texSampler[2];

void main() {
    outColor = texture(texSampler[fragTexIndex], fragTexCoord);
}


