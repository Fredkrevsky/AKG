#include "Renderer.hpp"
#include <algorithm>


Renderer::Renderer() noexcept
    : m_data(width * height)
    , m_z_buffer(width * height)
{}

void Renderer::clear(){
    std::ranges::fill(m_data, 0);
    std::ranges::fill(m_z_buffer, 1.0);
}

const uint8_t* Renderer::data() const {
    return reinterpret_cast<const uint8_t*>(m_data.data());
}

void Renderer::set_camera(std::shared_ptr<Camera> camera){
    m_camera = camera;
}

void Renderer::draw_triangle(Point p1, Point p2, Point p3) {

    const auto& [v1, n1, c1] = p1;
    const auto& [v2, n2, c2] = p2;
    const auto& [v3, n3, c3] = p3;

    int min_x = std::max(0, static_cast<int>(std::min({v1.x, v2.x, v3.x})));
    int max_x = std::min(width - 1, static_cast<int>(std::max({v1.x, v2.x, v3.x})));
    int min_y = std::max(0, static_cast<int>(std::min({v1.y, v2.y, v3.y})));
    int max_y = std::min(height - 1, static_cast<int>(std::max({v1.y, v2.y, v3.y})));

    double denominator = (v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y);
    if (std::abs(denominator) < 1e-6) return;

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            double u = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / denominator;
            double v = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / denominator;
            double w = 1.0 - u - v;

            if (u < 0 || v < 0 || w < 0) continue;

            double z = u * v1.z + v * v2.z + w * v3.z;
            int index = y * width + x;

            if (z < m_z_buffer[index]){
                uint32_t grey = static_cast<double>(c1 & 255) * u + 
                                static_cast<double>(c2 & 255) * v + 
                                static_cast<double>(c3 & 255) * w;
                uint32_t color = (255 << 24) | (grey << 16) | (grey << 8) | grey;
                m_z_buffer[index] = z;
                m_data[index] = color;
            }
        }
    }
}

void Renderer::draw(const Points& points, const Faces& faces) {
    clear();
    Points transformed_points = transform_points(points);

    std::ranges::for_each(faces, [&](const Face& face) {
        const auto& p1 = transformed_points[face[0]];
        const auto& p2 = transformed_points[face[1]];
        const auto& p3 = transformed_points[face[2]];
        const auto& [v1, n1, c1] = p1;
        const auto& [v2, n2, c2] = p2;
        const auto& [v3, n3, c3] = p3;

        if (v1.w > 0 && v2.w > 0 && v3.w > 0 &&
            c1 != Color::Basic::Transparent && 
            c2 != Color::Basic::Transparent && 
            c3 != Color::Basic::Transparent) {
            draw_triangle(p1, p2, p3);
        }
    });
}

TransformMatrix Renderer::get_view_matrix() const {
    const Vector4 eye = m_camera->get_eye();
    const Vector4 target = m_camera->get_target();
    const Vector4 up = m_camera->get_up();

    Vector4 ZAxis = (eye - target).normalize();
    Vector4 XAxis = up.cross(ZAxis).normalize();
    Vector4 YAxis = ZAxis.cross(XAxis).normalize();

    return {{
        {XAxis.x, XAxis.y, XAxis.z, -XAxis.dot(eye)},
        {YAxis.x, YAxis.y, YAxis.z, -YAxis.dot(eye)},
        {ZAxis.x, ZAxis.y, ZAxis.z, -ZAxis.dot(eye)},
        {0, 0, 0, 1}
    }};
}

TransformMatrix Renderer::get_projection_matrix() const {
    constexpr double f = 1.0 / std::tan(fov * 0.5);
    return {{
        {f / aspect, 0,  0,  0},
        {0, f,  0,  0},
        {0, 0, (zfar + znear) / (znear - zfar), 2 * zfar * znear / (znear - zfar)},
        {0, 0, -1,  0}
    }};
}

TransformMatrix Renderer::get_viewport_matrix() const {
    return {{
        {width / 2, 0, 0, width / 2},
        {0, -height / 2, 0, height / 2},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
}

TransformMatrix Renderer::get_scale_matrix() const {
    const double scale_factor = m_camera->get_scale();
    return {{
        {scale_factor, 0, 0, 0},
        {0, scale_factor, 0, 0},
        {0, 0, scale_factor, 0},
        {0, 0, 0, 1}
    }}; 
}

Points Renderer::transform_points(const Points& points) const {
    const auto view_matrix = get_view_matrix();
    const auto viewport_matrix = get_viewport_matrix();
    const auto scale_matrix = get_scale_matrix();
    const auto projection_matrix = get_projection_matrix();
    auto cached_matrix = projection_matrix * view_matrix * scale_matrix;

    Points transformed_points = points;
    std::ranges::for_each(transformed_points, [&](Point& point) {
        auto& [vertex, normal, color] = point;
        vertex *= cached_matrix;
        if (vertex.z < -vertex.w || vertex.z > vertex.w) {
            vertex.w = 0;
            return;
        }
        if (vertex.w != 0) {
            vertex *= 1 / vertex.w;
        }
        vertex *= viewport_matrix;
    });
    return transformed_points;
}
