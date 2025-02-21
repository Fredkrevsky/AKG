#include "MainForm.hpp"
#include <algorithm>
#include <memory>
#include <iostream>
#include <format>

using namespace std::string_literals;

constexpr auto FILE_NAME = "models/logan.obj";
constexpr static int MAX_FPS = 15000;
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
    m_window.setFramerateLimit(MAX_FPS);
    m_vertices = default_vertices;
    m_faces = default_faces;
    update_camera();
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
                is_mouse_pressed = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                is_mouse_pressed = false;
            }
            else if (event.type == sf::Event::KeyPressed) {
                on_key_press(event.key.code);
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    if (event.mouseWheelScroll.delta > 0) {
                        scalar *= 1.2;
                    } else {
                        scalar /= 1.2;
                    }
                }
            }
        }

        handle_mouse_rotation();
        handle_keyboard_movement();
        update_camera();

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

void MainForm::update_camera() {
    Point ZAxis = (eye - target).normalize();
    Point XAxis = up.cross(ZAxis).normalize();
    Point YAxis = ZAxis.cross(XAxis).normalize();

    m_view_matrix = {{
        {XAxis.x, XAxis.y, XAxis.z, -XAxis.dot(eye)},
        {YAxis.x, YAxis.y, YAxis.z, -YAxis.dot(eye)},
        {ZAxis.x, ZAxis.y, ZAxis.z, -ZAxis.dot(eye)},
        {0, 0, 0, 1}
    }};

    double znear = 0.01; 
    double zfar = 100.0; 
    double aspect = static_cast<double>(width) / height; 
    double fov = 2.0 * PI / 3.0;

    m_projection_matrix = {{
        {1.0 / (aspect * std::tan(fov / 2.0)), 0, 0, 0},
        {0, 1.0 / std::tan(fov / 2.0), 0, 0},
        {0, 0, zfar / (znear - zfar), zfar * znear / (znear - zfar)},
        {0, 0, -1, 0}
    }};

    m_viewport_matrix = {{
        {width / 2, 0, 0, width / 2},
        {0, -height / 2, 0, height / 2},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};

    m_scale = createScale(scalar); 
}

std::vector<Point> MainForm::transform_vertices(const std::vector<Point>& vertices) {
    std::vector<Point> transformed_vertices;
    TransformMatrix transform_matrix = m_viewport_matrix * m_projection_matrix * m_view_matrix * m_scale;

    std::transform(vertices.begin(), vertices.end(), std::back_inserter(transformed_vertices), 
        [&](const Point& vertex)
    {
        auto transformed_vertex = vertex * transform_matrix;
        transformed_vertex = transformed_vertex * (1 / transformed_vertex.w);
        return transformed_vertex;
    });

    return transformed_vertices;
}

void MainForm::handle_mouse_rotation() {
    if (is_mouse_pressed) {
        auto mouse_position = sf::Mouse::getPosition(m_window);
        auto [mouse_dx, mouse_dy] = mouse_position - mouse_press_position;

        if (mouse_dx != 0 || mouse_dy != 0){
            double angleX = mouse_dy * sensitivity;
            double angleY = mouse_dx * sensitivity;

            auto rotationX = createRotationX(angleX);
            auto rotationY = createRotationY(angleY);
            auto rotation = rotationX * rotationY;

            eye = eye * rotation;
            up = up * rotation;
        }
        mouse_press_position = mouse_position;
    }
}

void MainForm::handle_keyboard_movement() {
    Point forward = (target - eye).normalize();
    Point right = up.cross(forward).normalize();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        eye = eye + forward * camera_speed;
        target = target + forward * camera_speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        eye = eye - forward * camera_speed;
        target = target - forward * camera_speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        eye = eye - right * camera_speed;
        target = target - right * camera_speed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        eye = eye + right * camera_speed;
        target = target + right * camera_speed;
    }
}

void MainForm::on_key_press(sf::Keyboard::Key code) {
    auto load_default = [&](){
        m_vertices = default_vertices;
        m_faces = default_faces;
    };

    switch (code){
        case sf::Keyboard::L:
            load_from_file();
            break;
        case sf::Keyboard::K:
            load_default();
            break;
        case sf::Keyboard::Q:
            m_window.close();
            break;
        default: 
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
    else if (file_format == ".gltf"){
        parser.reset(nullptr);            //Implement GLTF parser
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
