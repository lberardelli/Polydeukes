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
#include "ShaderProgram.h"
#include "../model/vector.h"

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
    
    virtual ~Square() = default;
    
    void render(ShaderProgram shaderProgram) override {
        shaderProgram.setMat4("model", modellingTransform);
        shaderProgram.setVec3("aColour", colour);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    std::shared_ptr<Shape> clone() override {
        auto retval = std::shared_ptr<Square>(new Square(*this));
        retval->referenceToThis = retval;
        return retval;
    }
    
};

class Icon : public Square {
    friend SquareFactory;
    friend IconBuilder;
    
private:
    static int count;
    int currentCount = 0;
    
protected:
    Camera* camera;
    
    Icon(unsigned int VAO, float* vertices, Camera* camera) : Square(VAO, vertices), camera(camera) {}
    
    Icon(Icon& icon) : Square(icon) {
        currentCount = count;
        ++count;
        this->camera = icon.camera;
    }
    
    std::shared_ptr<Shape> clone() override {
        auto retval =  std::shared_ptr<Icon>(new Icon(*this));
        retval->referenceToThis = retval;
        return retval;
    }
    
public:
    
    virtual ~Icon() = default;
    
    void render(ShaderProgram shaderProgram) override {
        glm::mat4 meshTransform = glm::inverse(modellingTransform);
        modellingTransform = glm::translate(glm::mat4(1.0f), camera->getPosition());
        glm::vec3 direction = glm::vec3(camera->getDirection().x * 10, camera->getDirection().y * 10, camera->getDirection().z * 10);
        modellingTransform = glm::translate(modellingTransform, direction);
        modellingTransform = modellingTransform * glm::inverse(camera->arcballTransformation());
        modellingTransform = glm::translate(modellingTransform, glm::vec3(5.0f, 3.0f - currentCount, 1.0f));
        modellingTransform = glm::scale(modellingTransform, glm::vec3(.5f,.5f,.5f));
        shaderProgram.setMat4("model", modellingTransform);
        shaderProgram.setVec3("aColour", colour);
        glm::mat4 tmp = modellingTransform * meshTransform;
        mesh.updatePosition(tmp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        Shape::renderAABB(getAABB(), shaderProgram);
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
    
    std::shared_ptr<Shape> build() override {
        return std::shared_ptr<Square>(new Square(VAO, vertices));
    }
    
    std::shared_ptr<Shape> buildIcon(Camera* camera) {
        return std::shared_ptr<Icon>(new Icon(VAO, vertices, camera));
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
    
    virtual std::shared_ptr<Shape> build() {
        return shape->clone();
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
    
    virtual std::shared_ptr<Shape> build() {
        return shape->clone();
    }
    
};

#endif /* square_h */
