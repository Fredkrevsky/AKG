#include "Model.hpp"
#include <algorithm>
#include <iostream>

void Model::rotate(const Point &rotate_vector) {
    TransformMatrix rotation_matrix = create_rotation_matrix(rotate_vector);

    std::ranges::for_each(m_vertices, [&](auto& vertex){
        vertex *= rotation_matrix;
    });
}

void Model::move(const Point &move_vector) {
    TransformMatrix move_matrix = create_move_matrix(move_vector);
    std::ranges::for_each(m_vertices, [&](auto& vertex){
        vertex *= move_matrix;
    });
}

void Model::set_data(Vertices &&vertices, Faces&& faces) {
    m_vertices = vertices;
    m_faces = faces;
    calculate_normals();
}

Vertices Model::get_vertices() const {
    return m_vertices;
}

Faces Model::get_faces() const {
    return m_faces;
}

Normals Model::get_normals() const {
    return m_normals;
}

void Model::calculate_normals() {
    std::ranges::transform(m_faces, std::back_inserter(m_normals), 
        [this](const Face& face)
    {
        Point A = m_vertices[face[0]];
        Point B = m_vertices[face[1]];
        Point C = m_vertices[face[2]];

        Point AB = {B.x - A.x, B.y - A.y, B.z - A.z};
        Point AC = {C.x - A.x, C.y - A.y, C.z - A.z};

        Normal normal;
        normal.x = AB.y * AC.z - AB.z * AC.y;
        normal.y = AB.z * AC.x - AB.x * AC.z;
        normal.z = AB.x * AC.y - AB.y * AC.x;
        normal.normalize();

        return normal;
    }); 
}
