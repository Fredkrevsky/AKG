#pragma once
#include "Model.hpp"
#include "Parser.hpp"
#include "Camera.hpp"

class Scene final {
public:
    Scene() noexcept = default;

    [[nodiscard]] bool initialize();
    void rotate_model(const glm::vec4& rotate_vector, float delta_time);
    void move_model(const glm::vec4& move_vector, float delta_time);
    void update_points();

    Points get_points() const;
    const Faces& get_faces() const;
    Points get_normals() const;

private:
    constexpr static auto MODEL_FILE_PATH = "../models/logan.obj";

    Model m_model;
    glm::vec4 m_model_position, m_model_rotation;
    Points m_points;
    Faces m_faces;
};