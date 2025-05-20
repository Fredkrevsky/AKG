#pragma once
#include "Matrix.hpp"
#include "Point.hpp"
#include "Camera.hpp"
#include "Raster.hpp"
#include <memory>

namespace Rendered {
    struct PointData{
        glm::vec3 world; 
        glm::vec4 screen;
        glm::vec3 normal;
        glm::vec2 texture;
    };
}

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
    void draw_triangle(const Point& p1, const Point& p2, const Point& p3);

private:
    Raster m_raster;
    std::shared_ptr<Camera> m_camera;
    std::vector<Color::RGBA> m_data; 
    std::vector<float> m_z_buffer;
};