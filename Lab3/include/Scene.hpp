#pragma once
#include "Model.hpp"
#include "Parser.hpp"
#include "Camera.hpp"

class Scene final {
public:
    Scene() noexcept = default;

    [[nodiscard]] bool initialize();
    void set_camera(std::shared_ptr<Camera> camera);
    void rotate_model(const Vector4D& rotate_vector);
    void move_model(const Vector4D& move_vector);
    void invalidate_points();

    Points get_points();
    Faces get_faces() const;

private:
    constexpr static auto MODEL_FILE_PATH = "../models/logan.obj";

    Model m_model;
    Vector4D m_model_position, m_model_rotation;
    Points m_points;
    Faces m_faces;

    std::shared_ptr<Camera> m_camera;
    bool m_is_changed{false};
};