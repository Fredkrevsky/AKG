#include "Scene.hpp"

using namespace std::string_literals;

bool Scene::initialize() {
    std::unique_ptr<Parser> parser{nullptr};
    std::string file_path{MODEL_FILE_PATH};

    auto get_file_format = [](const std::string& file_path) -> std::string {
        std::size_t dot_index = file_path.find_last_of(".");
        if (dot_index == std::string::npos) {
            return ""s;
        }
        return file_path.substr(dot_index);
    };

    std::string file_format = get_file_format(file_path);

    if (file_format == ".obj"){
        parser.reset(new ParserOBJ());
    }
    else {
        parser.reset(nullptr);
        return false;
    }

    if (parser) {
        parser->parse_file(file_path);
        m_model.set_vertices(parser->get_vertices());
        m_model.set_faces(parser->get_faces());
    }

    return true;
}

void Scene::rotate_model(const Point &rotate_vector) {
    m_model.rotate(rotate_vector);
}

void Scene::move_model(const Point &move_vector) {
    m_model_position += move_vector;
}

Vertices Scene::get_vertices() const
{
    auto vertices = m_model.get_vertices();
    auto move_matrix = create_move_matrix(m_model_position);

    std::ranges::for_each(vertices, [&](auto& vertex){
        vertex *= move_matrix;
    });
    return vertices;
}

Faces Scene::get_faces() const
{
    return m_model.get_faces();
}
