#pragma once
#include "Matrix.hpp"
#include <string>
#include <memory>

class Parser {
public:
    Parser() noexcept = default;
    virtual ~Parser() = default;

    static std::unique_ptr<Parser> create_parser(const std::string& format);
    static std::string get_format(const std::string& path);

    virtual void parse_file(const std::string& file_path) = 0;
    
    Vertices get_vertices() const;
    Faces get_faces() const;
    Vertices get_normals() const;
    
protected:
    Vertices m_vertices;
    Faces m_faces;
    Vertices m_normals;
};

class ParserOBJ final : public Parser {
public:
    ParserOBJ() noexcept = default;
    ~ParserOBJ() noexcept = default;

    void parse_file(const std::string& file_path) override;
};
