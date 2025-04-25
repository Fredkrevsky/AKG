#pragma once
#include "Matrix.hpp"
#include "Point.hpp"
#include "Camera.hpp"
#include "Raster.hpp"
#include <memory>

class Renderer final {
public:
    Renderer() noexcept;
    ~Renderer() = default;

    void set_camera(std::shared_ptr<Camera> camera);
    const uint8_t* data() const;
    void clear_bitmap();
    void draw(Points&& points,
              const Faces& faces,
              const Vertices& normals, 
              const Vertices& texture_vertices);

private:
    glm::mat4x4 get_view_matrix() const;
    glm::mat4x4 get_viewport_matrix() const;
    glm::mat4x4 get_scale_matrix() const;
    glm::mat4x4 get_projection_matrix() const; 

    void project_points(Points& points) const;
    void draw_triangle(const Point* p1, const Point* p2, const Point* p3);

private:
    constexpr static int width{1600};
    constexpr static int height{900};
    constexpr static float fov = PI / 4.0;
    constexpr static float znear = 0.4; 
    constexpr static float zfar = 1000.0; 
    constexpr static float aspect = static_cast<float>(width) / height; 

    Raster m_raster;
    std::shared_ptr<Camera> m_camera;
    std::vector<Color::RGBA> m_data; 
    std::vector<float> m_z_buffer;
};