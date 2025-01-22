#version 410 core

// Uniform Buffer Object (UBO) to store edges
layout(std140) uniform EdgeData {
    vec4 edges[NUM_EDGES]; // Each edge is stored as vec4 (x1, y1, x2, y2)
    int numEdges;          // Total number of edges
};

out vec4 FragColor;

// Function to check if a point is on the correct side of an edge
bool isBetweenEdges(float y, vec4 edge) {
    // Extract edge coordinates
    float x1 = edge.x, y1 = edge.y;
    float x2 = edge.z, y2 = edge.w;

    // Skip horizontal edges
    if (y1 == y2) return false;

    // Ensure y1 <= y2 for consistency
    if (y1 > y2) {
        float tempX = x1, tempY = y1;
        x1 = x2; y1 = y2;
        x2 = tempX; y2 = tempY;
    }

    // Check if y is within the edge's vertical range
    if (y < y1 || y > y2) return false;

    // Compute x-intercept of the edge at the given y
    float xIntercept = x1 + (x2 - x1) * (y - y1) / (y2 - y1);

    // Return true if the x-intercept exists
    return gl_FragCoord.x >= xIntercept;
}

void main() {
    // Get the current pixel's y-coordinate in screen space
    float fragY = gl_FragCoord.y;

    // Count how many edges this fragment is "inside"
    int crossingCount = 0;

    for (int i = 0; i < numEdges; ++i) {
        if (isBetweenEdges(fragY, edges[i])) {
            crossingCount++;
        }
    }

    // Inside if the number of crossings is odd
    if (crossingCount % 2 == 1) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Inside: White
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Outside: Black
    }
}
