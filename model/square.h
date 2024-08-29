//
//  square.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2024-03-25.
//

#include "shape.h"
#include "mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "clickable.h"
#include "Camera.h"
#include "../control/ShaderProgram.h"

#ifndef square_h
#define square_h

class SquareFactory;
class SquareBuilder;
class ShapeBuilder;
class IconBuilder;

class Square : public Shape {
    friend SquareFactory;
    friend SquareBuilder;
    
protected:
    
    unsigned int VAO;
    
    Square(unsigned int VAO, float* vertices) : VAO(VAO)  {
        mesh = Mesh(vertices, 32);
    }
    
    Square(Square& square) : Shape(square), VAO(square.VAO) {}
    
    
public:
    
    void render(ShaderProgram shaderProgram) override {
        shaderProgram.setMat4("model", modellingTransform);
        shaderProgram.setVec3("aColour", colour);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        Shape::renderAABB(getAABB(), shaderProgram);
    }
    
    Square* clone() override {
        return new Square(*this);
    }
    
};

class Icon : public Square {
    friend SquareFactory;
    friend IconBuilder;
    
protected:
    Camera* camera;
    
    Icon(unsigned int VAO, float* vertices, Camera* camera) : Square(VAO, vertices), camera(camera) {}
    
    Icon(Icon& icon) : Square(icon) {
        this->camera = icon.camera;
    }
    
    Icon* clone() override {
        return new Icon(*this);
    }
    
public:
    
    void render(ShaderProgram shaderProgram) override {
        modellingTransform = glm::translate(glm::mat4(1.0f), camera->getPosition());
        glm::vec3 direction = glm::vec3(camera->getDirection().x * 10, camera->getDirection().y * 10, camera->getDirection().z * 10);
        modellingTransform = glm::translate(modellingTransform, direction);
        modellingTransform = modellingTransform * glm::inverse(camera->arcballTransformation());
        modellingTransform = glm::translate(modellingTransform, glm::vec3(2.0f, 2.0f, -2.0f));
        float time = glfwGetTime();
        float angle = glm::radians(45.0f) * time;
        glm::mat4 zoo = modellingTransform * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
        shaderProgram.setMat4("model", zoo);
        shaderProgram.setVec3("aColour", colour);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        Shape::renderAABB(getAABB(), shaderProgram);
    }
    
    void onClick() override {
        colour = glm::vec3(0.0f,1.0f,0.0f);
    }
    
};

class SquareFactory : public ShapeFactory {
    friend SquareBuilder;
    friend ShapeBuilder;
    friend IconBuilder;
    
private:
    
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    float vertices[32] =
    {
      -0.5,-0.5,0.0,0.0,0.0,-1.0, 0.0, 0.0,
      -0.5,0.5,0.0,0.0,0.0,-1.0, 0.0, 1.0,
      0.5,-0.5,0.0,0.0,0.0,-1.0, 1.0, 0.0,
      0.5,0.5,0.0,0.0,0.0,-1.0, 1.0, 1.0
    };
    
    unsigned int indices[6] =
    {
        0, 1, 2,
        1, 2, 3
    };
        
    SquareFactory() {
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    
    Square* build() override {
        return new Square(VAO, vertices);
    }
    
    Icon* buildIcon(Camera* camera) {
        return new Icon(VAO, vertices, camera);
    }
    
};

class SquareBuilder : public ShapeBuilder {
private:
    static SquareFactory* squareFactory;
    
public:
    
    SquareBuilder() {
        if (!squareFactory) {
            squareFactory = new SquareFactory();
        }
        shape = squareFactory->build();
    }

};

class IconBuilder : public ShapeBuilder {
private:
    static SquareFactory* squareFactory;
public:
    explicit IconBuilder(Camera* camera) {
        if (!squareFactory) {
            squareFactory = new SquareFactory();
        }
        shape = squareFactory->buildIcon(camera);
    }
    
};

#endif /* square_h */
