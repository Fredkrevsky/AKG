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
            Point3D vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            m_vertices.push_back(vertex);
        } 
        else if (type == "f") {
            Face face;
            int vertex_index;
            while (iss >> vertex_index) {
                face.push_back(vertex_index - 1);
            }
            m_faces.push_back(face);
        }
    }
}