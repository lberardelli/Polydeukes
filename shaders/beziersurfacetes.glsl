#version 410 core

layout( quads ) in;
out vec4 gsNormal; // Vertex normal in camera coords.
out vec4 gsPosition; // Vertex position in camera coords
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void basisFunctions(out float[4] b, out float[4] db, float t)
{
    float t1 = (1.0 - t);
    float t12 = t1 * t1;
    // Bernstein polynomials
    b[0] = t12 * t1;
    b[1] = 3.0 * t12 * t;
    b[2] = 3.0 * t1 * t * t;
    b[3] = t * t * t;
    // Derivatives
    db[0] = -3.0 * t1 * t1;
    db[1] = -6.0 * t * t1 + 3.0 * t12;
    db[2] = -3.0 * t * t + 6.0 * t * t1;
    db[3] = 3.0 * t * t;
}

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
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    // The sixteen control points
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p02 = gl_in[2].gl_Position;
    vec4 p03 = gl_in[3].gl_Position;
    vec4 p10 = gl_in[4].gl_Position;
    vec4 p11 = gl_in[5].gl_Position;
    vec4 p12 = gl_in[6].gl_Position;
    vec4 p13 = gl_in[7].gl_Position;
    vec4 p20 = gl_in[8].gl_Position;
    vec4 p21 = gl_in[9].gl_Position;
    vec4 p22 = gl_in[10].gl_Position;
    vec4 p23 = gl_in[11].gl_Position;
    vec4 p30 = gl_in[12].gl_Position;
    vec4 p31 = gl_in[13].gl_Position;
    vec4 p32 = gl_in[14].gl_Position;
    vec4 p33 = gl_in[15].gl_Position;
    // Compute basis functions
    float bu[4], bv[4]; // Basis functions for u and v
    float dbu[4], dbv[4]; // Derivitives for u and v
    basisFunctions(bu, dbu, u);
    basisFunctions(bv, dbv, v);
    // Bezier interpolation
    vec3 i1 = decasteljau(u,p00.xyz,p01.xyz,p02.xyz,p03.xyz);
    vec3 i2 = decasteljau(u,p10.xyz,p11.xyz,p12.xyz,p13.xyz);
    vec3 i3 = decasteljau(u,p20.xyz,p21.xyz,p22.xyz,p23.xyz);
    vec3 i4 = decasteljau(u,p30.xyz,p31.xyz,p32.xyz,p33.xyz);
    gsPosition = p00*bu[0]*bv[0] + p01*bu[0]*bv[1] + p02*bu[0]*bv[2] +
    p03*bu[0]*bv[3] +
    p10*bu[1]*bv[0] + p11*bu[1]*bv[1] + p12*bu[1]*bv[2] +
    p13*bu[1]*bv[3] +
    p20*bu[2]*bv[0] + p21*bu[2]*bv[1] + p22*bu[2]*bv[2] +
    p23*bu[2]*bv[3] +
    p30*bu[3]*bv[0] + p31*bu[3]*bv[1] + p32*bu[3]*bv[2] +
    p33*bu[3]*bv[3];
    // The partial derivatives
    vec4 du =
    p00*dbu[0]*bv[0] + p01*dbu[0]*bv[1] + p02*dbu[0]*bv[2] +
    p03*dbu[0]*bv[3] +
    p10*dbu[1]*bv[0] + p11*dbu[1]*bv[1] + p12*dbu[1]*bv[2] +
    p13*dbu[1]*bv[3] +
    p20*dbu[2]*bv[0] + p21*dbu[2]*bv[1] + p22*dbu[2]*bv[2] +
    p23*dbu[2]*bv[3] +
    p30*dbu[3]*bv[0] + p31*dbu[3]*bv[1] + p32*dbu[3]*bv[2] +
    p33*dbu[3]*bv[3];
    vec4 dv =
    p00*bu[0]*dbv[0] + p01*bu[0]*dbv[1] + p02*bu[0]*dbv[2] +
    p03*bu[0]*dbv[3] +
    p10*bu[1]*dbv[0] + p11*bu[1]*dbv[1] + p12*bu[1]*dbv[2] +
    p13*bu[1]*dbv[3] +
    p20*bu[2]*dbv[0] + p21*bu[2]*dbv[1] + p22*bu[2]*dbv[2] +
    p23*bu[2]*dbv[3] +
    p30*bu[3]*dbv[0] + p31*bu[3]*dbv[1] + p32*bu[3]*dbv[2] +
    p33*bu[3]*dbv[3];
    // The normal vector is the cross product of the partials
    vec3 n = normalize(cross(du.xyz, dv.xyz));
    // Transform to clip coordinates
    gl_Position = view * model * gsPosition;
    // Convert to camera coordinates
    gsPosition = model * gsPosition;
    gsNormal = normalize((transpose(inverse(model)) * vec4(n, 0.0)));
}
