//============================================================================
// Author: Kat Moormann
// File: shader.cpp
// Purpose: Implements OpenGL shader loading, compilation, linking, and
//          uniform uploads.
//============================================================================

#include "engine/rendering/shader.hpp"
#include "engine/rendering/graphics.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace engine
{

namespace
{

std::string loadTextFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << path << '\n';
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source, const std::string& path)
{
    GLuint shader = glCreateShader(type);
    const char* sourcePtr = source.c_str();

    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Failed to compile shader: " << path << '\n' << log << '\n';
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

} // anonymous namespace

bool Shader::initialize(const std::string& vertexPath, const std::string& fragmentPath)
{
    if (program_ != 0)
    {
        shutdown();
    }

    const std::string vertexSource = loadTextFile(vertexPath);
    const std::string fragmentSource = loadTextFile(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty())
    {
        return false;
    }

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);

    if (vertexShader == 0)
    {
        return false;
    }

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return false;
    }

    program_ = glCreateProgram();

    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    GLint success = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success)
    {
        char log[1024];
        glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
        std::cerr << "Failed to link shader program:\nVertex shader: " << vertexPath << "\nFragment shader: " << fragmentPath << '\n' << log << '\n';

        glDeleteProgram(program_);
        program_ = 0;
        return false;
    }

    return true;
}

void Shader::use() const
{
    glUseProgram(program_);
}

void Shader::setMatrix4(const std::string& name, const float* matrix) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform3f(location, x, y, z);
}

void Shader::setFloat(const std::string& name, float value) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform1f(location, value);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform2f(location, x, y);
}

void Shader::setInt(const std::string& name, int value) const
{
    const GLint location = glGetUniformLocation(program_, name.c_str());
    glUniform1i(location, value);
}

void Shader::shutdown()
{
    if (program_ != 0)
    {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

} // namespace engine