#include <Parser.hpp>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

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

void ParserGLTF::parse_file(const std::string& file_path) {
    using json = nlohmann::json;

    std::ifstream file(file_path);
    if (!file.is_open()) {
        return;
    }

    json gltf;
    file >> gltf;

    std::vector<std::vector<uint8_t>> buffers;
    for (const auto& buffer : gltf["buffers"]) {
        std::ifstream bin_file(buffer["uri"], std::ios::binary);
        if (!bin_file.is_open()) {
            return;
        }
        buffers.emplace_back(std::istreambuf_iterator<char>(bin_file), {});
    }

    for (const auto& accessor : gltf["accessors"]) {
        if (accessor["type"] == "VEC3") {
            int bufferViewIndex = accessor["bufferView"];
            const auto& bufferView = gltf["bufferViews"][bufferViewIndex];

            size_t byteOffset = bufferView.value("byteOffset", 0);
            size_t count = accessor["count"];
            size_t stride = bufferView.value("byteStride", sizeof(float) * 3);

            const auto& buffer = buffers[bufferView["buffer"]];
            const float* data = reinterpret_cast<const float*>(&buffer[byteOffset]);

            for (size_t i = 0; i < count; ++i) {
                Point vertex{ data[i * 3], data[i * 3 + 1], data[i * 3 + 2], 1 };
                m_vertices.push_back(vertex);
            }
        }
    }

    for (const auto& accessor : gltf["accessors"]) {
        if (accessor["type"] == "SCALAR") {
            int bufferViewIndex = accessor["bufferView"];
            const auto& bufferView = gltf["bufferViews"][bufferViewIndex];

            size_t byteOffset = bufferView.value("byteOffset", 0);
            size_t count = accessor["count"];
            size_t componentSize = 2;

            const auto& buffer = buffers[bufferView["buffer"]];
            const uint16_t* data = reinterpret_cast<const uint16_t*>(&buffer[byteOffset]);

            Face face;
            for (size_t i = 0; i < count; ++i) {
                face.push_back(static_cast<int>(data[i]));
                if ((i + 1) % 3 == 0) {
                    m_faces.push_back(face);
                    face.clear();
                }
            }
        }
    }
}