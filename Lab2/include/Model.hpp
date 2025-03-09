#pragma once
#include "Matrix.hpp"


class Model final {
public:
    Model() noexcept = default;

    void set_data(Vertices&& vertices, Faces&& faces, Normals&& normals);

    Vertices get_vertices() const;
    Faces get_faces() const;
    Normals get_normals() const;

private:
    Vertices m_vertices;
    Faces m_faces;
    Normals m_normals;
};