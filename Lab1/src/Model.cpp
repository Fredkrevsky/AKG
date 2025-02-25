#include "Model.hpp"

void Model::rotate(const Point &rotate_vector) {
    TransformMatrix rotation_matrix = create_rotation_matrix(rotate_vector);

    std::ranges::for_each(m_vertices, [&](auto& vertex){
        vertex = vertex * rotation_matrix;
    });
}

void Model::move(const Point &move_vector) {
    TransformMatrix move_matrix = create_move_matrix(move_vector);
    std::ranges::for_each(m_vertices, [&](auto& vertex){
        vertex = vertex * move_matrix;
    });
}

void Model::set_vertices(Vertices &&vertices)
{
    m_vertices = vertices;
}

void Model::set_faces(Faces &&faces) {
    m_faces = faces;
}

Vertices Model::get_vertices() const {
    return m_vertices;
}

Faces Model::get_faces() const {
    return m_faces;
}
