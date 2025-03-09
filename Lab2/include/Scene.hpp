#pragma once
#include "Model.hpp"
#include "Parser.hpp"

class Scene final {
public:
    Scene() noexcept = default;

    [[nodiscard]] bool initialize();
    void rotate_model(const Point& rotate_vector);
    void move_model(const Point& move_vector);

    Vertices get_vertices() const;
    Faces get_faces() const;

private:
    constexpr static auto MODEL_FILE_PATH = "../models/bmw.obj";

    Model m_model;
    Point m_model_position, m_model_rotation;
    Point m_sun{5.0, 5.0, 5.0};
};