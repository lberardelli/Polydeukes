#version 410 core
layout( isolines ) in;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 decasteljau(float parameterValue, vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
    vec3 firstInterpolatedValue = p1*parameterValue + p0 * (1.f-parameterValue);
    vec3 secondInterpolatedValue = p2*parameterValue + p1 * (1.f-parameterValue);
    vec3 thirdInterpolatedValue = p3*parameterValue + p2 * (1.f-parameterValue);
    vec3 secondFirstIV = secondInterpolatedValue*(parameterValue) + firstInterpolatedValue*(1.f-parameterValue);
    vec3 secondsecondIV = thirdInterpolatedValue*(parameterValue) + secondInterpolatedValue*(1.f-parameterValue);
    return secondsecondIV * (parameterValue) + secondFirstIV * (1.f-parameterValue);
}

void main()
{
    // The tessellation u coordinate
    float u = gl_TessCoord.x;
    // The patch vertices (control points)
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;
    gl_Position = projection * view * model * vec4(decasteljau(u, p0, p1,p2,p3), 1.0);
}
