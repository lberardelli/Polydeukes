//
//  cube.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-17.
//

#ifndef cube_h
#define cube_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Arcball.h"
#include "../model/Camera.h"
#include "ShaderProgram.h"
#include "../model/light.hpp"
#include "vector.h"
#include "../model/cube.h"

class cube {
    
private:
    static float fieldOfView;
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        fieldOfView += yoffset;
    }
    
    
    float axies[12] = {
        0.0,0.0,0.0,
        1.0,0.0,0.0,
        0.0,1.0,0.0,
        0.0,0.0,1.0
    };
    
    float frustum[24] = {
        -0.05523, -0.0414, -0.1,    //(l,b,n)
        -0.05523, 0.0414, -0.1,     //(l,t,n)
        0.05523, -0.0414, -0.1,     //(r,b,n)
        0.05523, 0.0414, -0.1,      //(r,t,n)
        -55.23, -41.4, -100,        //(l,b,f)
        -55.23, 41.4, -100,         //(l,t,f)
        55.23, -41.4, -100,         //r,b,f)
        55.23, 41.4, -100           //(r,t,f)
    };
    
    unsigned int axisIndices[6] = {
        0,1,
        0,2,
        0,3
    };
    
    unsigned int frustumIndices[24] = {
        //front
        0,1,
        1,3,
        3,2,
        2,0,
        //back
        4,5,
        5,7,
        7,6,
        6,4,
        //connecting lines
        1,5,
        3,7,
        2,6,
        0,4
    };
    
public:
    
    void render(GLFWwindow* window, ShaderProgram shaderProgram) {}
        
//        unsigned int VAO[3],VBO[3];
//
//        //set up axis drawer
//        unsigned int EBO[2];
//        glBindVertexArray(VAO[1]);
//        glGenBuffers(2, EBO);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(axies), axies, GL_STATIC_DRAW);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(axisIndices), axisIndices, GL_STATIC_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
//        glEnableVertexAttribArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//
//        //set up square drawer
//        glBindVertexArray(VAO[2]);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(frustum), frustum, GL_STATIC_DRAW);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frustumIndices), frustumIndices, GL_STATIC_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
//        glEnableVertexAttribArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//
//        Camera camera;
//        //arcball::setup(window, &camera);
//        light light(glm::vec3(1.0,1.0,1.0), glm::vec3(1.0, 2.0, 4.0));
//        glfwSetScrollCallback(window, scroll_callback);
//        ShaderProgram lightSourceProgram("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/LearnOpenGLProject1/LearnOpenGLProject1/shaders/lightsourcevs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/LearnOpenGLProject1/LearnOpenGLProject1/shaders/lightsourceshader.glsl");
//        ShaderProgram
//        axisShader("/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/LearnOpenGLProject1/LearnOpenGLProject1/shaders/axisvs.glsl", "/Users/lawrenceberardelli/Documents/coding/c++/learnopengl/LearnOpenGLProject1/LearnOpenGLProject1/shaders/axisfs.glsl");
//        axisShader.init();
//        lightSourceProgram.init();
//        //CubeFactory cubeFactory;
//        //Cube centralCube = *cubeFactory.buildCube();
//        while (!glfwWindowShouldClose(window)) {
//            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            glEnable(GL_DEPTH_TEST);
//            glm::mat4 view = camera.viewingTransformation();
//            glm::vec3 cameraPosition = camera.getPosition();
//            glm::mat4 projection = glm::perspective(glm::radians(fieldOfView), (float)800 / (float)600, 0.1f, 100.0f);
//            glm::mat4 model = glm::mat4(1.0f);
//
//            /*
//             The key question is how can i turn this into a loop over the scene.
//             A consideration is how do we swap out ShaderPrograms.
//             A potential answer is having a scene be made of groups that share the same shader programs.
//             Groups are scenes, scenes are made up of groups, groups are made up of shapes.
//             */
//            shaderProgram.bind();
//            shaderProgram.setMat4("view", view);
//            shaderProgram.setMat4("projection", projection);
//
//            //set the lighting uniforms
//            shaderProgram.setVec3("aColour", glm::vec3(1.0f, 0.5f, 0.31f));
//            shaderProgram.setVec3("lightColour", light.colour);
//            shaderProgram.setVec3("lightPosition", light.position);
//            shaderProgram.setVec3("eye", cameraPosition);
//            centralCube.render(shaderProgram);
//
////            glBindVertexArray(VAO[0]);
////            glDrawArrays(GL_TRIANGLES, 0, 36);
//
//            model = glm::mat4(1.0f);
//            model = glm::rotate(model, (float)glfwGetTime()*glm::radians(50.0f), glm::vec3(-1.0,1.0f,0.0f));
//            model = glm::translate(model, glm::vec3(2.5,2.5,2.5));
//            model = glm::scale(model, glm::vec3(0.25f,0.25f,0.25f));
//            shaderProgram.setMat4("model", model);
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//
//            model = glm::mat4(1.0f);
//            model = glm::rotate(model, (float)glfwGetTime()*glm::radians(100.0f), glm::vec3(1.0,0.0f,0.0f));
//            model = glm::translate(model, glm::vec3(0.0,1.5,1.5));
//            model = glm::scale(model, glm::vec3(0.25f,0.25f,0.25f));
//            shaderProgram.setMat4("model", model);
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//
//            //draw the light source
//            shaderProgram.unbind();
//            lightSourceProgram.bind();
//            model = glm::mat4(1.0f);
//            model = glm::translate(model, light.position);
//            model = glm::scale(model, glm::vec3(0.33, 0.33,0.33));
//            lightSourceProgram.setMat4("model", model);
//            lightSourceProgram.setMat4("view", view);
//            lightSourceProgram.setMat4("projection", projection);
//            lightSourceProgram.setVec3("colour", glm::vec3(1.0,1.0,1.0));
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//
//            //draw the viewing frustum
////            model = cameraView;
////            lightSourceProgram.setMat4("model", model);
////            glBindVertexArray(VAO[2]);
////            glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
////            lightSourceProgram.unbind();
//
//
//            //draw the axies
//            axisShader.bind();
//            model = glm::mat4(1.0f);
//            axisShader.setMat4("model", model);
//            axisShader.setMat4("view", view);
//            axisShader.setMat4("projection", projection);
//            glBindVertexArray(VAO[1]);
//            glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
//            GLenum error = glGetError();
//            if (error != GL_NO_ERROR) {
//                std::cout << "OpenGL Error: " << error << std::endl;
//            }
//
//            glBindBuffer(GL_ARRAY_BUFFER, 0);
//            glBindVertexArray(0);
//            axisShader.unbind();
//
//            glfwSwapBuffers(window);
//            glfwPollEvents();
};


#endif /* cube_h */
