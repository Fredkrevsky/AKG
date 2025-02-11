#include <SFML/Window.hpp>
#include <MainForm.hpp>
#include <algorithm>
#include <memory>

using namespace std::string_literals;

const std::string FILE_NAME = "models/logan.obj";

constexpr static int FPS = 144;
constexpr static double t = (1.0 + std::sqrt(5.0)) / 2.0;

static std::vector<Point3D> default_vertices = {      
    Point3D{-1,  t,  0}, Point3D{1,  t,  0}, Point3D{-1, -t,  0}, Point3D{1, -t,  0},
    Point3D{0, -1,  t}, Point3D{0,  1,  t}, Point3D{0, -1, -t}, Point3D{0,  1, -t},
    Point3D{t,  0, -1}, Point3D{t,  0,  1}, Point3D{-t,  0, -1}, Point3D{-t,  0,  1}
};

static std::vector<Face> default_faces = {          
    {0, 11, 5}, {0,  5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
    {1, 5,  9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
};


MainForm::MainForm() 
    : m_window(sf::VideoMode(width, height), "Lab #1") 
{ 
    m_window.setFramerateLimit(FPS);
    m_vertices = default_vertices;
    m_faces = default_faces;
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
            else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    if (event.mouseWheelScroll.delta > 0) {
                        scale *= scale_sensitivity;
                    } else {
                        scale /= scale_sensitivity;
                    }
                }
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            auto mouse_position = sf::Mouse::getPosition(m_window);
            auto [mouse_dx, mouse_dy] = mouse_position - mouse_press_position;

            m_angleX = m_old_angleX + mouse_dy * sensitivity;
            m_angleY = m_old_angleY + mouse_dx * sensitivity;
        }
        
        auto vertices = m_vertices;
        rotate_vertices(vertices, m_angleX, m_angleY);
        move_vertices(vertices, m_posX, m_posY);
        draw_vertices(vertices, m_faces);
    }
}

void MainForm::on_key_press(sf::Keyboard::Key code) {
    switch (code){
        case sf::Keyboard::Left: 
            m_posX -= keyboard_sensitivity;
            break;
        case sf::Keyboard::Up: 
            m_posY += keyboard_sensitivity; 
            break;
        case sf::Keyboard::Right: 
            m_posX += keyboard_sensitivity;
            break;
        case sf::Keyboard::Down: 
            m_posY -= keyboard_sensitivity;
            break;
        case sf::Keyboard::L:
            load_from_file();
            m_old_angleX = 0.0;
            m_old_angleY = 0.0;
            m_posX = 0.0;
            m_posY = 0.0;
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

void MainForm::load_from_file() {
    std::unique_ptr<Parser> parser{nullptr};
    std::string file_path{FILE_NAME};

    auto get_file_format = [](const std::string& file_path) -> std::string {
        std::size_t dot_index = file_path.find_last_of(".");
        if (dot_index == std::string::npos) {
            return ""s;
        }
        return file_path.substr(dot_index);
    };

    std::string file_format = get_file_format(file_path);

    if (file_format == ".obj"){
        parser.reset(new ParserOBJ());
    }
    else if (file_format == ".gltf"){
        parser.reset(nullptr);
        return;
    }
    else {
        return;
    }

    parser->parse_file(file_path);
    m_vertices = parser->get_vertices();
    m_faces = parser->get_faces();
}