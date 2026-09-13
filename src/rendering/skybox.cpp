#include "engine/rendering/skybox.hpp"
#include "engine/rendering/graphics.hpp"

#include "third_party/stb_image.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace engine
{
    std::string loadTextFile(const std::string& path)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "Failed to open shader: " << path << '\n';
            return {};
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }

    GLuint compileShader(GLenum type, const std::string& source)
    {
        GLuint shader = glCreateShader(type);

        const char* sourcePtr = source.c_str();

        glShaderSource(
            shader,
            1,
            &sourcePtr,
            nullptr
        );

        glCompileShader(shader);

        GLint success = 0;

        glGetShaderiv(
            shader,
            GL_COMPILE_STATUS,
            &success
        );

        if (!success)
        {
            char log[1024];

            glGetShaderInfoLog(
                shader,
                sizeof(log),
                nullptr,
                log
            );

            std::cerr
                << "Skybox shader compilation failed:\n"
                << log
                << '\n';

            glDeleteShader(shader);

            return 0;
        }

        return shader;
    }


    namespace
    {

        const float SKYBOX_VERTICES[] =
        {
            // Back
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            // Left
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            // Right
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            // Front
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            // Top
            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            // Bottom
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
    }

    bool Skybox::initialize(const std::vector<std::string>& faces)
    {
        if (faces.size() != 6)
        {
            std::cerr << "Skybox requires exactly 6 images.\n";
            return false;
        }

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTICES), SKYBOX_VERTICES, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        glBindVertexArray(0);

        glGenTextures(1, &cubemapTexture_);

        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture_);

        for (std::size_t i = 0; i < faces.size(); ++i)
        {
            int width = 0;
            int height = 0;
            int channels = 0;

            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

            if (!data)
            {
                std::cerr << "Failed to load skybox texture: " << faces[i] << '\n';

                return false;
            }

            GLenum format = channels == 4 ? GL_RGBA : GL_RGB;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }

        // NOW configure how OpenGL samples the cubemap
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

        glTexParameteri(
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE
        );

        glTexParameteri(
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_WRAP_R,
            GL_CLAMP_TO_EDGE
        );


        // 6. We're done with this texture for now
        glBindTexture(
            GL_TEXTURE_CUBE_MAP,
            0
        );

        const std::string vertexSource =
            loadTextFile("shaders/skybox.vert");

        const std::string fragmentSource =
            loadTextFile("shaders/skybox.frag");

        if (vertexSource.empty() ||
            fragmentSource.empty())
        {
            return false;
        }

        GLuint vertexShader =
            compileShader(
                GL_VERTEX_SHADER,
                vertexSource
            );

        if (vertexShader == 0)
        {
            return false;
        }

        GLuint fragmentShader =
            compileShader(
                GL_FRAGMENT_SHADER,
                fragmentSource
            );

        if (fragmentShader == 0)
        {
            glDeleteShader(vertexShader);
            return false;
        }

        shaderProgram_ =
            glCreateProgram();

        glAttachShader(
            shaderProgram_,
            vertexShader
        );

        glAttachShader(
            shaderProgram_,
            fragmentShader
        );

        glLinkProgram(shaderProgram_);

        GLint success = 0;

        glGetProgramiv(
            shaderProgram_,
            GL_LINK_STATUS,
            &success
        );

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);


        glUseProgram(shaderProgram_);

        GLint skyboxLocation =
            glGetUniformLocation(
                shaderProgram_,
                "uSkybox"
            );

        glUniform1i(
            skyboxLocation,
            0
        );


        if (!success)
        {
            char log[1024];

            glGetProgramInfoLog(
                shaderProgram_,
                sizeof(log),
                nullptr,
                log
            );

            std::cerr
                << "Skybox shader linking failed:\n"
                << log
                << '\n';

            glDeleteProgram(shaderProgram_);
            shaderProgram_ = 0;

            return false;
        }

        return true;
    }

    void Skybox::draw(const float* view, const float* projection)
    {
        glDepthFunc(GL_LEQUAL);

        glUseProgram(shaderProgram_);

        GLint viewLocation =
            glGetUniformLocation(
                shaderProgram_,
                "uView"
            );

        GLint projectionLocation =
            glGetUniformLocation(
                shaderProgram_,
                "uProjection"
            );

        glUniformMatrix4fv(
            viewLocation,
            1,
            GL_FALSE,
            view
        );

        glUniformMatrix4fv(
            projectionLocation,
            1,
            GL_FALSE,
            projection
        );

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(
            GL_TEXTURE_CUBE_MAP,
            cubemapTexture_
        );

        glBindVertexArray(vao_);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );

        glBindVertexArray(0);

        glBindTexture(
            GL_TEXTURE_CUBE_MAP,
            0
        );

        glDepthFunc(GL_LESS);
    }
    
    void Skybox::shutdown()
    {
        if (vbo_ != 0)
        {
            glDeleteBuffers(
                1,
                &vbo_
            );

            vbo_ = 0;
        }

        if (vao_ != 0)
        {
            glDeleteVertexArrays(
                1,
                &vao_
            );

            vao_ = 0;
        }

        if (cubemapTexture_ != 0)
        {
            glDeleteTextures(
                1,
                &cubemapTexture_
            );

            cubemapTexture_ = 0;
        }

        if (shaderProgram_ != 0)
        {
            glDeleteProgram(shaderProgram_);
            shaderProgram_ = 0;
        }
    }
}