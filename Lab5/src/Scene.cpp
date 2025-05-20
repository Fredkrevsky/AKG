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
    auto vertices = parser->get_vertices();
    m_normals = parser->get_normals();
    m_texture_vertices = parser->get_texture_vertices();

    m_points.clear();
    std::ranges::transform(vertices, std::back_inserter(m_points), [](const auto& vertex){
        return Point{
            vertex, 
            glm::vec4{0, 0, 0, 1.0},
            glm::vec4{} 
        };
    });

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

    std::ranges::for_each(m_points, [&](Point& point){
        auto& [world, screen, normal, texture] = point;
        glm::vec4 world4 = {world, 1.0};
        world4 = cached_matrix * world4;
        world = glm::vec3{world4};
    });

    std::ranges::for_each(m_normals, [&](glm::vec3& normal){
        glm::vec4 normal4 = {normal, 1.0};
        normal4 = cached_matrix * normal4;
        normal = glm::vec3{normal4};
    });

    m_model_position = {};
    m_model_rotation = {};
}

Points Scene::get_points() const {
    return m_points;
}

const Faces& Scene::get_faces() const {
    return m_faces;
}

const Vertices& Scene::get_normals() const {
    return m_normals;
}

const Vertices& Scene::get_texture_vertices() const {
    return m_texture_vertices;
}
