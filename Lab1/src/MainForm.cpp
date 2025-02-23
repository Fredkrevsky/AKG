#include "MainForm.hpp"
#include <algorithm>
#include <memory>
#include <iostream>
#include <format>

using namespace std::string_literals;

constexpr auto FILE_NAME = "models/logan.obj";
constexpr static int THREADS_COUNT = 12;
constexpr static double t = (1.0 + std::sqrt(5.0)) / 2.0;

static std::vector<Point> default_vertices = {      
    Point{-1,  t,  0}, Point{1,  t,  0}, Point{-1, -t,  0}, Point{1, -t,  0},
    Point{0, -1,  t}, Point{0,  1,  t}, Point{0, -1, -t}, Point{0,  1, -t},
    Point{t,  0, -1}, Point{t,  0,  1}, Point{-t,  0, -1}, Point{-t,  0,  1}
};

static std::vector<Face> default_faces = {          
    {0, 11, 5}, {0,  5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
    {1, 5,  9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
};

MainForm::MainForm() noexcept
    : m_window(sf::VideoMode(width, height), "Lab №1") 
    , m_bitmap(width, height)
{ 
    m_vertices = default_vertices;
    m_faces = default_faces;
    m_window.setMouseCursorVisible(false);
}

void MainForm::run_main_loop() {
    if (!m_counter.initialize()){
        return;
    }

    sf::Texture texture;
    texture.create(width, height);
    sf::Sprite sprite(texture);

    while (m_window.isOpen()) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                mouse_press_position = sf::Mouse::getPosition(m_window);
            }
            else if (event.type == sf::Event::KeyPressed) {
                on_key_press(event.key.code);
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    bool is_getting_closer = event.mouseWheelScroll.delta > 0;
                    m_camera.scale(is_getting_closer);
                }
            }
        }

        handle_mouse_rotation();
        handle_keyboard_movement();

        auto vertices = transform_vertices(m_vertices);
        m_bitmap.draw_faces(vertices, m_faces, THREADS_COUNT);
        texture.update(m_bitmap.data());

        m_window.clear();
        m_window.draw(sprite);
        m_counter.draw(m_window);
        m_window.display();
        m_counter.update();
    }
}


std::vector<Point> MainForm::transform_vertices(const std::vector<Point>& vertices) {
    std::vector<Point> transformed_vertices;

    TransformMatrix transform_matrix = m_camera.get_transform_matrix();

    std::ranges::transform(vertices, std::back_inserter(transformed_vertices), 
        [&](const Point& vertex)
    {
        auto transformed_vertex = vertex * transform_matrix;
        double w = transformed_vertex.w;
        transformed_vertex = transformed_vertex * (1 / w);
        return transformed_vertex;
    });

    return transformed_vertices;
}

void MainForm::handle_mouse_rotation() {
    auto mouse_position = sf::Mouse::getPosition(m_window);
    auto [mouse_x, mouse_y] = mouse_position;

    if (mouse_x >= 0 && mouse_y >= 0 && 
        mouse_x <= width && mouse_y <= height)
    {
        auto [mouse_dx, mouse_dy] = mouse_position - mouse_press_position;
    
        if (mouse_dx != 0 || mouse_dy != 0){
            double angle_x = mouse_dy * sensitivity;
            double angle_y = mouse_dx * sensitivity;
            m_camera.rotate({angle_x, angle_y, 0.0, 0.0});
        }
        mouse_press_position = mouse_position;
    }
}

void MainForm::handle_keyboard_movement() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        m_camera.move(MoveDirection::FORWARD);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        m_camera.move(MoveDirection::BACK);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        m_camera.move(MoveDirection::LEFT);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        m_camera.move(MoveDirection::RIGHT);
    }
}

void MainForm::on_key_press(sf::Keyboard::Key code) {
    auto load_default = [&](){
        m_vertices = default_vertices;
        m_faces = default_faces;
    };

    switch (code)
    {
        case sf::Keyboard::L:
            load_from_file();
            break;

        case sf::Keyboard::K:
            load_default();
            break;

        case sf::Keyboard::Q:
            m_window.close();
            break;
    }
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
    else {
        parser.reset(nullptr);
    }

    if (parser) {
        parser->parse_file(file_path);
        m_vertices = parser->get_vertices();
        m_faces = parser->get_faces();
    }
}
