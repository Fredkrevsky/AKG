#pragma once
#include "Parser.hpp"
#include "Camera.hpp"

class Scene {
public:
    Scene() noexcept = default;
    virtual ~Scene() = default;

    [[nodiscard]] bool initialize();
    void rotate_model(const glm::vec3& rotate_vector);
    void move_model(const glm::vec3& move_vector);
    void update_points();

    const Vertices& get_vertices() const;
    const Faces& get_faces() const;
    const Vertices& get_normals() const;
    const TextureVertices& get_texture_vertices() const;

private:
    constexpr static auto MODEL_FILE_PATH = "../model/model.obj";

    glm::vec3 m_model_position{};
    glm::vec3 m_model_rotation{};
    Vertices m_vertices;
    Faces m_faces;
    Vertices m_normals;
    TextureVertices m_texture_vertices;
};