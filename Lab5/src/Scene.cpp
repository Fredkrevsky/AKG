#include <memory>
#include <algorithm>
#include <ranges>
#include <iostream>

#include "Scene.hpp"
#include "Color.hpp"

using namespace std::string_literals;

namespace {

constexpr auto MODEL_FILE_PATH_PREFIX = "../model/Knight/Hell Knight";
constexpr int FRAMES_COUNT = 77;
constexpr int FPS = 60;

}

bool Scene::initialize() {
    auto parser = Parser::create_parser("obj");
    if (!parser) {
        return false;
    }

    for (int i = 0; i < FRAMES_COUNT; ++i) {
        char frame_str[5];
        std::snprintf(frame_str, sizeof(frame_str), "%04d", i);
        std::string path = std::string(MODEL_FILE_PATH_PREFIX) + frame_str + ".obj";

        parser->parse_file(path);

        Model current;
        current.m_faces = parser->get_faces();
        current.m_normals = parser->get_normals();
        current.m_texture_vertices = parser->get_texture_vertices();
        current.m_vertices = parser->get_vertices();
        current.m_mtls = parser->get_mtls();

        m_models.emplace_back(std::move(current));
        std::cout << "Loaded: " << path << '\n';
    }

    return true;
}

void Scene::rotate_model(const glm::vec3& rotate_vector) {
    m_model_rotation += rotate_vector;
}

void Scene::move_model(const glm::vec3& move_vector) {
    m_model_position += move_vector;
}

void Scene::update() {
    constexpr float seconds_per_frame = 1.0f / FPS;
    
    m_elapsed_time += m_clock.restart();
    
    while (m_elapsed_time.asSeconds() >= seconds_per_frame) {
        m_index = (m_index + 1) % FRAMES_COUNT;
        m_elapsed_time -= sf::seconds(seconds_per_frame);
    }
}

Vertices Scene::get_vertices() const {
    auto move_matrix = create_move_matrix(m_model_position);
    auto rotation_matrix = create_rotation_matrix(m_model_rotation);
    auto cached_matrix = move_matrix * rotation_matrix;

    Vertices vertices = m_models[m_index].m_vertices;
    std::ranges::transform(vertices, vertices.begin(), [&](const Vertex& vertex){
        return glm::vec3{cached_matrix * glm::vec4{vertex, 1.0}};
    });

    return vertices;
}

const Faces& Scene::get_faces() const {
    return m_models[m_index].m_faces;
}

Vertices Scene::get_normals() const {
    auto move_matrix = create_move_matrix(m_model_position);
    auto rotation_matrix = create_rotation_matrix(m_model_rotation);
    auto cached_matrix = move_matrix * rotation_matrix;

    Vertices normals = m_models[m_index].m_normals;
    std::ranges::transform(normals, normals.begin(), [&](const Vertex& normal){
        return glm::vec3{cached_matrix * glm::vec4{normal, 1.0}};
    });

    return normals;
}

const TextureVertices& Scene::get_texture_vertices() const {
    return m_models[m_index].m_texture_vertices;
}

const Mtls& Scene::get_mtls() const {
    return m_models[m_index].m_mtls;
}
