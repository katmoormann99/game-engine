//============================================================================
// Author: Kat Moormann
// File: mesh.cpp
// Purpose: Implements GPU mesh creation, drawing, and resource cleanup.
//============================================================================

#include "engine/rendering/mesh.hpp"
#include "engine/rendering/graphics.hpp"

#include <cstddef>

namespace engine
{

bool GPUMesh::initialize(const MeshData& meshData)
{
    if (meshData.vertices.empty() || meshData.indices.empty())
    {
        return false;
    }

    if (vao_ != 0 || vbo_ != 0 || ebo_ != 0)
    {
        shutdown();
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(MeshVertex), meshData.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.indices.size() * sizeof(std::uint32_t), meshData.indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, normal)));

    indexCount_ = static_cast<std::uint32_t>(meshData.indices.size());

    glBindVertexArray(0);

    return true;
}

void GPUMesh::draw() const
{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GPUMesh::shutdown()
{
    if (ebo_ != 0)
    {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }

    if (vbo_ != 0)
    {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }

    if (vao_ != 0)
    {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }

    indexCount_ = 0;
}

} // namespace engine