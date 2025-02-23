#include "Camera.hpp"

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
    Point forward = (target - eye).normalize();
    Point right = up.cross(forward).normalize();

    auto rotationY = createRotationY(deltaX * rotation_sensitivity);
    forward = forward * rotationY;
    up = up * rotationY;

    auto rotationX = createRotationX(deltaY * rotation_sensitivity);
    forward = forward * rotationX;
    up = up * rotationX;

    target = eye + forward;
}

void Camera::scale(bool is_getting_closer) {
    if (is_getting_closer){
        scale_factor *= scale_sensitivity;
    }
    else {
        scale_factor /= scale_sensitivity;
    }
}

TransformMatrix Camera::get_transform_matrix()
{
    constexpr double znear = 0.01; 
    constexpr double zfar = 100.0; 
    constexpr double aspect = width / height; 

    Point ZAxis = (eye - target).normalize();
    Point XAxis = up.cross(ZAxis).normalize();
    Point YAxis = ZAxis.cross(XAxis).normalize();

    TransformMatrix view_matrix = {{
        {XAxis.x, XAxis.y, XAxis.z, -XAxis.dot(eye)},
        {YAxis.x, YAxis.y, YAxis.z, -YAxis.dot(eye)},
        {ZAxis.x, ZAxis.y, ZAxis.z, -ZAxis.dot(eye)},
        {0, 0, 0, 1}
    }};

    TransformMatrix projection_matrix = {{
        {1.0 / (aspect * std::tan(fov / 2.0)), 0, 0, 0},
        {0, 1.0 / std::tan(fov / 2.0), 0, 0},
        {0, 0, (zfar + znear) / (znear - zfar), 2 * zfar * znear / (znear - zfar)},
        {0, 0, -1, 0}
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

std::vector<Point> Camera::get_vertices(const std::vector<Point>& vertices) {
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
