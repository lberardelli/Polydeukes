//
//  grid.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2025-01-05.
//

#ifndef grid_h
#define grid_h

#include "shape.h"
#include <vector>

class Grid : public Shape {
    
private:
    struct Line {
        glm::vec3 top{};
        glm::vec3 bottom{};
        Line(glm::vec3 top, glm::vec3 bottom) : top(top), bottom(bottom) {}
    };
    
    std::vector<float> vertices{};
    
    unsigned int VAO{};
    unsigned int VBO{};
    
public:
    
    Grid(glm::vec3 fovbottomLeft, glm::vec3 fovTopRight, int nHorizontal, int nVertical) {
        float horizontalDelta = (fovTopRight.x - fovbottomLeft.x)/(float)nVertical;
        float verticalDelta = (fovTopRight.y - fovbottomLeft.y)/(float)nHorizontal;
        colour = glm::vec3(1.0f,1.0f,1.0f);
        for (int i = 0; i < nHorizontal + 1; ++i) {
            Line line(glm::vec3(fovbottomLeft.x, fovTopRight.y - verticalDelta * i, 0.f), glm::vec3(fovTopRight.x, fovTopRight.y - verticalDelta * i, 0.f));
            vertices.push_back(line.top.x);
            vertices.push_back(line.top.y);
            vertices.push_back(line.top.z);
            vertices.push_back(line.bottom.x);
            vertices.push_back(line.bottom.y);
            vertices.push_back(line.bottom.z);
        }
        for (int i = 0; i < nVertical + 1; ++i) {
            Line line(glm::vec3(fovbottomLeft.x + horizontalDelta * i, fovTopRight.y, 0.0f), glm::vec3(fovbottomLeft.x + horizontalDelta * i, fovbottomLeft.y, 0.0f));
            vertices.push_back(line.top.x);
            vertices.push_back(line.top.y);
            vertices.push_back(line.top.z);
            vertices.push_back(line.bottom.x);
            vertices.push_back(line.bottom.y);
            vertices.push_back(line.bottom.z);
        }
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }
    
    void render(ShaderProgram shaderProgram) override {
        shaderProgram.setMat4("model", modellingTransform);
        shaderProgram.setVec3("aColour", colour);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, vertices.size() / 3);
    }
    
    virtual std::shared_ptr<Shape> clone() override {
        return nullptr;
    }
    
    
};


#endif /* grid_h */
