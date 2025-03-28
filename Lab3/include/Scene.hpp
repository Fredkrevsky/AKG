#pragma once
#include "Model.hpp"
#include "Parser.hpp"
#include "Camera.hpp"

class Scene final {
public:
    Scene() noexcept = default;

    [[nodiscard]] bool initialize();
    void rotate_model(const Vector4& rotate_vector);
    void move_model(const Vector4& move_vector);
    void update_points();

    Points get_points() const;
    Faces get_faces() const;

private:
    constexpr static auto MODEL_FILE_PATH = "../models/bmw.obj";

    Model m_model;
    Vector4 m_model_position, m_model_rotation;
    Points m_points;
    Faces m_faces;
};