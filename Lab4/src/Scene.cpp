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

    m_faces = faces;
    m_model.set_data(std::move(vertices), 
                     std::move(faces), 
                     std::move(normals));

    m_points = m_model.get_points();
    update_points();

    return true;
}

void Scene::rotate_model(const glm::vec4 &rotate_vector, float delta_time) {
    m_model_rotation += rotate_vector * delta_time;
    update_points();
}

void Scene::move_model(const glm::vec4 &move_vector, float delta_time) {
    m_model_position += move_vector * delta_time;
    update_points();
}

void Scene::update_points() {
    auto move_matrix = create_move_matrix(m_model_position);
    auto rotation_matrix = create_rotation_matrix(m_model_rotation);
    auto cached_matrix = move_matrix * rotation_matrix;

    m_points = m_model.get_points();
    std::ranges::for_each(m_points, [&](Point& point){
        auto& [world, screen, normal] = point;
        glm::vec4 world4 = {world.x, world.y, world.z, 1.0};
        glm::vec4 normal4 = {normal.x, normal.y, normal.z, 1.0};
        world4 = cached_matrix * world4;
        normal4 = cached_matrix * normal4;
        world = glm::vec3{world4.x, world4.y, world4.z};
        normal = glm::vec3{normal4.x, normal4.y, normal4.z};
    });
}

Points Scene::get_points() const {
    return m_points;
}

const Faces& Scene::get_faces() const {
    return m_faces;
}
