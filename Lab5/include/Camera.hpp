#pragma once
#include "Matrix.hpp"

class Camera {
public:
    Camera() noexcept = default;
    virtual ~Camera() = default;
    
    void move(const glm::vec3& move_vector);
    void rotate(const glm::vec3& rotation_vector);
    void scale(bool mode);
    
    glm::vec3 get_eye() const;
    glm::vec3 get_target() const;
    glm::vec3 get_up() const;
    float get_scale() const;
    
private:
    glm::vec3 m_eye{0.0, 0.0, 5.0};
    glm::vec3 m_target{0, 0, 0};
    glm::vec3 m_up{0, 1, 0};
    float m_scale_factor{1.0};
    float m_yaw{-PI / 2};
    float m_pitch{};
};