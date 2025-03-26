#include "Model.hpp"
#include <algorithm>
#include <ranges>

void Model::set_data(Vertices &&vertices, Faces&& faces, Normals&& normals) {
    m_points.clear();
    auto points_zip = std::ranges::zip_view(vertices, normals);
    for (auto&& [vertex, normal] : points_zip){
        m_points.emplace_back(Point{
            std::move(vertex), 
            std::move(normal) 
        });
    }
    m_faces = faces;
}

Points Model::get_points() const {
    return m_points;
}

Faces Model::get_faces() const {
    return m_faces;
}
