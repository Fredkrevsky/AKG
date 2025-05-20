#include <memory>
#include <algorithm>
#include <ranges>

#include "Scene.hpp"
#include "Color.hpp"

using namespace std::string_literals;

bool Scene::initialize() {
    std::string file_path{MODEL_FILE_PATH};
    std::string file_format = Parser::get_format(file_path);
    
    auto parser = Parser::create_parser(file_format);
    if (!parser){
        return false;
    }

    parser->parse_file(file_path);
    m_faces = parser->get_faces();
    m_vertices = parser->get_vertices();
    m_normals = parser->get_normals();
    m_texture_vertices = parser->get_texture_vertices();

    update_points();
    return true;
}

void Scene::rotate_model(const glm::vec3& rotate_vector) {
    m_model_rotation += rotate_vector;
    update_points();
}

void Scene::move_model(const glm::vec3& move_vector) {
    m_model_position += move_vector;
    update_points();
}

void Scene::update_points() {
    auto move_matrix = create_move_matrix(m_model_position);
    auto rotation_matrix = create_rotation_matrix(m_model_rotation);
    auto cached_matrix = move_matrix * rotation_matrix;

    std::ranges::transform(m_vertices, m_vertices.begin(), [&](const Vertex& vertex){
        return glm::vec3{cached_matrix * glm::vec4{vertex, 1.0}};
    });

    std::ranges::transform(m_normals, m_normals.begin(), [&](const Vertex& normal){
        return glm::vec3{cached_matrix * glm::vec4{normal, 1.0}};
    });

    m_model_position = {};
    m_model_rotation = {};
}

const Vertices& Scene::get_vertices() const {
    return m_vertices;
}

const Faces& Scene::get_faces() const {
    return m_faces;
}

const Vertices& Scene::get_normals() const {
    return m_normals;
}

const TextureVertices& Scene::get_texture_vertices() const {
    return m_texture_vertices;
}
