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
    return true;
}

void Scene::set_camera(std::shared_ptr<Camera> camera) {
    m_camera = camera;
}

void Scene::rotate_model(const Vector4D &rotate_vector) {
    m_model_rotation += rotate_vector;
}

void Scene::move_model(const Vector4D &move_vector) {
    m_model_position += move_vector;
}

Points Scene::get_points() const {
    auto move_matrix = create_move_matrix(m_model_position);
    auto rotation_matrix = create_rotation_matrix(m_model_rotation);
    auto cached_matrix = move_matrix * rotation_matrix;

    auto fill_vertices_color = [&](Points& points) {
        auto eye = m_camera->get_eye();

        std::ranges::for_each(points, [&](Point& point){
            auto& [vertex, normal, color] = point;
            vertex *= cached_matrix;
            Vector4D new_normal = normal;
            new_normal *= rotation_matrix;
            new_normal.normalize();
    
            Vector4D sun = (eye - vertex).normalize();
            double intensity = new_normal.dot(sun);
            color = Color::from_intensity(intensity);
        });
    };

    auto transform_vertices = [&](Points& points) {
        auto view_matrix = m_camera->get_view_matrix();
        auto viewport_matrix = m_camera->get_viewport_matrix();
        auto scale_matrix = m_camera->get_scale_matrix();
        auto projection_matrix = m_camera->get_projection_matrix();
        auto cached_matrix = projection_matrix * view_matrix * scale_matrix;
    
        std::ranges::for_each(points, [&](Point& point) {
            auto& [vertex, normal, color] = point;
            vertex *= cached_matrix;
            if (vertex.z < -vertex.w || vertex.z > vertex.w) {
                vertex.w = 0;
                return;
            }
            if (vertex.w != 0) {
                vertex *= 1 / vertex.w;
            }
            vertex *= viewport_matrix;
        });
    };

    auto points = m_model.get_points();
    fill_vertices_color(points);
    transform_vertices(points);
    return points;
}

Faces Scene::get_faces() const {
    auto faces = m_model.get_faces();
    return faces;
}
