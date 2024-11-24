#version 410 core

layout(triangles) in;                  // Receive points from the tessellation evaluation shader
layout(triangle_strip, max_vertices = 3) out;

in vec4 gsPosition[];
in vec4 gsNormal[];

out vec4 FragPosition;
out vec4 FragNormal;

uniform mat4 projection;


void main() {
    for (int i = 0; i < 3; ++i) {
        gl_Position = projection * gl_in[i].gl_Position;
        FragPosition = gsPosition[i];
        FragNormal = gsNormal[i];
        EmitVertex();
    }
    EndPrimitive();
}

