#pragma once
#include "Matrix.hpp"
#include "Point.hpp"

class Model final {
public:
    Model() noexcept = default;

    void set_data(Vertices&& vertices, Faces&& faces, Normals&& normals);

    Points get_points() const;
    Faces get_faces() const;

private:
    Points m_points;
    Faces m_faces;
};