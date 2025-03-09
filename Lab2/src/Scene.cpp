#include "Scene.hpp"
#include <memory>
#include <algorithm>
#include <ranges>

using namespace std::string_literals;

bool Scene::initialize() {
    std::string file_path{MODEL_FILE_PATH};

    auto get_file_format = [](const std::string& file_path) -> std::string {
        std::size_t dot_index = file_path.find_last_of(".");
        if (dot_index == std::string::npos) {
            return ""s;
        }
        return file_path.substr(dot_index + 1);
    };

    std::string file_format = get_file_format(file_path);
    
    auto parser = Parser::create_parser(file_format);
    if (!parser){
        return false;
    }

    parser->parse_file(file_path);
    auto faces = parser->get_faces();
    auto vertices = parser->get_vertices();
    m_model.set_data(std::move(vertices), std::move(faces));
    return true;
}

void Scene::rotate_model(const Point &rotate_vector) {
    m_model.rotate(rotate_vector);
}

void Scene::move_model(const Point &move_vector) {
    m_model_position += move_vector;
}

Vertices Scene::get_vertices() const {
    auto vertices = m_model.get_vertices();
    auto move_matrix = create_move_matrix(m_model_position);

    std::ranges::for_each(vertices, [&](auto& vertex){
        vertex *= move_matrix;
    });
    return vertices;
}

Faces Scene::get_faces() const {
    Faces faces = m_model.get_faces();
    Normals normals = m_model.get_normals();

    for (auto&& [face, normal] : std::views::zip(faces, normals)) {
        double intensity = std::max(0.0, normal.x * m_sun.x + normal.y * m_sun.y + normal.z * m_sun.z);
        uint8_t red = static_cast<uint8_t>(0xFF * intensity);
        uint8_t green = static_cast<uint8_t>(0xFF * intensity);
        uint8_t blue = static_cast<uint8_t>(0xFF * intensity);
        uint32_t color = (0xFF << 24) | (blue << 16) | (green << 8) | red;
        face[3] = color;
    }

    return faces;
}
