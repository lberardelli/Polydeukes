//
//  Program.h
//  LearnOpenGLProject1
//
//  Created by Lawrence Berardelli on 2023-11-26.
//

#ifndef Program_h
#define Program_h

#include <string>
#include <iostream>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gtc/type_ptr.hpp>

class ShaderProgram {
  
private:
    
    std::string vertexShaderName;
    std::string fragmentShaderName;
    unsigned int pid;
    
    std::string readShaderFromFile(std::string& shader) {
        std::string content;
        std::ifstream fileStream(shader, std::ios::in);

        if(!fileStream.is_open()) {
            std::cerr << "Could not read file " << shader << ". File does not exist." << std::endl;
            return "";
        }

        std::string line = "";
        while(!fileStream.eof()) {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();
        return content;
    }
    
public:
    
    ShaderProgram(std::string vertexShaderName, std::string fragmentShaderName) : vertexShaderName{vertexShaderName}, fragmentShaderName{fragmentShaderName} {}
    
    void init() {
        //compile the vertex shader
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::string vshaderCodeString = readShaderFromFile(vertexShaderName);
        char* vshaderCString = new char[vshaderCodeString.size() + 1]{};
        for (int i = 0; i < vshaderCodeString.size(); ++i) {
            vshaderCString[i] = vshaderCodeString[i];
        }
        glShaderSource(vertexShader, 1, &vshaderCString, NULL);
        glCompileShader(vertexShader);
        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        
        //compile the fragment shader
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fshaderCode = readShaderFromFile(fragmentShaderName);
        char* fshaderCString = new char[fshaderCode.size() + 1]{};
        for (int i = 0; i < fshaderCode.size(); ++i) {
            fshaderCString[i] = fshaderCode[i];
        }
        glShaderSource(fragmentShader, 1, &fshaderCString, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        
        //linking the shader program
        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        delete[] vshaderCString;
        delete[] fshaderCString;
        pid = shaderProgram;
    }
    
    void bind()
    {
        glUseProgram(pid);
    }

    void unbind()
    {
        glUseProgram(0);
    }
    
    void setMat4(std::string&& name, glm::mat4& data) {
        glUniformMatrix4fv(glGetUniformLocation(pid, name.c_str()), 1, GL_FALSE, glm::value_ptr(data));
    }
    
    void setVec3(std::string&& name, glm::vec3 data) {
        glUniform3fv(glGetUniformLocation(pid, name.c_str()), 1, glm::value_ptr(data));
    }
    
    void setFloat(std::string&& name, float& data) {
        glUniform1fv(glGetUniformLocation(pid, name.c_str()), 1, &data);
    }
    
    void setInt(std::string&& name, int data) {
        glUniform1i(glGetUniformLocation(pid, name.c_str()), data);
    }
};

#endif /* Program_h */
