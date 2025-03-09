#include <Parser.hpp>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <iostream>

std::unique_ptr<Parser> Parser::create_parser(const std::string& format){
    std::unique_ptr<Parser> parser{};
    if (format == "obj"){
        parser.reset(new ParserOBJ());
    }
    else if (format == "gltf"){
        parser.reset(new ParserGLTF());
    }
    return parser;
}

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
            Point normal{0, 0, 0, 1};
            iss >> normal.x >> normal.y >> normal.z;
            m_normals.push_back(normal);
        }
    }
}

void ParserGLTF::parse_file(const std::string& file_path) {
    using json = nlohmann::json;

    auto get_folder = [](const std::string& file_path) -> std::string {
        auto slash_index = file_path.find_last_of('/');
        if (slash_index == std::string::npos){
            return "";
        }
        return file_path.substr(0, slash_index + 1);
    };

    std::ifstream file(file_path);
    if (!file.is_open()) {
        return;
    }
    
    json gltf;
    file >> gltf;
    
    std::string current_folder = get_folder(file_path);
    std::vector<std::vector<uint8_t>> buffers;

    for (const auto& buffer : gltf["buffers"]) {
        std::string buffer_file_path = 
            current_folder + std::string(buffer["uri"]);

        std::ifstream bin_file(buffer_file_path, std::ios::binary);
        if (!bin_file.is_open()) {
            return;
        }

        buffers.emplace_back(std::vector<uint8_t>(
            (std::istreambuf_iterator<char>(bin_file)), 
            std::istreambuf_iterator<char>()
        ));
    }

    for (const auto& accessor : gltf["accessors"]) {
        std::string accessor_type = accessor["type"];

        if (accessor_type == "VEC3" || 
            accessor_type == "SCALAR") 
        {
            const int bufferViewIndex = accessor["bufferView"];
            const auto& bufferView = gltf["bufferViews"][bufferViewIndex];

            const size_t byteOffset = bufferView.value("byteOffset", 0);
            const size_t count = accessor["count"];

            const auto& buffer = buffers[bufferView["buffer"]];

            if (accessor_type == "VEC3") {
                const float* data = reinterpret_cast<const float*>(&buffer[byteOffset]);
    
                for (size_t index = 0; index < count * 3; index += 3) {
                    m_vertices.emplace_back(Point{ 
                        data[index], 
                        data[index + 1], 
                        data[index + 2], 
                        1.0 
                    });
                }
            }
            else {
                const uint32_t* data = reinterpret_cast<const uint32_t*>(&buffer[byteOffset]);

                for (size_t index = 0; index < count; index += 3) {
                    m_faces.emplace_back(Face{
                        static_cast<uint32_t>(data[index]),
                        static_cast<uint32_t>(data[index + 1]),
                        static_cast<uint32_t>(data[index + 2])
                    });
                }
            }
        }
    }
}