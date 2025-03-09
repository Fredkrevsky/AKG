#pragma once
#include "Matrix.hpp"


class Model final {
public:
    Model() noexcept = default;

    void rotate(const Point& rotate_vector);
    void move(const Point& move_vector);
    
    void set_data(Vertices&& vertices, Faces&& faces);
    Vertices get_vertices() const;
    Faces get_faces() const;
    Normals get_normals() const;

private:
    void calculate_normals();

private:
    Vertices m_vertices;
    Faces m_faces;
    Normals m_normals;
};