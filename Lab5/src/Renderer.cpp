#include "Renderer.hpp"
#include <algorithm>
#include <iostream>

namespace {

constexpr int WIDTH{1600};
constexpr int HEIGHT{900};
constexpr float FOV{PI / 4.0f};
constexpr float ZNEAR{0.4f}; 
constexpr float ZFAR{1000.f}; 
constexpr float ASPECT = static_cast<float>(WIDTH) / HEIGHT; 

template<typename T>
T lerp(const T& a, const T& b, float t){
    return (1 - t) * a + t * b;
}

}

Renderer::Renderer() noexcept
    : m_data(WIDTH * HEIGHT)
    , m_z_buffer(WIDTH * HEIGHT)
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

void Renderer::draw_triangle(const Point& p1, const Point& p2, const Point& p3) {
    const Point* points[3] = {&p1, &p2, &p3};
    
    std::sort(points, points + 3, [](const Point* a, const Point* b) {
        return a->screen.y < b->screen.y;
    });

    auto& [w1, s1, n1, t1] = *points[0]; 
    auto& [w2, s2, n2, t2] = *points[1]; 
    auto& [w3, s3, n3, t3] = *points[2];  

    const int x1 = static_cast<int>(std::round(s1.x));
    const int y1 = static_cast<int>(std::round(s1.y));
    const int x2 = static_cast<int>(std::round(s2.x));
    const int y2 = static_cast<int>(std::round(s2.y));
    const int x3 = static_cast<int>(std::round(s3.x));
    const int y3 = static_cast<int>(std::round(s3.y));
    const float z1 = s1.z;
    const float z2 = s2.z;
    const float z3 = s3.z;

    const float inv_w1 = 1.0f / s1.w;
    const float inv_w2 = 1.0f / s2.w;
    const float inv_w3 = 1.0f / s3.w;

    const glm::vec2 t1_persp = t1 * inv_w1;
    const glm::vec2 t2_persp = t2 * inv_w2;
    const glm::vec2 t3_persp = t3 * inv_w3;

    const glm::vec3 n1_persp = n1 * inv_w1;
    const glm::vec3 n2_persp = n2 * inv_w2;
    const glm::vec3 n3_persp = n3 * inv_w3;

    const glm::vec3 w1_persp = w1 * inv_w1;
    const glm::vec3 w2_persp = w2 * inv_w2;
    const glm::vec3 w3_persp = w3 * inv_w3;

    const int total_height = y3 - y1;
    if (total_height == 0) return;

    int min_i = std::max(-y1, 0);
    int max_i = std::min(y3, HEIGHT) - y1;
    
    for (int i = min_i; i < max_i; ++i) {
        bool second_half = i > (y2 - y1) || y2 == y1;
        int segment_height = second_half ? (y3 - y2) : (y2 - y1);
        if (segment_height == 0) continue;

        const float alpha = static_cast<float>(i) / total_height;
        const float beta = static_cast<float>(second_half ? (i - (y2 - y1)) : i) / segment_height;

        float inv_wA = lerp(inv_w1, inv_w3, alpha);
        float inv_wB = second_half 
            ? lerp(inv_w2, inv_w3, beta)
            : lerp(inv_w1, inv_w2, beta);

        float zA = lerp(z1, z3, alpha);
        float zB = second_half 
            ? lerp(z2, z3, beta)
            : lerp(z1, z2, beta);

        glm::vec2 At_persp = lerp(t1_persp, t3_persp, alpha);
        glm::vec2 Bt_persp = second_half
            ? lerp(t2_persp, t3_persp, beta)
            : lerp(t1_persp, t2_persp, beta);

        glm::vec3 An_persp = lerp(n1_persp, n3_persp, alpha);
        glm::vec3 Bn_persp = second_half
            ? lerp(n2_persp, n3_persp, beta)
            : lerp(n1_persp, n2_persp, beta);

        glm::vec3 Aw_persp = lerp(w1_persp, w3_persp, alpha);
        glm::vec3 Bw_persp = second_half
            ? lerp(w2_persp, w3_persp, beta)
            : lerp(w1_persp, w2_persp, beta);

        int Ax = static_cast<int>(lerp(x1, x3, alpha));
        int Ay = y1 + i;
        int Bx = static_cast<int>(second_half 
            ? lerp(x2, x3, beta)
            : lerp(x1, x2, beta));
        int By = second_half ? y2 + (i - (y2 - y1)) : y1 + i;

        if (Ax > Bx) {
            std::swap(Ax, Bx);
            std::swap(zA, zB);
            std::swap(inv_wA, inv_wB);
            std::swap(At_persp, Bt_persp);
            std::swap(An_persp, Bn_persp);
            std::swap(Aw_persp, Bw_persp);
        }

        const int min_x = std::max(Ax, 0);
        const int max_x = std::min(Bx, WIDTH);
        const int index = Ay * WIDTH;

        for (int x = min_x; x < max_x; ++x) {
            const float t = (x - Ax) / static_cast<float>(Bx - Ax);
            const float inv_w = lerp(inv_wA, inv_wB, t);
            const float z = lerp(zA, zB, t);
            
            if (z < m_z_buffer[index + x]) {
                const glm::vec2 tex_persp = lerp(At_persp, Bt_persp, t);
                const glm::vec2 tex_coord = tex_persp / inv_w;

                const glm::vec3 normal_persp = lerp(An_persp, Bn_persp, t);
                const glm::vec3 normal = glm::normalize(normal_persp / inv_w);

                const glm::vec3 world_persp = lerp(Aw_persp, Bw_persp, t);
                const glm::vec3 world = world_persp / inv_w;

                Point point{ 
                    world, 
                    glm::vec4{static_cast<float>(x), static_cast<float>(Ay), z, 1.0f},
                    normal, 
                    tex_coord 
                };

                Color::RGBA color = m_raster.get_color(point);
                m_data[index + x] = color;
                m_z_buffer[index + x] = z;
            }
        }
    }
}

