#include "MainForm.hpp"
#include <algorithm>
#include <memory>
#include <format>
#include <map>

using namespace std::string_literals;

MainForm::MainForm() noexcept
    : m_window(sf::VideoMode(width, height), "Lab 3")
    , m_camera(std::make_shared<Camera>())
    , m_counter(std::make_shared<FPSCounter>())
    , m_needs_update(true)
{ 
    m_window.setMouseCursorVisible(false);
    m_window.setFramerateLimit(144);
    m_texture.create(width, height);
    m_renderer.set_camera(m_camera);
    m_logger.set_camera(m_camera);
    m_logger.set_fps_counter(m_counter);
}

void MainForm::run_main_loop() {
    if (!m_logger.initialize() ||
        !m_scene.initialize()) {
        return;
    }

    sf::Clock frame_clock;
    bool flag = true;
    sf::Mouse::setPosition(center, m_window);

    while (m_window.isOpen()) {
        const float delta_time = frame_clock.restart().asSeconds();

        sf::Event event;
        while (m_window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    m_window.close();
                    break;

                case sf::Event::KeyPressed:
                    on_key_press(event.key.code);
                    break;

                case sf::Event::MouseWheelScrolled:
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                        bool mode = event.mouseWheelScroll.delta > 0;
                        m_camera->scale(mode);
                        m_needs_update = true;
                    }
                    break;

                case sf::Event::GainedFocus:
                    m_window.setMouseCursorVisible(false);
                    sf::Mouse::setPosition(center, m_window);
                    break;

                case sf::Event::LostFocus:
                    m_window.setMouseCursorVisible(true);
                    break;

                default:
                    break;
            }
        }

        if (m_window.hasFocus() && !flag){
            handle_mouse_rotation();
            handle_keyboard_movement(delta_time);
            draw();
        }
        
        flag = false;
    }
}

void MainForm::draw() {
    static sf::Sprite sprite{m_texture};

    if (m_needs_update) {
        auto&& points = m_scene.get_points();
        const auto& faces = m_scene.get_faces();
        const auto& normals = m_scene.get_normals();
        const auto& texture_vertices = m_scene.get_texture_vertices();
        m_renderer.draw(std::move(points), faces, normals, texture_vertices);
        m_texture.update(m_renderer.data());
        m_needs_update = false;
    }

    m_window.clear();
    m_window.draw(sprite);
    m_logger.draw(m_window);
    m_window.display();
    
    m_counter->update();
    m_logger.update();
}

void MainForm::handle_mouse_rotation() {
    sf::Vector2i mouse_position = sf::Mouse::getPosition(m_window);

    int delta_x = center.x - mouse_position.x;
    int delta_y = center.y - mouse_position.y;

    if (delta_x != 0 || delta_y != 0) {
        float angle_x = delta_x * sensitivity;
        float angle_y = delta_y * sensitivity;

        m_camera->rotate(angle_x, angle_y);
        m_needs_update = true;

        sf::Mouse::setPosition(center, m_window);
    }
}

void MainForm::handle_keyboard_movement(float delta_time) {
    constexpr static float rotation_angle = 2.0;
    constexpr static float move_distance = 2.0;

    constexpr static std::array<std::pair<sf::Keyboard::Key, Camera::MoveDirection>, 4> camera_movement_keys = {{
        {sf::Keyboard::W, Camera::MoveDirection::FORWARD},
        {sf::Keyboard::S, Camera::MoveDirection::BACK},
        {sf::Keyboard::A, Camera::MoveDirection::LEFT},
        {sf::Keyboard::D, Camera::MoveDirection::RIGHT}
    }};

    constexpr static std::array<std::pair<sf::Keyboard::Key, glm::vec4>, 3> model_rotation_keys = {{
        {sf::Keyboard::Num1, {rotation_angle, 0, 0, 0}},
        {sf::Keyboard::Num2, {0, rotation_angle, 0, 0}},
        {sf::Keyboard::Num3, {0, 0, rotation_angle, 0}}
    }};

    constexpr static std::array<std::pair<sf::Keyboard::Key, glm::vec4>, 6> model_move_keys = {{
        {sf::Keyboard::Numpad1, {move_distance, 0, 0, 0}},
        {sf::Keyboard::Numpad2, {-move_distance, 0, 0, 0}},
        {sf::Keyboard::Numpad3, {0, move_distance, 0, 0}},
        {sf::Keyboard::Numpad4, {0, -move_distance, 0, 0}},
        {sf::Keyboard::Numpad5, {0, 0, move_distance, 0}},
        {sf::Keyboard::Numpad6, {0, 0, -move_distance, 0}}
    }};

    std::ranges::for_each(camera_movement_keys, [&](const auto& element) {
        const auto& [key, direction] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_camera->move(direction, delta_time);
            m_scene.update_points();
            m_needs_update = true;
        }
    });

    std::ranges::for_each(model_rotation_keys, [&](const auto& element) {
        const auto& [key, rotation] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_scene.rotate_model(rotation, delta_time);
            m_needs_update = true;
        }
    });

    std::ranges::for_each(model_move_keys, [&](const auto& element) {
        const auto& [key, move] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_scene.move_model(move, delta_time);
            m_needs_update = true;
        }
    });
}

void MainForm::on_key_press(sf::Keyboard::Key code) {
    switch (code)
    {
        case sf::Keyboard::Q:
            // m_window.close();
            break;
    }
}
