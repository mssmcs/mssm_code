#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
// layout (location = 0) in vec4 fragBorderColor;
// layout (location = 1) in vec4 fragFillColor;
// layout (location = 2) in vec2 boundary;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;

void main() {
    //outColor = vec4(fragTexCoord, 0.0, 1.0);
    outColor = texture(texSampler, fragTexCoord);
}


// layout (location = 0) in vec4 fragBorderColor;
// layout (location = 1) in vec4 fragFillColor;
// layout (location = 2) in vec2 boundary;

// layout (location = 0) out vec4 outColor;

//  void main ()
//  {
//     if (abs(boundary.x) > 1 || abs(boundary.y) > 1)
//     {
//         outColor = fragBorderColor;
//     }
//     else
//     {
//         outColor = fragFillColor;
//     }
//  }
