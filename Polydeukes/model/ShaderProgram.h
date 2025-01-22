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
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gtc/type_ptr.hpp>

class ShaderProgram {
  
private:
    
    std::string vertexShaderName;
    std::string fragmentShaderName;
    
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
    unsigned int pid;
    
    ShaderProgram(std::string vertexShaderName, std::string fragmentShaderName) : vertexShaderName{vertexShaderName}, fragmentShaderName{fragmentShaderName} {}
    
    ShaderProgram(){}
    
    GLuint loadShader(const char* shaderPath, GLenum shaderType) {
        std::ifstream shaderFile(shaderPath);
        if (!shaderFile) {
            std::cerr << "Failed to open shader file: " << shaderPath << std::endl;
            return 0;
        }
        std::stringstream shaderStream{};
        shaderStream << shaderFile.rdbuf();
        std::string shaderCode = shaderStream.str();
        const char* shaderSource = shaderCode.c_str();

        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderSource, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
            return 0;
        }

        return shader;
    }
    
    void createShaderProgram(std::string vertexPath, std::string tessControlPath,
                             std::string tessEvalPath, std::string fragmentPath) {
        GLuint vertexShader = loadShader(vertexPath.c_str(), GL_VERTEX_SHADER);
        GLuint tessControlShader = loadShader(tessControlPath.c_str(), GL_TESS_CONTROL_SHADER);
        GLuint tessEvalShader = loadShader(tessEvalPath.c_str(), GL_TESS_EVALUATION_SHADER);
        GLuint fragmentShader = loadShader(fragmentPath.c_str(), GL_FRAGMENT_SHADER);

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, tessControlShader);
        glAttachShader(shaderProgram, tessEvalShader);
        glAttachShader(shaderProgram, fragmentShader);

        glLinkProgram(shaderProgram);

        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Program linking failed:\n" << infoLog << std::endl;
            return;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(tessControlShader);
        glDeleteShader(tessEvalShader);
        glDeleteShader(fragmentShader);

        pid = shaderProgram;
    }
    
    void createShaderProgram(std::string vertexPath, std::string tessControlPath,
                             std::string tessEvalPath, std::string fragmentPath, std::string geometryPath) {
        GLuint vertexShader = loadShader(vertexPath.c_str(), GL_VERTEX_SHADER);
        GLuint tessControlShader = loadShader(tessControlPath.c_str(), GL_TESS_CONTROL_SHADER);
        GLuint tessEvalShader = loadShader(tessEvalPath.c_str(), GL_TESS_EVALUATION_SHADER);
        GLuint fragmentShader = loadShader(fragmentPath.c_str(), GL_FRAGMENT_SHADER);
        GLuint geometryShader = loadShader(geometryPath.c_str(), GL_GEOMETRY_SHADER);

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, tessControlShader);
        glAttachShader(shaderProgram, tessEvalShader);
        glAttachShader(shaderProgram, fragmentShader);
        glAttachShader(shaderProgram, geometryShader);

        glLinkProgram(shaderProgram);

        GLint success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "Program linking failed:\n" << infoLog << std::endl;
            return;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(tessControlShader);
        glDeleteShader(tessEvalShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(geometryShader);

        pid = shaderProgram;
    }
    
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
    
    void setVec2(std::string&& name, glm::vec2 data) {
        glUniform2fv(glGetUniformLocation(pid, name.c_str()), 1, glm::value_ptr(data));
    }
    
    void setFloat(std::string&& name, float& data) {
        glUniform1fv(glGetUniformLocation(pid, name.c_str()), 1, &data);
    }
    
    void setInt(std::string&& name, int data) {
        glUniform1i(glGetUniformLocation(pid, name.c_str()), data);
    }
};

#endif /* Program_h */
