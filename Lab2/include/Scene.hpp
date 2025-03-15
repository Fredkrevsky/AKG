#pragma once
#include "Model.hpp"
#include "Parser.hpp"
#include "Camera.hpp"

class Scene final {
public:
    Scene() noexcept = default;

    [[nodiscard]] bool initialize();
    void set_camera(std::shared_ptr<Camera> camera);
    void rotate_model(const Point& rotate_vector);
    void move_model(const Point& move_vector);

    Vertices get_vertices() const;
    Faces get_faces() const;

private:
    constexpr static auto MODEL_FILE_PATH = "../models/logan.obj";

    Model m_model;
    std::shared_ptr<Camera> m_camera;
    Point m_model_position, m_model_rotation;
};