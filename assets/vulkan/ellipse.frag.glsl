#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 fragBorderColor;
layout (location = 1) in vec4 fragFillColor;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec2 boundary;

layout (location = 0) out vec4 outColor;

 void main ()
 {
     if (dot(uv, uv) > 1.0) {
         discard;
     }
     else if (dot(boundary, boundary) > 1)
     {
         outColor = fragBorderColor;
     }
     else
     {
         outColor = fragFillColor;
     }
 }
