//
//  glyph.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2025-01-22.
//

#ifndef glyph_h
#define glyph_h

#include "shape.h"
#include <vector>
#include <glm.hpp>
#include <glad/glad.h>


class Glyph : public Shape {
private:
    GLuint ubo, vao, vbo, ebo;
    int numEdges{};

public:
    
    float boundingBox[4];
    
    virtual void render(ShaderProgram shaderProgram) override {
        shaderProgram.setVec2("resolution", glm::vec2(1600,900));
        shaderProgram.setMat4("model", modellingTransform);
        GLuint blockIndex = glGetUniformBlockIndex(shaderProgram.pid, "EdgeData");
        glUniformBlockBinding(shaderProgram.pid, blockIndex, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    virtual std::shared_ptr<Shape> clone() override {
        return nullptr;
    }
    
    Glyph(const std::vector<std::vector<glm::vec2>>& worldSpaceBezierPaths) {
        std::vector<glm::vec4> edges{};
        std::vector<glm::vec2> polygon{};
        for (auto path : worldSpaceBezierPaths) {
            for (int i = 0; i < path.size(); ++i) {
                int next = i + 1;
                if (i == path.size()-1) {
                    next = 0;
                }
                edges.push_back(glm::vec4(path[i].x, path[i].y, path[next].x, path[next].y));
                polygon.push_back(glm::vec2(path[i].x, path[i].y));
            }
        }
        numEdges = (int)edges.size();
        //compute the bounding box
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxX = -1 * std::numeric_limits<float>::max();
        float maxY = -1 * std::numeric_limits<float>::max();
        for (const auto& vertex : polygon) {
            minX = std::min(minX, vertex.x);
            minY = std::min(minY, vertex.y);
            maxX = std::max(maxX, vertex.x);
            maxY = std::max(maxY, vertex.y);
        }
        
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 1024 + sizeof(int), nullptr, GL_STATIC_DRAW);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4) * 1024, edges.data());

        // Upload the number of edges
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 1024, sizeof(int), &numEdges);
        
        float quadVertices[] = {
            minX, minY,
            maxX, minY,
            maxX, maxY,
            minX, maxY
        };
        
        boundingBox[0] = minX; boundingBox[1] = minY; boundingBox [2] = maxX; boundingBox[3] = maxY;

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    
};


#endif /* glyph_h */
