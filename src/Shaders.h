//
// Created by cew05 on 06/07/2024.
//

#ifndef UNTITLED7_SHADERS_H
#define UNTITLED7_SHADERS_H

#include <fstream>
#include <glew.h>

#include "CoreGlobals.h"

static std::string LoadShaderSourceFromFile(const std::string& _path) {
    // Shader program is loaded into a single string (source)
    std::string source {};
    std::string line {};

    std::fstream shaderSource(_path);
    if (!shaderSource.is_open()) {
        LogError("Failed to open shader source file", _path.c_str(), true);
        return source;
    }

    // Read source
    while (std::getline(shaderSource, line)) source += line + '\n';
    shaderSource.close();

    return source;
}

static unsigned int CompileShader(unsigned int type, const std::string &source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        LogError("GL shader compilation error", message, false);
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string &_vertexShader, const std::string &_fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, _vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, _fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


#endif //UNTITLED7_SHADERS_H
