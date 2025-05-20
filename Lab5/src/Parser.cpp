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

TextureVertices Parser::get_texture_vertices() const {
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
            std::vector<std::array<uint32_t, 3>> face_vertices;
            std::string vertex_data;
            
            while (iss >> vertex_data) {
                std::vector<std::string> parts;
                size_t start = 0;
                size_t end = vertex_data.find('/');
                
                while (true) {
                    parts.push_back(vertex_data.substr(start, end - start));
                    if (end == std::string::npos) break;
                    start = end + 1;
                    end = vertex_data.find('/', start);
                }

                uint32_t v = 0, vt = 0, vn = 0;

                if (!parts[0].empty())
                    v = std::stoi(parts[0]);
                if (parts.size() > 1 && !parts[1].empty())
                    vt = std::stoi(parts[1]);
                if (parts.size() > 2 && !parts[2].empty())
                    vn = std::stoi(parts[2]);

                v = (v != 0) ? v - 1 : -1;
                vt = (vt != 0) ? vt - 1 : -1;
                vn = (vn != 0) ? vn - 1 : -1;

                face_vertices.push_back({v, vt, vn});
            }

            size_t num_verts = face_vertices.size();
            if (num_verts < 3) continue;

            for (size_t i = 1; i < num_verts - 1; ++i) {
                Face face;
                face[0] = face_vertices[0];
                face[1] = face_vertices[i];
                face[2] = face_vertices[i + 1];
                m_faces.push_back(face);
            }
        }
        else if (type == "vn") {
            glm::vec3 normal{0, 0, 0};
            iss >> normal.x >> normal.y >> normal.z;
            m_normals.push_back(normal);
        }
        else if (type == "vt") {
            glm::vec2 texture_vertex{};
            iss >> texture_vertex.x >> texture_vertex.y;
            m_texture_vertices.push_back(texture_vertex);
        }
    }
}
