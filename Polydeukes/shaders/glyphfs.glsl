#version 410 core

in vec2 fragCoord; // Screen-space coordinates passed from vertex shader

out vec4 FragColor; // Final fragment color
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 resolution;

// Uniform Buffer Object (UBO) to store edges
layout(std140) uniform EdgeData {
    vec4 edges[80]; // Each edge is stored as vec4 (x1, y1, x2, y2)
    int numEdges; // Total number of edges
};

// Function to check if a point is inside the polygon using a scanline algorithm
bool isInsidePolygon(vec2 point) {
    bool inside = false;

    for (int i = 0; i < numEdges; ++i) {
        vec2 p1 = edges[i].xy;
        vec2 p2 = edges[i].zw;

        // Check if the edge crosses the scanline
        if (((p1.y > point.y) != (p2.y > point.y)) &&
            (point.x < (p2.x - p1.x) * (point.y - p1.y) / (p2.y - p1.y) + p1.x)) {
            inside = !inside;
        }
    }

    return inside;
}

void main() {
    // Check if the current fragment (pixel) is inside the polygon
    if (isInsidePolygon(fragCoord)) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White if inside
    } else {
        discard;
    }
}
