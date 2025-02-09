#pragma once
#include <Matrix.hpp>
#include <string>

using Vertices = std::vector<Point3D>;
using Faces = std::vector<Face>;


class Parser {
public:
    Parser() = default;
    virtual ~Parser() = default;

    virtual void parse_file(const std::string& file_path) = 0;
    Vertices get_vertices() const;
    Faces get_faces() const;
    
protected:
    Vertices m_vertices;
    Faces m_faces;
};


class ParserOBJ final : public Parser {
public:
    ParserOBJ() = default;
    ~ParserOBJ() = default;

    virtual void parse_file(const std::string& file_path) override;
};