void Renderer::draw(Points&& points, const Faces& faces, const Vertices& normals, const Vertices& texture_vertices) {
    const glm::vec3 eye = m_camera->get_eye();
    m_raster.set_eye(eye);
    m_raster.set_sun(eye);

    clear_bitmap();
    project_points(points);

    auto check = [](const Point& vertex) -> bool {
        return vertex.screen.z >= -1 && vertex.screen.z <= 1;
    };

    std::ranges::for_each(faces, [&](const Face& face) {
        const auto& p1 = points[face[0][0]];
        const auto& p2 = points[face[1][0]];
        const auto& p3 = points[face[2][0]];
        const auto& t1 = texture_vertices[face[0][1]];
        const auto& t2 = texture_vertices[face[1][1]];
        const auto& t3 = texture_vertices[face[2][1]];
        const auto& n1 = normals[face[0][2]];
        const auto& n2 = normals[face[1][2]];
        const auto& n3 = normals[face[2][2]];

        if (check(p1) && check(p2) && check(p3)) {
            Point p41{p1.world, p1.screen, n1, t1};
            Point p42{p2.world, p2.screen, n2, t2};
            Point p43{p3.world, p3.screen, n3, t3};
        
            draw_triangle(p41, p42, p43);
        }
    });
}

glm::mat4x4 Renderer::get_view_matrix() const {
    const glm::vec3 eye = m_camera->get_eye();
    const glm::vec3 target = m_camera->get_target();
    const glm::vec3 up = m_camera->get_up();

    glm::vec3 ZAxis = glm::normalize(eye - target);
    glm::vec3 XAxis = glm::normalize(glm::cross(up, ZAxis));
    glm::vec3 YAxis = glm::normalize(glm::cross(ZAxis, XAxis));

    return glm::mat4x4{
        {XAxis.x, XAxis.y, XAxis.z, glm::dot(-XAxis, eye)},
        {YAxis.x, YAxis.y, YAxis.z, glm::dot(-YAxis, eye)},
        {ZAxis.x, ZAxis.y, ZAxis.z, glm::dot(-ZAxis, eye)},
        {0, 0, 0, 1}
    };
}

glm::mat4x4 Renderer::get_projection_matrix() const {
    constexpr float f = 1.0 / std::tan(FOV * 0.5);
    return glm::mat4x4{
        {f / ASPECT, 0,  0,  0},
        {0, f,  0,  0},
        {0, 0, ZFAR / (ZNEAR - ZFAR), ZFAR * ZNEAR / (ZNEAR - ZFAR)},
        {0, 0, -1,  0}
    };
}

glm::mat4x4 Renderer::get_viewport_matrix() const {
    return glm::mat4x4{
        {WIDTH / 2, 0, 0, WIDTH / 2},
        {0, -HEIGHT / 2, 0, HEIGHT / 2},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
}

glm::mat4x4 Renderer::get_scale_matrix() const {
    const float scale_factor = m_camera->get_scale();
    return glm::mat4x4{
        {scale_factor, 0, 0, 0},
        {0, scale_factor, 0, 0},
        {0, 0, scale_factor, 0},
        {0, 0, 0, 1}
    }; 
}

void Renderer::project_points(Points& points) const {
    const auto view_matrix = get_view_matrix();
    const auto viewport_matrix = get_viewport_matrix();
    const auto scale_matrix = get_scale_matrix();
    const auto projection_matrix = get_projection_matrix();
    const auto cached_matrix = glm::transpose(view_matrix * projection_matrix * viewport_matrix);

    std::ranges::for_each(points, [&](Point& point) {
        auto& [world, screen, normal, texture] = point;
        glm::vec4 world4 = {world, 1.0};
        screen = cached_matrix * world4;

        if (screen.w != 0) {
            screen.x /= screen.w;
            screen.y /= screen.w;
            screen.z /= screen.w;
        }
    });
}