#include <Parser.hpp>
#include <fstream>
#include <sstream>


Vertices Parser::get_vertices() const {
    return m_vertices;
}

Faces Parser::get_faces() const {
    return m_faces;
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
            Point vertex{0, 0, 0, 1};
            iss >> vertex.x >> vertex.y >> vertex.z;
            m_vertices.push_back(vertex);
        } 
        else if (type == "f") {
            Face face;
            std::string vertex_data;
            while (iss >> vertex_data) {
                std::istringstream vertex_stream(vertex_data);
                std::string vertex_index_str;
                std::getline(vertex_stream, vertex_index_str, '/');
                face.push_back(std::stoi(vertex_index_str) - 1);
            }
            m_faces.push_back(face);
        }
    }
}