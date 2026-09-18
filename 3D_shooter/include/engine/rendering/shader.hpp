//============================================================================
// Author: Kat Moormann
// File: shader.hpp
// Purpose: Owns an OpenGL shader program and provides basic uniform uploads.
//============================================================================

#pragma once

#include <cstdint>
#include <string>

namespace engine
{

class Shader
{
public:
    Shader() = default;
    ~Shader() = default;

    bool initialize(const std::string& vertexPath, const std::string& fragmentPath);

    void use() const;
    void setMatrix4(const std::string& name, const float* matrix) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setFloat( const std::string& name, float value) const;
    void setInt(const std::string &name, int value) const;

    void shutdown();

private:
    std::uint32_t program_ = 0;
};

} // namespace engine