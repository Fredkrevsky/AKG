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
    auto& [v1, n1] = p1;
    auto& [v2, n2] = p2;
    auto& [v3, n3] = p3;

    if (v1.y > v2.y) std::swap(p1, p2);
    if (v1.y > v3.y) std::swap(p1, p3);
    if (v2.y > v3.y) std::swap(p2, p3);

    const int x1 = static_cast<int>(v1.x);
    const int y1 = static_cast<int>(v1.y);
    const int x2 = static_cast<int>(v2.x);
    const int y2 = static_cast<int>(v2.y);
    const int x3 = static_cast<int>(v3.x);
    const int y3 = static_cast<int>(v3.y);

    const double z1 = v1.z;
    const double z2 = v2.z;
    const double z3 = v3.z;

    const int total_height = y3 - y1;
    if (total_height == 0) return;

    int min_i = std::max(-y1, 0);
    int max_i = std::min(y3, height) - y1;
    for (int i = min_i; i < max_i; ++i) {
        bool second_half = i > (y2 - y1) || y2 == y1;
        int segment_height = second_half ? (y3 - y2) : (y2 - y1);
        if (segment_height == 0) continue;

        double alpha = static_cast<double>(i) / total_height;
        double beta = static_cast<double>(second_half ? (i - (y2 - y1)) : i) / segment_height;

        int Ax = x1 + (x3 - x1) * alpha;
        int Ay = y1 + i;
        double Az = z1 + (z3 - z1) * alpha;
        double Anx = n1.x + (n3.x - n1.x) * alpha;
        double Any = n1.y + (n3.y - n1.y) * alpha;
        double Anz = n1.z + (n3.z - n1.z) * alpha;

        int Bx, By;
        double Bz, Bnx, Bny, Bnz;
        if (second_half) {
            Bx = x2 + (x3 - x2) * beta;
            By = y2 + (i - (y2 - y1));
            Bz = z2 + (z3 - z2) * beta;
            Bnx = n2.x + (n3.x - n2.x) * beta;
            Bny = n2.y + (n3.y - n2.y) * beta;
            Bnz = n2.z + (n3.z - n2.z) * beta;
        } else {
            Bx = x1 + (x2 - x1) * beta;
            By = y1 + i;
            Bz = z1 + (z2 - z1) * beta;
            Bnx = n1.x + (n2.x - n1.x) * beta;
            Bny = n1.y + (n2.y - n1.y) * beta;
            Bnz = n1.z + (n2.z - n1.z) * beta;
        }

        if (Ax > Bx) {
            std::swap(Ax, Bx);
            std::swap(Az, Bz);
            std::swap(Anx, Bnx);
            std::swap(Any, Bny);
            std::swap(Anz, Bnz);
        }

        int min_x = std::max(Ax, 0);
        int max_x = std::min(Bx, width);
        int index = Ay * width;

        for (int x = min_x; x < max_x; ++x) {
            double t = (x - Ax) / static_cast<double>(Bx - Ax);
            double z = Az + (Bz - Az) * t;
            if (z < m_z_buffer[index + x]) {
                double nx = Anx + (Bnx - Anx) * t;
                double ny = Any + (Bny - Any) * t;
                double nz = Anz + (Bnz - Anz) * t;

                Point point{
                    Vertex{x, Ay, z, 1.0},
                    Normal{nx, ny, nz, 1.0}
                };

                Color::RGBA color = m_raster.get_color(point);
                m_data[index + x] = color;
                m_z_buffer[index + x] = z;
            }
        }
    }
}

void Renderer::draw(const Points& points, const Faces& faces) {
    const Vector4 eye = m_camera->get_eye();
    m_raster.set_eye(eye);
    m_raster.set_sun(eye);

    clear();
    Points transformed_points = transform_points(points);
    std::ranges::for_each(faces, [&](const Face& face) {
        const auto& p1 = transformed_points[face[0]];
        const auto& p2 = transformed_points[face[1]];
        const auto& p3 = transformed_points[face[2]];
        const auto& [v1, n1] = p1;
        const auto& [v2, n2] = p2;
        const auto& [v3, n3] = p3;

        if (v1.w > 0 && v2.w > 0 && v3.w > 0) {
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
        auto& [vertex, normal] = point;
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
