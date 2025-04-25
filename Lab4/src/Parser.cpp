#include <Parser.hpp>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

using namespace std::string_literals;

std::unique_ptr<Parser> Parser::create_parser(const std::string& format){
    std::unique_ptr<Parser> parser{};
    if (format == "obj"){
        parser.reset(new ParserOBJ());
    }
    return parser;
}

Vertices Parser::get_vertices() const {
    return m_vertices;
}

Faces Parser::get_faces() const {
    return m_faces;
}

Vertices Parser::get_normals() const {
    return m_normals;
}

Vertices Parser::get_texture_vertices() const {
    return m_texture_vertices;
}

std::string Parser::get_format(const std::string& path) {
    std::size_t dot_index = path.find_last_of(".");
    if (dot_index == std::string::npos) {
        return ""s;
    }
    return path.substr(dot_index + 1);
}


void ParserOBJ::parse_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) { return; }

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec4 vertex{0, 0, 0, 1};
            iss >> vertex.x >> vertex.y >> vertex.z;
            m_vertices.push_back(vertex);
        } 
        else if (type == "f") {
            Face face;
            for (int i = 0; i < 3; ++i) {
                std::string vertex_data;
                iss >> vertex_data;
                size_t pos1 = vertex_data.find('/');
                size_t pos2 = vertex_data.find('/', pos1 + 1);

                int a = std::stoi(vertex_data.substr(0, pos1));
                int b = std::stoi(vertex_data.substr(pos1 + 1, pos2 - pos1 - 1));
                int c = std::stoi(vertex_data.substr(pos2 + 1));

                face[i][0] = a - 1;
                face[i][1] = b - 1;
                face[i][2] = c - 1;
            }
            m_faces.push_back(face);
        }
        else if (type == "vn") {
            glm::vec3 normal{0, 0, 0};
            iss >> normal.x >> normal.y >> normal.z;
            m_normals.push_back(normal);
        }
        else if (type == "vt") {
            glm::vec3 texture_vertex{0, 0, 0};
            iss >> texture_vertex.x >> texture_vertex.y;
            m_texture_vertices.push_back(texture_vertex);
        }
    }
}
