//
//  spline.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2024-10-30.
//

#ifndef spline_h
#define spline_h

#include "shape.h"
#include "../control/ShaderProgram.h"
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Spline : public Shape {
private:
    unsigned int VAO;
    unsigned int VBO;
    float vertices[12]{};
    
public:
    
    Spline(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
        vertices[0] = p0.x; vertices[1] = p0.y; vertices[2] = p0.z;
        vertices[3] = p1.x; vertices[4] = p1.y; vertices[5] = p1.z;
        vertices[6] = p2.x; vertices[7] = p2.y; vertices[8] = p2.z;
        vertices[9] = p3.x; vertices[10] = p3.y; vertices[11] = p3.z;
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    
    Spline(unsigned int VAO, unsigned int VBO, float* vertices) : VAO(VAO), VBO(VBO) {
        for (int i = 0; i<12; ++i) {
            this->vertices[i] = vertices[i];
        }
    }
    
    void updateLocation(int i, glm::vec3 newPosition) {
        vertices[i*3] = newPosition.x;
        vertices[i*3+1] = newPosition.y;
        vertices[i*3+2] = newPosition.z;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }
    
    virtual std::shared_ptr<Shape> clone() {
        return std::shared_ptr<Spline>(new Spline(VAO, VBO, vertices));
    }
    
    virtual void render(ShaderProgram shaderProgram) {
        shaderProgram.setMat4("model", modellingTransform);
        shaderProgram.setVec3("LineColour", glm::vec3(0.749,0.749,0.));
        shaderProgram.setInt("NumSegments", 1);
        shaderProgram.setInt("NumStrips", 100);
        glBindVertexArray(VAO);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glDrawArrays(GL_PATCHES, 0, 4);
    }
    
};


#endif /* spline_h */
