//
//  sierpinski.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-15.
//

#ifndef sierpinski_h
#define sierpinski_h

#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "texture.h"

class sierpinski {
    
public:
    const char* vetexShader = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 transform;\n"
        "in vec2 texCoords;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "out vec2 aTexture;\n"
        "void main() {\n"
        "gl_Position = projection * view * model * transform * vec4(aPos, 1.0f);\n"
        "aTexture = texCoords;"
        "}\0";

    const char* fragmentShader = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D texture1;\n"
        "in vec2 aTexture;\n"
        "void main() {\n"
        "FragColor = texture(texture1, aTexture);\n"
        "}\0";
    
    static void compute(int depth, int maxDepth, unsigned int shaderProgram, glm::vec3 translate, glm::vec3 scale) {
        if (depth == maxDepth) return;
        //do translation and scaling for leftward triangle then draw
        glm::mat4 transformation = glm::mat4(1.0f);
        glm::vec3 currentTranslate = glm::vec3(translate.x-(scale.x / 2.0f), translate.y-(scale.y / 2.0f), translate.z);
        transformation = glm::translate(transformation, currentTranslate);
        transformation = glm::rotate(transformation, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 currentScale = glm::vec3(scale.x / 2.0f, scale.y / 2.0f, scale.z / 2.0f);
        transformation = glm::scale(transformation, currentScale);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transformation));
        glDrawArrays(GL_TRIANGLES, 0 , 3);
        compute(depth+1, maxDepth, shaderProgram, currentTranslate, currentScale);
        //do translation and scaling for rightward triangle then draw
        transformation = glm::mat4(1.0f);
        currentTranslate = glm::vec3(translate.x+(scale.x / 2.0f), translate.y-(scale.x / 2.0f), translate.z);
        transformation = glm::translate(transformation, currentTranslate);
        transformation = glm::rotate(transformation, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        transformation = glm::scale(transformation, currentScale);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transformation));
        glDrawArrays(GL_TRIANGLES, 0 , 3);
        compute(depth+1, maxDepth, shaderProgram, currentTranslate, currentScale);
        //do translation and scaling for upward triangle then draw
        transformation = glm::mat4(1.0f);
        float upTrans = (std::sqrtf(3.0f)/2.0f * currentScale.y) + ((std::sqrtf(3.0f)/2.0f - 1.0f) * currentScale.y);
        currentTranslate = glm::vec3(translate.x, translate.y + upTrans, translate.z);
        transformation = glm::translate(transformation, currentTranslate);
        transformation = glm::rotate(transformation, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        transformation = glm::scale(transformation, currentScale);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transformation));
        glDrawArrays(GL_TRIANGLES, 0 , 3);
        compute(depth+1, maxDepth, shaderProgram, currentTranslate, currentScale);
    }
    
    static void startRendering(GLFWwindow* window, unsigned int shaderProgram) {
        float vertices[] = {
            //locations                                 // texture coords
            -0.5f, -1.0f + std::sqrtf(3.0f)/2.0f, 0.0f, 0.0f, 1.0f, //top left
            0.5f, -1.0f + std::sqrtf(3.0f)/2.0f, 0.0f, 1.0f, 1.0f,  //top right
            0.0f, -1.0f, 0.0f,                         0.5f, 0.0f   //bottom center
        };
        
        unsigned int VAO, VBO;
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), ((void*)0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        glm::mat4 camera = glm::mat4(1.0f);
        camera = glm::translate(camera, glm::vec3(0.0f,0.0f,-3.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera));
        
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        unsigned int texture = texture::generateTexture("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/LearnOpenGLProject1/LearnOpenGLProject1/container.jpg");
        while (!glfwWindowShouldClose(window)) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            
            glBindVertexArray(VAO);
            glUseProgram(shaderProgram);
            
            glm::mat4 transformation = glm::mat4(1.0f);
            transformation = glm::rotate(transformation, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transformation));
            glDrawArrays(GL_TRIANGLES, 0, 3);
            compute(1,8, shaderProgram, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,1.0f,1.0f));
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
    }
};


#endif /* sierpinski_h */
