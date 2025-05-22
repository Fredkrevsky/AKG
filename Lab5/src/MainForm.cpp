#include "MainForm.hpp"
#include <algorithm>
#include <memory>
#include <format>
#include <map>

using namespace std::string_literals;

namespace {
    
constexpr float MOUSE_SENSITIVITY{0.05};
constexpr float KEYBOARD_SENSITIVITY{0.1};
constexpr int WIDTH{1600};
constexpr int HEIGHT{900};
constexpr int MAX_FPS{144};

}

MainForm::MainForm() noexcept
    : m_window(sf::VideoMode(WIDTH, HEIGHT), "Lab 5")
    , m_camera(std::make_shared<Camera>())
    , m_counter(std::make_shared<FPSCounter>())
    , m_center(WIDTH / 2, HEIGHT / 2)
{ 
    m_window.setMouseCursorVisible(false);
    m_window.setFramerateLimit(MAX_FPS);
    m_texture.create(WIDTH, HEIGHT);
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
    sf::Mouse::setPosition(m_center, m_window);

    while (m_window.isOpen()) {
        m_delta_time = frame_clock.restart().asSeconds();

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
                        bool mode = event.mouseWheelScroll.delta >= 0;
                        //m_camera->scale(mode);
                    }
                    break;

                case sf::Event::GainedFocus:
                    m_window.setMouseCursorVisible(false);
                    sf::Mouse::setPosition(m_center, m_window);
                    break;

                case sf::Event::LostFocus:
                    m_window.setMouseCursorVisible(true);
                    break;

                default:
                    break;
            }
        }

        if (m_window.hasFocus()){
            handle_mouse();
            handle_keyboard();
            draw();
        }        
    }
}

void MainForm::draw() {
    static sf::Sprite sprite{m_texture};

    const auto& vertices = m_scene.get_vertices();
    const auto& faces = m_scene.get_faces();
    const auto& normals = m_scene.get_normals();
    const auto& texture_vertices = m_scene.get_texture_vertices();
    const auto& mtls = m_scene.get_mtls();
    m_scene.update();
    
    m_renderer.draw(vertices, faces, normals, texture_vertices, mtls);
    m_texture.update(m_renderer.data());

    m_window.clear();
    m_window.draw(sprite);
    m_logger.draw(m_window);
    m_window.display();

    m_counter->update();
    m_logger.update();
}

void MainForm::handle_mouse() {
    sf::Vector2i mouse_position = sf::Mouse::getPosition(m_window);

    int delta_x = m_center.x - mouse_position.x;
    int delta_y = m_center.y - mouse_position.y;

    if (delta_x != 0 || delta_y != 0) {
        float angle_x = delta_x * MOUSE_SENSITIVITY;
        float angle_y = delta_y * MOUSE_SENSITIVITY;

        m_camera->rotate(glm::vec3{angle_x, angle_y, 0.f});

        sf::Mouse::setPosition(m_center, m_window);
    }
}

void MainForm::handle_keyboard() {
    constexpr float ROTATION_ANGLE = 2.0;
    constexpr float MOVE_DISTANCE = 2.0;

    constexpr static std::array<std::pair<sf::Keyboard::Key, glm::vec3>, 6> camera_movement_keys = {{
        {sf::Keyboard::W,      { 0,  1,  0}},
        {sf::Keyboard::S,      { 0, -1,  0}},
        {sf::Keyboard::A,      { 1,  0,  0}},
        {sf::Keyboard::D,      {-1,  0,  0}},
        {sf::Keyboard::LShift, { 0,  0,  1}},
        {sf::Keyboard::Space,  { 0,  0, -1}}
    }};

    constexpr static std::array<std::pair<sf::Keyboard::Key, glm::vec3>, 3> model_rotation_keys = {{
        {sf::Keyboard::Num1, {ROTATION_ANGLE, 0, 0}},
        {sf::Keyboard::Num2, {0, ROTATION_ANGLE, 0}},
        {sf::Keyboard::Num3, {0, 0, ROTATION_ANGLE}}
    }};

    constexpr static std::array<std::pair<sf::Keyboard::Key, glm::vec3>, 6> model_move_keys = {{
        {sf::Keyboard::Numpad1, {MOVE_DISTANCE, 0, 0}},
        {sf::Keyboard::Numpad2, {-MOVE_DISTANCE, 0, 0}},
        {sf::Keyboard::Numpad3, {0, MOVE_DISTANCE, 0}},
        {sf::Keyboard::Numpad4, {0, -MOVE_DISTANCE, 0}},
        {sf::Keyboard::Numpad5, {0, 0, MOVE_DISTANCE}},
        {sf::Keyboard::Numpad6, {0, 0, -MOVE_DISTANCE}}
    }};

    std::ranges::for_each(camera_movement_keys, [&](const auto& element) {
        const auto& [key, direction] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_camera->move(direction * m_delta_time);
        }
    });

    std::ranges::for_each(model_rotation_keys, [&](const auto& element) {
        const auto& [key, rotation] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_scene.rotate_model(rotation * m_delta_time);
        }
    });

    std::ranges::for_each(model_move_keys, [&](const auto& element) {
        const auto& [key, move] = element;
        if (sf::Keyboard::isKeyPressed(key)) {
            m_scene.move_model(move * m_delta_time);
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
