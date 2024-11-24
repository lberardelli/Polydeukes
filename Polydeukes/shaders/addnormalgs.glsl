#version 410 core

layout(triangles) in;                  // Receive points from the tessellation evaluation shader
layout(line_strip, max_vertices = 6) out;

in vec4 gsPosition[];
in vec4 gsNormal[];

uniform mat4 projection;

void GenerateLine(int index)
{
    vec4 direction = gsNormal[index] - gl_in[index].gl_Position;
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position +
                                gsNormal[index]);
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position +
                                gsNormal[index] + vec4(0.1 * direction.x,0.0,0.0,0.0));
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position +
                                gsNormal[index] + vec4(0.0,0.1 * direction.y,0.0,0.0));
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position +
                                gsNormal[index] + vec4(-0.1 * direction.x,0.0,0.0,0.0));
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position +
                                gsNormal[index]);
    EmitVertex();
    
    EndPrimitive();
}

void main() {
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
    

}
