#pragma once
#include "Matrix.hpp"
#include "Camera.hpp"
#include "Raster.hpp"
#include <memory>

class Renderer final {
public:
    struct PointData {
        const Vertex& world; 
        const ScreenVertex& screen;
        const Vertex& normal;
        const TextureVertex& texture;
    };

    Renderer() noexcept;
    ~Renderer() = default;

    void set_camera(std::shared_ptr<Camera> camera);
    const uint8_t* data() const;
    void clear_bitmap();
    void draw(const Vertices& vertices, 
              const Faces& faces,
              const Vertices& normals, 
              const TextureVertices& texture_vertices);

private:
    glm::mat4x4 get_view_matrix() const;
    glm::mat4x4 get_viewport_matrix() const;
    glm::mat4x4 get_scale_matrix() const;
    glm::mat4x4 get_projection_matrix() const; 

    ScreenVertices get_screen_vertices(const Vertices& vertices) const;
    void draw_triangle(const PointData& p1, const PointData& p2, const PointData& p3);

private:
    Raster m_raster;
    std::shared_ptr<Camera> m_camera;
    std::vector<Color::RGBA> m_data; 
    std::vector<float> m_z_buffer;
};