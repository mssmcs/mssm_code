#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;  // Optional, depending on usage
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 gp;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;       // 2D view matrix or default view matrix
    mat4 proj;       // 2D projection matrix or default projection matrix
    mat4 view3d;     // 3D-specific view matrix
    mat4 proj3d;     // 3D-specific projection matrix
    vec3 lightPosition;
    vec3 lightColor;
    vec3 viewPos;    // Camera position
} ubo;

layout(location = 0) out vec4 outColor;

void main() {
    // Normalize normal vector
    vec3 norm = fragNormal;

    vec4 litColor = vec4(ubo.lightColor, 1.0) * fragColor;

    // Ambient lighting
    vec4 ambient = 0.1 * litColor; // Flexible ambient color

    ambient = ambient * vec4(ambient.a, ambient.a, ambient.a, 0);
    ambient.a = fragColor.a;

    // Diffuse lighting
    vec3 lightDir = normalize(ubo.lightPosition - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = 0.7* diff * litColor;

    diffuse = diffuse * vec4(diffuse.a, diffuse.a, diffuse.a, 0);
    diffuse.a = fragColor.a;

    // Specular lighting
    vec3 viewDir = normalize(ubo.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float shininess = 32.0; // Adjust shininess or pass it as a uniform
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec4 specular = spec * litColor;

    // Combine results
    vec4 result = ambient + diffuse + specular;

    result.a = fragColor.a; // hack

    outColor = result; // vec4(norm, 1); //vec4(fragPos.yyy, 1);// fragColor; // vec4(1,1,1, 1);
}
