#include "MainForm.hpp"
#include <algorithm>
#include <memory>
#include <iostream>
#include <format>
#include <map>

using namespace std::string_literals;

MainForm::MainForm() noexcept
    : m_window(sf::VideoMode(width, height), "Lab №1")
    , m_bitmap(width, height)
    , m_camera(std::make_shared<Camera>())
    , m_counter(std::make_shared<FPSCounter>())
{ 
    m_window.setMouseCursorVisible(false);
    m_texture.create(width, height);
    m_logger.set_camera(m_camera);
    m_logger.set_fps_counter(m_counter);
}

void MainForm::run_main_loop() {
    if (!m_logger.initialize() ||
        !m_scene.initialize()) {
        return;
    }

    sf::Mouse::setPosition(center, m_window);

    while (m_window.isOpen()) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                on_key_press(event.key.code);
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    bool is_getting_closer = event.mouseWheelScroll.delta > 0;
                    m_camera->scale(is_getting_closer);
                }
            }
        }

        handle_mouse_rotation();
        handle_keyboard_movement();
        draw();
    }
}

void MainForm::draw() {
    static sf::Sprite sprite{m_texture};

    auto transform_vertices = [&](Vertices& vertices){
        TransformMatrix transform_matrix = m_camera->get_transform_matrix();
    
        std::ranges::for_each(vertices, [&](auto& vertex) {
            vertex *= transform_matrix;
            if (vertex.w >= 1.0){
                vertex *= (1 / vertex.w);
            }
        });
    };

    auto vertices = m_scene.get_vertices();
    auto faces = m_scene.get_faces();
    transform_vertices(vertices);

    m_bitmap.draw_faces(vertices, faces);
    m_texture.update(m_bitmap.data());

    m_window.clear();
    m_window.draw(sprite);
    m_logger.draw(m_window);
    m_window.display();
    
    m_counter->update();
    m_logger.update();
}

void MainForm::handle_mouse_rotation() {
    sf::Vector2i mouse_position = sf::Mouse::getPosition(m_window);

    int deltaX = center.x - mouse_position.x;
    int deltaY = center.y - mouse_position.y;

    if (deltaX != 0 || deltaY != 0) {
        double angle_x = deltaX * sensitivity;
        double angle_y = deltaY * sensitivity;
        m_camera->rotate(angle_x, angle_y);

        sf::Mouse::setPosition(center, m_window);
    }
}

void MainForm::handle_keyboard_movement() {
    constexpr double rotation_angle = 0.1;
    constexpr double move_distance = 0.1;

    constexpr static std::array<std::pair<sf::Keyboard::Key, MoveDirection>, 4> camera_movement_keys = {{
        {sf::Keyboard::W, MoveDirection::FORWARD},
        {sf::Keyboard::S, MoveDirection::BACK},
        {sf::Keyboard::A, MoveDirection::LEFT},
        {sf::Keyboard::D, MoveDirection::RIGHT}
    }};

    constexpr static std::array<std::pair<sf::Keyboard::Key, Point>, 3> model_rotation_keys = {{
        {sf::Keyboard::Num1, {rotation_angle, 0, 0}},
        {sf::Keyboard::Num2, {0, rotation_angle, 0}},
        {sf::Keyboard::Num3, {0, 0, rotation_angle}}
    }};

    constexpr static std::array<std::pair<sf::Keyboard::Key, Point>, 6> model_move_keys = {{
        {sf::Keyboard::Numpad1, {move_distance, 0, 0}},
        {sf::Keyboard::Numpad2, {-move_distance, 0, 0}},
        {sf::Keyboard::Numpad3, {0, move_distance, 0}},
        {sf::Keyboard::Numpad4, {0, -move_distance, 0}},
        {sf::Keyboard::Numpad5, {0, 0, move_distance}},
        {sf::Keyboard::Numpad6, {0, 0, -move_distance}}
    }};

    std::ranges::for_each(camera_movement_keys, [&](const auto& element) {
        const auto& [key, direction] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_camera->move(direction);
        }
    });

    std::ranges::for_each(model_rotation_keys, [&](const auto& element) {
        const auto& [key, rotation] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_scene.rotate_model(rotation);
        }
    });

    std::ranges::for_each(model_move_keys, [&](const auto& element) {
        const auto& [key, move] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_scene.move_model(move);
        }
    });
}

void MainForm::on_key_press(sf::Keyboard::Key code) {
    switch (code)
    {
        case sf::Keyboard::Q:
            m_window.close();
            break;
    }
}
