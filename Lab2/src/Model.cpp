#include "Model.hpp"
#include <algorithm>
#include <iostream>

void Model::set_data(Vertices &&vertices, Faces&& faces, Normals&& normals) {
    m_vertices = vertices;
    m_faces = faces;
    m_normals = normals;
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
