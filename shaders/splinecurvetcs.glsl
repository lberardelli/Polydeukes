#version 410 core

layout(vertices=4) out;
uniform int NumSegments;
uniform int NumStrips;
void main()
{
    // Pass along the vertex position unmodified
    gl_out[gl_InvocationID].gl_Position =
    gl_in[gl_InvocationID].gl_Position;
    // Define the tessellation levels (this works on
    // ATI Catalyst drivers as of this writing, you may
    // need to swap these)
    gl_TessLevelOuter[0] = float(NumSegments);
    gl_TessLevelOuter[1] = float(NumStrips);
}
