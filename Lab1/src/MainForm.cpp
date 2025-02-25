#include "MainForm.hpp"
#include <algorithm>
#include <memory>
#include <iostream>
#include <format>

using namespace std::string_literals;

MainForm::MainForm() noexcept
    : m_window(sf::VideoMode(width, height), "Lab №1")
    , m_bitmap(width, height)
    , m_camera(std::make_shared<Camera>())
    , m_counter(std::make_shared<FPSCounter>())
    , m_logger(std::make_shared<Logger>())
{ 
    m_window.setMouseCursorVisible(false);
    m_texture.create(width, height);
    m_logger->set_camera(m_camera);
    m_logger->set_fps_counter(m_counter);
    m_scene.initialize();
}

void MainForm::run_main_loop() {
    if (!m_logger->initialize()){
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

    auto vertices = m_scene.get_vertices();
    auto faces = m_scene.get_faces();
    auto transformed_vertices = m_camera->transform_vertices(vertices);

    m_bitmap.draw_faces(transformed_vertices, faces);
    m_texture.update(m_bitmap.data());

    m_window.clear();
    m_window.draw(sprite);
    m_logger->draw(m_window);
    m_window.display();
    
    m_counter->update();
    m_logger->update();
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        m_camera->move(MoveDirection::FORWARD);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        m_camera->move(MoveDirection::BACK);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        m_camera->move(MoveDirection::LEFT);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        m_camera->move(MoveDirection::RIGHT);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        m_scene.rotate_model({rotation_angle, 0, 0});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        m_scene.rotate_model({0, rotation_angle, 0});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
        m_scene.rotate_model({0, 0, rotation_angle});
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) {
        m_scene.move_model({move_distance, 0, 0});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) {
        m_scene.move_model({-move_distance, 0, 0});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3)) {
        m_scene.move_model({0, move_distance, 0});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) {
        m_scene.move_model({0, -move_distance, 0});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5)) {
        m_scene.move_model({0, 0, move_distance});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6)) {
        m_scene.move_model({0, 0, -move_distance});
    }
}

void MainForm::on_key_press(sf::Keyboard::Key code) {
    switch (code)
    {
        case sf::Keyboard::Q:
            m_window.close();
            break;
    }
}
