#include "Renderer.hpp"
#include <algorithm>


Renderer::Renderer() noexcept
    : m_data(width * height)
    , m_z_buffer(width * height)
{}

void Renderer::clear_bitmap(){
    std::ranges::fill(m_data, 0);
    std::ranges::fill(m_z_buffer, 1.0);
}

const uint8_t* Renderer::data() const {
    return reinterpret_cast<const uint8_t*>(m_data.data());
}

void Renderer::set_camera(std::shared_ptr<Camera> camera){
    m_camera = camera;
}

void Renderer::draw_triangle(const Point* p1, const Point* p2, const Point* p3) {
    const Point* points[3] = {p1, p2, p3};
    
    std::sort(points, points + 3, [](const Point* a, const Point* b) {
        return a->screen.y < b->screen.y;
    });

    auto& [w1, s1, n1] = *points[0];
    auto& [w2, s2, n2] = *points[1];
    auto& [w3, s3, n3] = *points[2];

    const float z1 = s1.z;
    const float z2 = s2.z;
    const float z3 = s3.z;

    const int x1 = static_cast<int>(s1.x);
    const int y1 = static_cast<int>(s1.y);
    const int x2 = static_cast<int>(s2.x);
    const int y2 = static_cast<int>(s2.y);
    const int x3 = static_cast<int>(s3.x);
    const int y3 = static_cast<int>(s3.y);

    const int total_height = y3 - y1;
    if (total_height == 0) return;

    int min_i = std::max(-y1, 0);
    int max_i = std::min(y3, height) - y1;
    for (int i = min_i; i < max_i; ++i) {
        bool second_half = i > (y2 - y1) || y2 == y1;
        int segment_height = second_half ? (y3 - y2) : (y2 - y1);
        if (segment_height == 0) continue;

        float alpha = static_cast<float>(i) / total_height;
        float beta = static_cast<float>(second_half ? (i - (y2 - y1)) : i) / segment_height;

        int Ax = x1 + (x3 - x1) * alpha;
        int Ay = y1 + i;
        float Az = z1 + (z3 - z1) * alpha;    
        
        Vertex A = w1 + (w3 - w1) * alpha;
        Normal An = n1 + (n3 - n1) * alpha;

        int Bx, By;
        float Bz; 
        Vertex B;
        Normal Bn;

        if (second_half) {
            Bx = x2 + (x3 - x2) * beta;
            By = y2 + (i - (y2 - y1));
            Bz = z2 + (z3 - z2) * beta;
            B = w2 + (w3 - w2) * beta;
            Bn = n2 + (n3 - n2) * beta;
        } else {
            Bx = x1 + (x2 - x1) * beta;
            By = y1 + i;
            Bz = z1 + (z2 - z1) * beta;
            B = w1 + (w2 - w1) * beta;
            Bn = n1 + (n2 - n1) * beta;
        }

        if (Ax > Bx) {
            std::swap(Ax, Bx);
            std::swap(Az, Bz);
            std::swap(A, B);
            std::swap(An, Bn);
        }

        int min_x = std::max(Ax, 0);
        int max_x = std::min(Bx, width);
        int index = Ay * width;

        for (int x = min_x; x < max_x; ++x) {
            float t = (x - Ax) / static_cast<float>(Bx - Ax);
            float z = Az + (Bz - Az) * t;
            if (z < m_z_buffer[index + x]) {

                Point point{ 
                    Vertex{A + (B - A) * t},
                    Vertex{static_cast<float>(x), static_cast<float>(Ay), z, 1.0},
                    Normal{An + (Bn - An) * t}
                };

                Color::RGBA color = m_raster.get_color(point);
                m_data[index + x] = color;
                m_z_buffer[index + x] = z;
            }
        }
    }
}

void Renderer::draw(Points&& points, const Faces& faces) {

    static Vector4 sun{5.0, 5.0, 5.0, 1.0};

    const Vector4 eye = m_camera->get_eye();
    m_raster.set_eye(eye);
    m_raster.set_sun(sun);

    clear_bitmap();
    project_points(points);

    std::ranges::for_each(faces, [&](const Face& face) {
        const auto& p1 = points[face[0]];
        const auto& p2 = points[face[1]];
        const auto& p3 = points[face[2]];
        const auto& [w1, s1, n1] = p1;
        const auto& [w2, s2, n2] = p2;
        const auto& [w3, s3, n3] = p3;

        if (s1.w > 0 && s2.w > 0 && s3.w > 0) {
            draw_triangle(&p1, &p2, &p3);
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
    constexpr float f = 1.0 / std::tan(fov * 0.5);
    return {{
        {f / aspect, 0,  0,  0},
        {0, f,  0,  0},
        {0, 0, zfar / (znear - zfar), zfar * znear / (znear - zfar)},
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
    const float scale_factor = m_camera->get_scale();
    return {{
        {scale_factor, 0, 0, 0},
        {0, scale_factor, 0, 0},
        {0, 0, scale_factor, 0},
        {0, 0, 0, 1}
    }}; 
}

void Renderer::project_points(Points& points) const {
    const auto view_matrix = get_view_matrix();
    const auto viewport_matrix = get_viewport_matrix();
    const auto scale_matrix = get_scale_matrix();
    const auto projection_matrix = get_projection_matrix();
    auto cached_matrix = viewport_matrix * projection_matrix * view_matrix * scale_matrix;

    std::ranges::for_each(points, [&](Point& point) {
        auto& [world, screen, normal] = point;
        screen = cached_matrix * world;
        if (screen.w != 0) {
            screen *= 1 / screen.w;
        }

        if (screen.z < -1 || screen.z > 1) {
            screen.w = 0;
        }
    });
}
