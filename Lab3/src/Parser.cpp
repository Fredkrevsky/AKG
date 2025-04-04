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

Normals Parser::get_normals() const {
    return m_normals;
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
            Vector4 vertex{0, 0, 0, 1};
            iss >> vertex.x >> vertex.y >> vertex.z;
            m_vertices.push_back(vertex);
        } 
        else if (type == "f") {
            Face face;
            for (int i = 0; i < 3; ++i) {
                std::string vertex_data;
                iss >> vertex_data;
                std::istringstream vertex_stream(vertex_data);
                std::string vertex_index_str;
                std::getline(vertex_stream, vertex_index_str, '/');
                face[i] = std::stoi(vertex_index_str) - 1;
            }
            m_faces.push_back(face);
        }
        else if (type == "vn") {
            Vector4 normal{0, 0, 0, 1};
            iss >> normal.x >> normal.y >> normal.z;
            m_normals.push_back(normal);
        }
    }
}
