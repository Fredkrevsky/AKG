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
    void clear();
    void draw(Points&& points,
              const Faces& faces);

private:
    TransformMatrix get_view_matrix() const;
    TransformMatrix get_viewport_matrix() const;
    TransformMatrix get_scale_matrix() const;
    TransformMatrix get_projection_matrix() const; 

    void project_points(Points& points) const;
    void draw_triangle(Point p1, Point p2, Point p3);

private:
    constexpr static int width{1600};
    constexpr static int height{900};
    constexpr static double fov = PI / 4.0;
    constexpr static double znear = 0.4; 
    constexpr static double zfar = 1000.0; 
    constexpr static double aspect = static_cast<double>(width) / height; 

    Raster m_raster;
    std::shared_ptr<Camera> m_camera;
    std::vector<uint32_t> m_data; 
    std::vector<double> m_z_buffer;
};