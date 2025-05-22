#pragma once
#include "Parser.hpp"
#include "Camera.hpp"
#include <SFML/System/Clock.hpp>

struct Model {
    Vertices m_vertices;
    Faces m_faces;
    Vertices m_normals;
    TextureVertices m_texture_vertices;
    Mtls m_mtls;
};

class Scene {
public:
    Scene() noexcept = default;
    virtual ~Scene() = default;

    [[nodiscard]] bool initialize();
    void rotate_model(const glm::vec3& rotate_vector);
    void move_model(const glm::vec3& move_vector);
    void update();

    Vertices get_vertices() const;
    const Faces& get_faces() const;
    Vertices get_normals() const;
    const TextureVertices& get_texture_vertices() const;
    const Mtls& get_mtls() const;

private:
    glm::vec3 m_model_position{};
    glm::vec3 m_model_rotation{};
    std::vector<Model> m_models;
    int m_index{};

    sf::Clock m_clock;
    sf::Time m_elapsed_time{};
};