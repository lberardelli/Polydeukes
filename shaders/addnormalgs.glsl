#version 410 core

layout(triangles) in;                  // Receive points from the tessellation evaluation shader
layout(triangle_strip, max_vertices = 4) out;

in vec4 gsPosition[];
in vec4 gsNormal[];

out vec4 FragPosition;
out vec4 FragNormal;

void main() {
    // First triangle from the quad
    FragPosition = gsPosition[0]; // Bottom left
    FragNormal = gsNormal[0];
    gl_Position = gsPosition[0];
    EmitVertex();

    FragPosition = gsPosition[1]; // Bottom right
    FragNormal = gsNormal[1];
    gl_Position = gsPosition[1];
    EmitVertex();

    FragPosition = gsPosition[2]; // Top left
    FragNormal = gsNormal[2];
    gl_Position = gsPosition[2];
    EmitVertex();
    EndPrimitive();

    // Second triangle from the quad
    FragPosition = gsPosition[2]; // Top left
    FragNormal = gsNormal[2];
    gl_Position = gsPosition[2];
    EmitVertex();

    FragPosition = gsPosition[1]; // Bottom right
    FragNormal = gsNormal[1];
    gl_Position = gsPosition[1];
    EmitVertex();

    FragPosition = gsPosition[3]; // Top right
    FragNormal = gsNormal[3];
    gl_Position = gsPosition[3];
    EmitVertex();
    EndPrimitive();
}
