#include "Scene.hpp"
#include <memory>
#include <algorithm>
#include <ranges>
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
    auto faces = parser->get_faces();
    auto vertices = parser->get_vertices();
    auto normals = parser->get_normals();
    m_model.set_data(std::move(vertices), 
                     std::move(faces), 
                     std::move(normals));

    m_points = m_model.get_points();
    invalidate_points();
    return true;
}

void Scene::set_camera(std::shared_ptr<Camera> camera) {
    m_camera = camera;
}

void Scene::rotate_model(const Vector4 &rotate_vector) {
    m_model_rotation += rotate_vector;
    invalidate_points();
}

void Scene::move_model(const Vector4 &move_vector) {
    m_model_position += move_vector;
    invalidate_points();
}

void Scene::invalidate_points(){
    m_is_changed = true;
}

Points Scene::get_points() {
    auto move_matrix = create_move_matrix(m_model_position);
    auto rotation_matrix = create_rotation_matrix(m_model_rotation);
    auto cached_matrix = move_matrix * rotation_matrix;

    auto fill_vertices_color = [&](Points& points) {
        std::ranges::for_each(points, [&](Point& point){
            auto& [vertex, normal, color] = point;
            vertex *= cached_matrix;
            normal *= cached_matrix;
        });
    };

    if (m_is_changed){
        m_points = m_model.get_points();
        fill_vertices_color(m_points);
        m_is_changed = false;
    }
    return m_points;
}

Faces Scene::get_faces() const {
    auto faces = m_model.get_faces();
    return faces;
}
