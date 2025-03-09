#include "Camera.hpp"
#include <algorithm>

void Camera::move(MoveDirection direction) {
    Point forward = (target - eye).normalize();
    Point left = up.cross(forward).normalize();
    
    Point forward_move_vector = forward * camera_speed;
    Point left_move_vector = left * camera_speed;

    switch (direction)
    {
        case MoveDirection::FORWARD:
            eye += forward_move_vector;
            target += forward_move_vector;
            break;

        case MoveDirection::BACK:
            eye -= forward_move_vector;
            target -= forward_move_vector;
            break;

        case MoveDirection::LEFT:      
            eye += left_move_vector;
            target += left_move_vector;
            break;

        case MoveDirection::RIGHT:  
            eye -= left_move_vector;
            target -= left_move_vector;
            break;
    }
}

void Camera::rotate(double delta_x, double delta_y) {
    constexpr auto radians = [](double angle) {
        return PI * angle / 180.0;
    };

    constexpr double max_pitch = radians(89.0);
    constexpr static Point world_up{0.0, 1.0, 0.0, 0.0};

    Point forward = (target - eye).normalize();
    double yaw = std::atan2(forward.z, forward.x);
    double pitch = std::asin(forward.y);

    yaw -= delta_x * rotation_sensitivity;
    pitch += delta_y * rotation_sensitivity;
    pitch = std::clamp(pitch, -max_pitch, max_pitch);

    forward = {
        std::cos(yaw) * std::cos(pitch),
        std::sin(pitch),
        std::sin(yaw) * std::cos(pitch),
        1.0
    };
    forward.normalize();

    Point right = world_up.cross(forward).normalize();
    up = forward.cross(right).normalize();
    target = eye + forward;
}

void Camera::scale(bool is_getting_closer) {
    double new_scale_factor = is_getting_closer 
        ? scale_factor * scale_sensitivity
        : scale_factor / scale_sensitivity;

    scale_factor = std::clamp(
        new_scale_factor,
        MIN_SCALE_FACTOR,
        MAX_SCALE_FACTOR
    );
}

TransformMatrix Camera::get_view_matrix() const {
    Point ZAxis = (eye - target).normalize();
    Point XAxis = up.cross(ZAxis).normalize();
    Point YAxis = ZAxis.cross(XAxis).normalize();

    return {{
        {XAxis.x, XAxis.y, XAxis.z, -XAxis.dot(eye)},
        {YAxis.x, YAxis.y, YAxis.z, -YAxis.dot(eye)},
        {ZAxis.x, ZAxis.y, ZAxis.z, -ZAxis.dot(eye)},
        {0, 0, 0, 1}
    }};
}

TransformMatrix Camera::get_projection_matrix() const {
    constexpr double f = 1.0 / std::tan(fov * 0.5);
    return {{
        {f / aspect, 0,  0,  0},
        {0, f,  0,  0},
        {0, 0, (zfar + znear) / (znear - zfar), 2 * zfar * znear / (znear - zfar)},
        {0, 0, -1,  0}
    }};
}

TransformMatrix Camera::get_viewport_matrix() const {
    return {{
        {width / 2, 0, 0, width / 2},
        {0, -height / 2, 0, height / 2},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
}

TransformMatrix Camera::get_scale_matrix() const {
    return {{
        {scale_factor, 0, 0, 0},
        {0, scale_factor, 0, 0},
        {0, 0, scale_factor, 0},
        {0, 0, 0, 1}
    }}; 
}

Point Camera::get_eye() const {
    return eye;
}

Point Camera::get_target() const {
    return target;
}

Point Camera::get_up() const {
    return up;
}

double Camera::get_scale() const {
    return scale_factor;
}
