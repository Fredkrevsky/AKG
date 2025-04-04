#pragma once
#include "Matrix.hpp"


class Model final {
public:
    Model() noexcept = default;

    void rotate(const Point& rotate_vector);
    void move(const Point& move_vector);
    
    void set_vertices(Vertices&& vertices);
    void set_faces(Faces&& faces);
    Vertices get_vertices() const;
    Faces get_faces() const;

private:
    Vertices m_vertices;
    Faces m_faces;
};