#pragma once
#include "Matrix.hpp"
#include <string>
#include <memory>


class Parser {
public:
    Parser() noexcept = default;
    virtual ~Parser() noexcept = default;

    static std::unique_ptr<Parser> create_parser(const std::string& format);

    virtual void parse_file(const std::string& file_path) = 0;
    Vertices get_vertices() const;
    Faces get_faces() const;
    
protected:
    Vertices m_vertices;
    Faces m_faces;
    Normals m_normals;
};


class ParserOBJ final : public Parser {
public:
    ParserOBJ() noexcept = default;
    ~ParserOBJ() noexcept = default;

    void parse_file(const std::string& file_path) override;
};

class ParserGLTF final : public Parser {
public:
    ParserGLTF() noexcept = default;
    ~ParserGLTF() noexcept = default;

    void parse_file(const std::string& file_path) override;
};