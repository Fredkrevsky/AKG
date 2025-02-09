#include <MainForm.hpp>
#include <algorithm>

constexpr static double t = (1.0f + sqrt(5.0f)) / 2.0f;     // Золотое сечение, надо просто для рисования фигуры

std::vector<Point3D> vertices = {  
        
    // Массив вершин фигуры. Поменяешь - будет рисоваться другая
    
    Point3D{-1,  t,  0}, Point3D{1,  t,  0}, Point3D{-1, -t,  0}, Point3D{1, -t,  0},
    Point3D{0, -1,  t}, Point3D{0,  1,  t}, Point3D{0, -1, -t}, Point3D{0,  1, -t},
    Point3D{t,  0, -1}, Point3D{t,  0,  1}, Point3D{-t,  0, -1}, Point3D{-t,  0,  1}
};

std::vector<Face> faces = {      
    
    // Массив связей вершин фигуры. Каждый элемент - это плоскость, в данном случае все плоскости треугольники но можешь поменять. 
    // Числа в векторах это индексы точек в массиве vertices               
    
    {0, 11, 5}, {0,  5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
    {1, 5,  9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
};

// Твоя задача: из файла заполнить эти два массива


MainForm::MainForm() 
    : m_window(sf::VideoMode(width, height), "Lab #1") 
{ 
    m_window.setFramerateLimit(144);
}

void MainForm::run_main_loop() {
    sf::Vector2i mouse_press_position{};

    while (m_window.isOpen()) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                mouse_press_position = sf::Mouse::getPosition(m_window);
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                m_old_angleX = normalize_angle(m_angleX);
                m_old_angleY = normalize_angle(m_angleY);
            }
            else if (event.type == sf::Event::KeyPressed) {
                on_key_press(event.key.code);
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            auto mouse_position = sf::Mouse::getPosition(m_window);
            auto [mouse_dx, mouse_dy] = mouse_position - mouse_press_position;

            m_angleX = m_old_angleX + mouse_dy * sensitivity;
            m_angleY = m_old_angleY + mouse_dx * sensitivity;
        }
        
        // Вершины для отрисовки должны быть в vertices
        auto rotated_vertices = rotate_vertices(vertices);

        // Грани фигуры должны быть в faces 
        // Поменяй глобальные массивы на parser.get_faces() и parser.get_vertices(), например
        draw_vertices(rotated_vertices, faces);
    }
}

void MainForm::on_key_press(sf::Keyboard::Key code) {
    switch (code){
        case sf::Keyboard::Left: 
            m_old_angleY += keyboard_sensitivity;
            break;
        case sf::Keyboard::Up: 
            m_old_angleX += keyboard_sensitivity; 
            break;
        case sf::Keyboard::Right: 
            m_old_angleY -= keyboard_sensitivity;
            break;
        case sf::Keyboard::Down: 
            m_old_angleX -= keyboard_sensitivity;
            break;
        case sf::Keyboard::Q:
            m_window.close();
            break;
        default: 
            break;
    }
    m_angleX = m_old_angleX;
    m_angleY = m_old_angleY;
}

std::vector<Point3D> MainForm::rotate_vertices(const std::vector<Point3D>& vertices){
    m_angleX = normalize_angle(m_angleX);
    m_angleY = normalize_angle(m_angleY);
    auto rotationX = createRotationX(m_angleX);
    auto rotationY = createRotationY(m_angleY);

    std::vector<Point3D> rotated_vertices;
    std::ranges::transform(vertices, std::back_inserter(rotated_vertices), 
        [&](const auto& vertex) {
        Point3D rotated = vertex;
        rotated = rotated * rotationX;
        rotated = rotated * rotationY;
        return rotated;
    });

    return rotated_vertices;
}

void MainForm::draw_line(sf::VertexArray& vertices, int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        vertices.append(sf::Vertex(sf::Vector2f(x1, y1), sf::Color::White));

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void MainForm::draw_vertices(const std::vector<Point3D>& vertices, const std::vector<Face>& faces) {

    m_window.clear();

    sf::VertexArray points(sf::Points);

    for (const auto& face : faces) {
        for (size_t i = 0; i < face.size(); ++i) {
            const auto& vertex1 = vertices[face[i]];
            const auto& vertex2 = vertices[face[(i + 1) % face.size()]];

            int x1 = width / 2 + static_cast<int>(vertex1.x * scale);
            int y1 = height / 2 - static_cast<int>(vertex1.y * scale);
            int x2 = width / 2 + static_cast<int>(vertex2.x * scale);
            int y2 = height / 2 - static_cast<int>(vertex2.y * scale);

            draw_line(points, x1, y1, x2, y2);
        }
    }

    m_window.draw(points);
    m_window.display();
}