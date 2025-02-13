#include <SFML/Window.hpp>
#include <MainForm.hpp>
#include <algorithm>
#include <memory>

using namespace std::string_literals;

constexpr auto FILE_NAME = "models/logan.obj";
constexpr static int FPS = 150;
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


MainForm::MainForm() noexcept
    : m_window(sf::VideoMode(width, height), "Lab #1") 
{ 
    m_window.setFramerateLimit(FPS);
    m_vertices = default_vertices;
    m_faces = default_faces;
}

void MainForm::run_main_loop() {
    if (!m_counter.initialize()){
        return;
    }

    sf::Texture texture;
    texture.create(width, height);
    sf::Sprite sprite(texture);

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
        draw_vertices(vertices);
        texture.update((uint8_t*)m_bitmap.data());

        m_window.clear();
        m_window.draw(sprite);
        m_counter.draw(m_window);
        m_window.display();
        m_counter.update();
    }
}

void MainForm::on_key_press(sf::Keyboard::Key code) {
    
    auto reset_position = [&](){
        m_old_angleX = 0.0;
        m_old_angleY = 0.0;
    };

    auto load_default = [&](){
        m_vertices = default_vertices;
        m_faces = default_faces;
    };

    switch (code){
        case sf::Keyboard::Left: 
            move_vertices(m_vertices, -keyboard_sensitivity, 0);
            break;
        case sf::Keyboard::Up: 
            move_vertices(m_vertices, 0, keyboard_sensitivity);
            break;
        case sf::Keyboard::Right: 
            move_vertices(m_vertices, keyboard_sensitivity, 0);
            break;
        case sf::Keyboard::Down: 
            move_vertices(m_vertices, 0, -keyboard_sensitivity);
            break;
        case sf::Keyboard::L:
            load_from_file();
            reset_position();
            break;
        case sf::Keyboard::D:
            load_default();
            reset_position();
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

void MainForm::draw_line(int x1, int y1, int x2, int y2) {
    
    constexpr static uint32_t WHITE = 0xFFFFFFFF;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
            m_bitmap[y1 * width + x1] = WHITE;
        }
        else {
            break;
        }

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

void MainForm::draw_vertices(const std::vector<Point3D>& vertices) {
    std::ranges::fill(m_bitmap, 0);

    std::ranges::for_each(m_faces, [&, this](const Face& face) {
        const size_t face_size = face.size();
        for (size_t i = 0; i < face_size; ++i) {
            const auto& vertex1 = vertices[face[i]];
            const auto& vertex2 = vertices[face[(i + 1) % face_size]];

            int x1 = width / 2 + static_cast<int>(vertex1.x * scale);
            int y1 = height / 2 - static_cast<int>(vertex1.y * scale);
            int x2 = width / 2 + static_cast<int>(vertex2.x * scale);
            int y2 = height / 2 - static_cast<int>(vertex2.y * scale);

            draw_line(x1, y1, x2, y2);
        }
    });
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