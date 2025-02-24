#include "Camera.hpp"
#include <iostream>

void Camera::move(MoveDirection direction) {
    Point forward = (target - eye).normalize();
    Point right = up.cross(forward).normalize();
    
    switch (direction)
    {
        case MoveDirection::FORWARD:
            eye = eye + forward * camera_speed;
            target = target + forward * camera_speed;
            break;

        case MoveDirection::BACK:
            eye = eye - forward * camera_speed;
            target = target - forward * camera_speed;
            break;

        case MoveDirection::LEFT:
            eye = eye + right * camera_speed;
            target = target + right * camera_speed;
            break;

        case MoveDirection::RIGHT:
            eye = eye - right * camera_speed;
            target = target - right * camera_speed;
            break;
    }
}

void Camera::rotate(double deltaX, double deltaY) {

    constexpr auto radians = [](double angle) {
        return PI * angle / 180.0;
    };

    constexpr double max_pitch = radians(89.0);
    constexpr static Point world_up{0.0, 1.0, 0.0, 0.0};

    static double yaw = radians(90.0);
    static double pitch = 0.0;

    yaw -= deltaX * rotation_sensitivity;
    pitch += deltaY * rotation_sensitivity;

    yaw = normalize_angle(yaw);
    pitch = std::clamp(pitch, -max_pitch, max_pitch);

    Point forward {
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

TransformMatrix Camera::get_transform_matrix()
{
    Point ZAxis = (eye - target).normalize();
    Point XAxis = up.cross(ZAxis).normalize();
    Point YAxis = ZAxis.cross(XAxis).normalize();

    TransformMatrix view_matrix = {{
        {XAxis.x, XAxis.y, XAxis.z, -XAxis.dot(eye)},
        {YAxis.x, YAxis.y, YAxis.z, -YAxis.dot(eye)},
        {ZAxis.x, ZAxis.y, ZAxis.z, -ZAxis.dot(eye)},
        {0, 0, 0, 1}
    }};

    double f = 1.0 / std::tan(fov * 0.5);
    TransformMatrix projection_matrix = {{
        {f / aspect, 0,  0,  0},
        {0, f,  0,  0},
        {0, 0, (zfar + znear) / (znear - zfar), 2 * zfar * znear / (znear - zfar)},
        {0, 0, -1,  0}
    }};

    TransformMatrix viewport_matrix = {{
        {width / 2, 0, 0, width / 2},
        {0, -height / 2, 0, height / 2},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};

    TransformMatrix scale_matrix = {{
        {scale_factor, 0, 0, 0},
        {0, scale_factor, 0, 0},
        {0, 0, scale_factor, 0},
        {0, 0, 0, 1}
    }}; 

    return viewport_matrix * projection_matrix * view_matrix * scale_matrix;
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

std::vector<Point> Camera::transform_vertices(const std::vector<Point>& vertices) {
    std::vector<Point> transformed_vertices;

    TransformMatrix transform_matrix = get_transform_matrix();

    std::ranges::transform(vertices, std::back_inserter(transformed_vertices), 
        [&](const Point& vertex)
    {
        auto transformed_vertex = vertex * transform_matrix;
        double w = transformed_vertex.w;
        transformed_vertex = transformed_vertex * (1 / w);
        return transformed_vertex;
    });

    return transformed_vertices;
}
