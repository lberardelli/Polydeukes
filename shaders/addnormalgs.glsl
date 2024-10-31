#version 410 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec4 gsPosition[];    // Position from TES
in vec3 gsNormal[];      // Normal from TES

out vec4 FragNormal;     // Pass to FS
out vec4 FragPosition;   // Pass to FS

uniform mat4 view;
uniform mat4 projection;
uniform float normalLength = 0.1;

void main() {
    for (int i = 0; i < 3; ++i) {
        // Pass along position and normal to fragment shader
        FragPosition = gsPosition[i];
        FragNormal = vec4(gsNormal[i], 0.0);

        // Emit original vertex position
        gl_Position = projection * view * gsPosition[i];
        EmitVertex();

        // Emit end of the normal line
        vec3 endPoint = gsPosition[i].xyz + gsNormal[i] * normalLength;
        gl_Position = projection * view * vec4(endPoint, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}
