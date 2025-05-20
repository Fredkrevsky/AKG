#include "Camera.hpp"
#include <algorithm>

namespace {

constexpr float ROTATION_SENSITIVITY{0.1f};
constexpr float SCALE_SENSITIVITY{0.2f};
constexpr float MIN_SCALE_FACTOR{0.2f};
constexpr float MAX_SCALE_FACTOR{5.0f};
constexpr float CAMERA_SPEED{1.0f};
constexpr glm::vec3 world_up{0.0, 1.0, 0.0};

constexpr float get_radians(float angle) {
    return PI * angle / 180.f;
};

}

void Camera::move(const glm::vec3& move_vector) {
    glm::vec3 forward = glm::normalize(m_target - m_eye);
    glm::vec3 left = glm::normalize(glm::cross(m_up, forward));
    
    glm::vec3 left_move_vector = left * (CAMERA_SPEED * move_vector.x);
    glm::vec3 forward_move_vector = forward * (CAMERA_SPEED * move_vector.y);
    glm::vec3 up_move_vector = world_up * (CAMERA_SPEED * move_vector.z);
    glm::vec3 result_vector = left_move_vector + forward_move_vector + up_move_vector;

    m_eye += result_vector;
    m_target += result_vector;
}

void Camera::rotate(const glm::vec3& rotation_vector) {
    constexpr float MAX_PITCH = get_radians(89.0);

    m_yaw -= rotation_vector.x * ROTATION_SENSITIVITY;
    m_pitch += rotation_vector.y * ROTATION_SENSITIVITY;
    m_pitch = std::clamp(m_pitch, -MAX_PITCH, MAX_PITCH);

    glm::vec3 forward = glm::normalize(glm::vec3{
        std::cos(m_yaw) * std::cos(m_pitch),
        std::sin(m_pitch),
        std::sin(m_yaw) * std::cos(m_pitch)
    });

    glm::vec3 left = glm::normalize(glm::cross(forward, world_up));
    m_up = glm::normalize(glm::cross(left, forward));
    m_target = m_eye + forward;
}

void Camera::scale(bool mode) {
    float new_scale_factor = mode 
        ? m_scale_factor + SCALE_SENSITIVITY
        : m_scale_factor - SCALE_SENSITIVITY;

    m_scale_factor = std::clamp(
        new_scale_factor,
        MIN_SCALE_FACTOR,
        MAX_SCALE_FACTOR
    );
}

glm::vec3 Camera::get_eye() const {
    return m_eye;
}

glm::vec3 Camera::get_target() const {
    return m_target;
}

glm::vec3 Camera::get_up() const {
    return m_up;
}

float Camera::get_scale() const {
    return m_scale_factor;
}
