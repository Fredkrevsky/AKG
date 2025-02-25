#pragma once
#include "Matrix.hpp"
#include <string>


class Parser {
public:
    Parser() noexcept = default;
    virtual ~Parser() noexcept = default;

    virtual void parse_file(const std::string& file_path) = 0;
    Vertices get_vertices() const;
    Faces get_faces() const;
    
protected:
    Vertices m_vertices;
    Faces m_faces;
};


class ParserOBJ final : public Parser {
public:
    ParserOBJ() noexcept = default;
    ~ParserOBJ() noexcept = default;

    virtual void parse_file(const std::string& file_path) override;
};