/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Sat, Mar, 2025                                                     *
 * Title           - B-OOP-400-TLS-4-1-arcade-nolan.dumoulin [WSL: Debian]              *
 * Description     -                                                                    *
 *     Snake                                                                            *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *       ▄▀▀█▄▄▄▄  ▄▀▀▄▀▀▀▄  ▄▀▀█▀▄    ▄▀▀▀█▀▀▄  ▄▀▀█▄▄▄▄  ▄▀▄▄▄▄   ▄▀▀▄ ▄▄             *
 *      ▐  ▄▀   ▐ █   █   █ █   █  █  █    █  ▐ ▐  ▄▀   ▐ █ █    ▌ █  █   ▄▀            *
 *        █▄▄▄▄▄  ▐  █▀▀▀▀  ▐   █  ▐  ▐   █       █▄▄▄▄▄  ▐ █      ▐  █▄▄▄█             *
 *        █    ▌     █          █        █        █    ▌    █         █   █             *
 *       ▄▀▄▄▄▄    ▄▀        ▄▀▀▀▀▀▄   ▄▀        ▄▀▄▄▄▄    ▄▀▄▄▄▄▀   ▄▀  ▄▀             *
 *       █    ▐   █         █       █ █          █    ▐   █     ▐   █   █               *
 *       ▐        ▐         ▐       ▐ ▐          ▐        ▐         ▐   ▐               *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "../interfaces/IDisplayModule.hpp"
#include "Snake.hpp"

namespace arcade {
    Snake::Snake() : AGameModule("Snake"), m_width(40), m_height(30), m_gameOver(false), m_score(0), moveTimer(0), moveDelay(0.2f)
    {
        std::srand(time(nullptr));
    }

    void Snake::stop()
    {
        
    }

    void Snake::init()
    {
        reset();
    }

    void Snake::reset()
    {
        // Reset game state
        m_snake.clear();

        // Create initial snake with 4 segments in the center
        int startX = m_width / 2;
        int startY = m_height / 2;

        for (int i = 0; i < 4; ++i) {
            m_snake.push_back({startX - i, startY});
        }

        // Set initial direction (right)
        m_direction = {1, 0};

        // Generate initial food
        generateFood();
        // Reset score and game over flag
        m_score = 0;
        m_gameOver = false;

        // Reset timer
        moveTimer = 0;
    }

    void Snake::update(float deltaTime)
    {
        if (m_gameOver)
            return;
        
        moveTimer += deltaTime;
        if (moveTimer >= moveDelay) {
            moveSnake();
            moveTimer = 0;
        }
    }

    void Snake::moveSnake()
    {
        Position newHead = {m_snake.front().x + m_direction.x, m_snake.front().y + m_direction.y};

        if (checkCollision(newHead)) {
            m_gameOver = true;
            return;
        }
        m_snake.insert(m_snake.begin(), newHead);
        if (newHead == m_food) {
            m_score +=10;
            generateFood();
        } else {
            m_snake.pop_back();
        }
    }

    void Snake::handleEvent(const InputEvent &event)
    {
        if (event.type != EventType::KeyPressed)
            return;

        switch (event.key.keyCode) {
            case KeyCode::Up:
                // Prevent moving down when going up
                if (m_direction.y != 1) m_direction = {0, -1};
                break;
            case KeyCode::Down:
                // Prevent moving up when going down
                if (m_direction.y != -1) m_direction = {0, 1};
                break;
            case KeyCode::Left:
                // Prevent moving right when going left
                if (m_direction.x != 1) m_direction = {-1, 0};
                break;
            case KeyCode::Right:
                // Prevent moving left when going right
                if (m_direction.x != -1) m_direction = {1, 0};
                break;
            default:
                break;
        }
    }

    void Snake::render(IDisplayModule &displayModule)
    {
        int cellSize = std::min(displayModule.getWidth() / m_width, displayModule.getHeight() / m_height);

        for (size_t i = 0; i < m_snake.size(); ++i) {
            Color color = (i == 0) ? Colors::Green : Colors::White;
            displayModule.drawRect(m_snake[i].x * cellSize, m_snake[i].y * cellSize, cellSize, cellSize, color);
        }
        displayModule.drawRect(m_food.x * cellSize, m_food.y * cellSize, cellSize, cellSize, Colors::Red);
        displayModule.drawText(10, 10, "Score: " + std::to_string(m_score), Colors::White);

        if (m_gameOver) {
            displayModule.drawText(displayModule.getWidth() / 2 - 50, displayModule.getHeight() / 2 - 10, "Game Over", Colors::Red);
            displayModule.drawText(displayModule.getWidth() / 2 - 100, displayModule.getHeight() / 2 + 20, "Press R to restart", Colors::White);
        }
        // if (displayModule.getName() == "NCurses") {
        //     // Use characters instead of rectangles for better terminal visibility
        //     // Implementation details here...
        // } else {
        //     // Normal rendering for graphical displays
        //     // Existing rendering code...
        // }
    }

    void Snake::generateFood()
    {
        while (true) {
            Position newFood = {std::rand() % m_width, std::rand() % m_height};

            if (std::find(m_snake.begin(), m_snake.end(), newFood) == m_snake.end()) {
                m_food = newFood;
                break;
            }
        }
    }

    bool Snake::checkCollision(const Position &pos) const
    {
        if (pos.x < 0 || pos.x >= m_width || pos.y < 0 || pos.y >= m_height)
            return true;
        for (size_t i = 0; i < m_snake.size(); i++) {
            if (m_snake[i] == pos)
                return true;
        }
        return false;
    }

    // Entry point function implementation
    extern "C" {
        IGameModule *createGameModule() {
            return new Snake();
        }
    }
} // namespace arcade