//
//  axis.h
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2024-05-24.
//

#ifndef axis_h
#define axis_h

#include "shape.h"
#include <glad/glad.h>

class AxiesFactory;
class AxiesBuilder;

class Axies : public Shape {
    friend AxiesFactory;
protected:
    unsigned int VAO[3];
    Axies(Axies& that) : Shape(that) {
        for (int i = 0; i < 3; ++i) {
            VAO[i] = that.VAO[i];
        }
    }
    
    Axies(unsigned int* VAO, float* vertices) {
        for (int i = 0; i < 3; ++i) {
            this->VAO[i] = VAO[i];
        }
        mesh = Mesh(vertices, 32);
    }
    
public:
    
    Shape* clone() override {
        return new Axies(*this);
    }
    
    void render(ShaderProgram shaderProgram) override {
        shaderProgram.setMat4("model", modellingTransform);
        for (int i = 0; i < 3; ++i) {
            glm::vec3 colour;
            if (i == 0) {
                colour = glm::vec3(1.0f,0.0f,0.0f);
            }
            else if (i == 1) {
                colour = glm::vec3(0.0f,1.0f,0.0f);
            }
            else {
                colour = glm::vec3(0.0f,0.0f,1.0f);
            }
            glBindVertexArray(VAO[i]);
            shaderProgram.setVec3("aColour", colour);
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
        }
        Shape::renderAABB(getAABB(), shaderProgram);
    }
    
};


class AxiesFactory : public ShapeFactory {
    friend AxiesBuilder;
protected:
    
    float axis_vertices[32] = {
        0.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,
        1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 1.0f,0.0f,
        0.0f,1.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,1.0f,
        0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, 1.0f,1.0f
    };
    
    float x_axis[16] = {
        0.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,
        1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 1.0f,0.0f
    };
    
    float y_axis[16] = {
        0.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,
        0.0f,1.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,1.0f
    };
    
    float z_axis[16] = {
        0.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,
        0.0f,0.0f,1.0f, 0.0f,0.0f,1.0f, 1.0f,1.0f
    };
    
    unsigned int axisIndices[2] = {
        0,1
    };
    
    unsigned int VAO[3];
    
    
    
    AxiesFactory() {
        unsigned int VBO[3]{};
        unsigned int EBO[3]{};
        glGenVertexArrays(3, VAO);
        glGenBuffers(3, EBO);
        glGenBuffers(3, VBO);
        for (int i = 0; i < 3; ++i) {
            float* cpy;
            if (i == 0) {
                cpy = x_axis;
            }
            else if (i == 1) {
                cpy = y_axis;
            }
            else {
                cpy = z_axis;
            }
            glBindVertexArray(VAO[i]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
            glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), cpy, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(axisIndices), axisIndices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }
        
    Shape* build() override {
        return new Axies(VAO, axis_vertices);
    }
    
};

class AxiesBuilder : public ShapeBuilder {
private:
    static AxiesFactory* factory;
public:
    AxiesBuilder() {
        if (!factory) {
            factory = new AxiesFactory();
        }
        shape = factory->build();
    }
};

AxiesFactory* AxiesBuilder::factory = 0;


#endif /* axis_h */
