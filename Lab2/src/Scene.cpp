#include "Scene.hpp"
#include <memory>
#include <algorithm>
#include <ranges>

using namespace std::string_literals;

bool Scene::initialize() {
    std::string file_path{MODEL_FILE_PATH};
    std::string file_format = Parser::get_format(file_path);
    
    auto parser = Parser::create_parser(file_format);
    if (!parser){
        return false;
    }

    parser->parse_file(file_path);
    auto&& faces = parser->get_faces();
    auto&& vertices = parser->get_vertices();
    auto&& normals = parser->get_normals();
    m_model.set_data(std::move(vertices), 
                     std::move(faces), 
                     std::move(normals));
    return true;
}

void Scene::rotate_model(const Point &rotate_vector) {
    m_model_rotation += rotate_vector;
}

void Scene::move_model(const Point &move_vector) {
    m_model_position += move_vector;
}

Vertices Scene::get_vertices() const {
    
    auto move_matrix = create_move_matrix(m_model_position);
    auto rotation_matrix = create_rotation_matrix(m_model_rotation);
    auto cached_matrix = move_matrix * rotation_matrix;

    auto vertices = m_model.get_vertices();
    auto normals = m_model.get_normals();
    auto zip = std::ranges::zip_view(vertices, normals);

    for (auto&& [vertex, normal] : zip) {
        Point temp_normal = normal;
        Point temp_normal_end = vertex + temp_normal;
        vertex *= cached_matrix;
        temp_normal_end *= cached_matrix;
        temp_normal = (temp_normal_end - vertex).normalize();

        Point sun = m_sun - vertex;
        sun.normalize();

        double intensity = std::max(0.0, temp_normal.dot(sun));
        vertex.color = static_cast<uint8_t>(0xFF * intensity);
    }

    return vertices;
}

Faces Scene::get_faces() const {
    return m_model.get_faces();
}
