/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Sat, Mar, 2025                                                     *
 * Title           - B-OOP-400-TLS-4-1-arcade-nolan.dumoulin [WSL: Debian]              *
 * Description     -                                                                    *
 *     Minesweeper                                                                      *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *             ███████╗██████╗ ██╗████████╗███████╗ ██████╗██╗  ██╗                     *
 *             ██╔════╝██╔══██╗██║╚══██╔══╝██╔════╝██╔════╝██║  ██║                     *
 *             █████╗  ██████╔╝██║   ██║   █████╗  ██║     ███████║                     *
 *             ██╔══╝  ██╔═══╝ ██║   ██║   ██╔══╝  ██║     ██╔══██║                     *
 *             ███████╗██║     ██║   ██║   ███████╗╚██████╗██║  ██║                     *
 *             ╚══════╝╚═╝     ╚═╝   ╚═╝   ╚══════╝ ╚═════╝╚═╝  ╚═╝                     *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#include "Minesweeper.hpp"
#include "../interfaces/IDisplayModule.hpp"
#include <algorithm>
#include <string>

namespace arcade {

    Minesweeper::Minesweeper() : AGameModule("Minesweeper"), gridWidth(BEGINNER_WIDTH), gridHeight(BEGINNER_HEIGHT), mineCount(BEGINNER_MINES), revealedCount(0), flagCount(0), firstClick(true), won(false), timer(0)
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        initGrid();
    }

    Minesweeper::~Minesweeper()
    {
        stop();
    }

    void Minesweeper::init()
    {
        reset();
    }

    void Minesweeper::stop()
    {
    }

    void Minesweeper::reset()
    {
        // Reset game state
        revealedCount = 0;
        flagCount = 0;
        firstClick = true;
        won = false;
        timer = 0;
        score = 0;
        gameOver = false;
        
        initGrid();
    }

    void Minesweeper::initGrid()
    {
        // Create grid
        grid.resize(gridHeight);
        for (int y = 0; y < gridHeight; ++y) {
            grid[y].resize(gridWidth);
            for (int x = 0; x < gridWidth; ++x) {
                grid[y][x] = Cell();
            }
        }
    }

    void Minesweeper::placeMines(int firstClickX, int firstClickY)
    {
        // Place mines randomly
        int minesPlaced = 0;
        while (minesPlaced < mineCount) {
            int x = rand() % gridWidth;
            int y = rand() % gridHeight;
            
            // Avoid placing a mine at the first click position and adjacent cells
            if (abs(x - firstClickX) <= 1 && abs(y - firstClickY) <= 1) {
                continue;
            }
            
            // Avoid placing a mine on an existing mine
            if (!grid[y][x].isMine) {
                grid[y][x].isMine = true;
                minesPlaced++;
            }
        }
        
        // Calculate adjacent mines for each cell
        calculateAdjacentMines();
    }

    void Minesweeper::calculateAdjacentMines()
    {
        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                if (grid[y][x].isMine) {
                    continue;
                }
                
                int count = 0;
                
                // Check all adjacent cells
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = x + dx;
                        int ny = y + dy;
                        
                        // Skip out of bounds cells
                        if (nx < 0 || nx >= gridWidth || ny < 0 || ny >= gridHeight) {
                            continue;
                        }
                        
                        // Count mines
                        if (grid[ny][nx].isMine) {
                            count++;
                        }
                    }
                }
                
                grid[y][x].adjacentMines = count;
            }
        }
    }

    void Minesweeper::revealCell(int x, int y)
    {
        // Skip out of bounds cells
        if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight) {
            return;
        }
        
        // Skip already revealed or flagged cells
        if (grid[y][x].state != CellState::Hidden) {
            return;
        }
        
        // Handle first click
        if (firstClick) {
            firstClick = false;
            placeMines(x, y);
        }
        
        // Reveal the cell
        grid[y][x].state = CellState::Revealed;
        revealedCount++;
        
        // Update score
        score += 10;
        
        // Check if the player revealed a mine
        if (grid[y][x].isMine) {
            // Game over
            gameOver = true;
            
            // Reveal all mines
            for (int cy = 0; cy < gridHeight; ++cy) {
                for (int cx = 0; cx < gridWidth; ++cx) {
                    if (grid[cy][cx].isMine && grid[cy][cx].state == CellState::Hidden) {
                        grid[cy][cx].state = CellState::Revealed;
                    }
                }
            }
            return;
        }
        
        // If the cell has no adjacent mines, reveal adjacent cells
        if (grid[y][x].adjacentMines == 0) {
            revealEmptyCells(x, y);
        }
        
        // Check for win condition
        if (checkWin()) {
            won = true;
            gameOver = true;
            
            // Add bonus points for completing the game
            score += 500 + static_cast<int>(1000 / timer);
        }
    }

    void Minesweeper::revealEmptyCells(int x, int y)
    {
        // Reveal adjacent cells recursively for empty cells
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int nx = x + dx;
                int ny = y + dy;
                
                // Skip out of bounds cells
                if (nx < 0 || nx >= gridWidth || ny < 0 || ny >= gridHeight) {
                    continue;
                }
                
                // Skip already revealed cells
                if (grid[ny][nx].state == CellState::Revealed) {
                    continue;
                }
                
                // Reveal the cell
                grid[ny][nx].state = CellState::Revealed;
                revealedCount++;
                
                // Update score
                score += 5;
                
                // If the cell has no adjacent mines, reveal adjacent cells
                if (grid[ny][nx].adjacentMines == 0) {
                    revealEmptyCells(nx, ny);
                }
            }
        }
    }

    void Minesweeper::flagCell(int x, int y)
    {
        // Skip out of bounds cells
        if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight) {
            return;
        }
        
        // Skip revealed cells
        if (grid[y][x].state == CellState::Revealed) {
            return;
        }
        
        // Toggle flag state
        if (grid[y][x].state == CellState::Hidden) {
            grid[y][x].state = CellState::Flagged;
            flagCount++;
        } else if (grid[y][x].state == CellState::Flagged) {
            grid[y][x].state = CellState::Hidden;
            flagCount--;
        }
    }

    bool Minesweeper::checkWin()
    {
        // Player wins when all non-mine cells are revealed
        return revealedCount == (gridWidth * gridHeight - mineCount);
    }

    int Minesweeper::getCellSize(const IDisplayModule &display) const
    {
        int maxCellWidth = display.getWidth() / gridWidth;
        int maxCellHeight = display.getHeight() / gridHeight;
        return std::min(maxCellWidth, maxCellHeight);
    }

    bool Minesweeper::getGridCoords(const IDisplayModule &display, int displayX, int displayY, int &gridX, int &gridY) const
    {
        int cellSize = getCellSize(display);
        
        // Calculate grid coordinates
        gridX = displayX / cellSize;
        gridY = displayY / cellSize;
        
        // Check if coordinates are within bounds
        return (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight);
    }

    void Minesweeper::update(float deltaTime)
    {
        if (!gameOver && !firstClick) {
            timer += deltaTime;
        }
    }

    void Minesweeper::render(IDisplayModule &displayModule)
    {
        this->displayModule = &displayModule;
        // Calculate cell size
        int cellSize = getCellSize(displayModule);
        
        // Calculate grid offset to center it
        int offsetX = (displayModule.getWidth() - (gridWidth * cellSize)) / 2;
        int offsetY = (displayModule.getHeight() - (gridHeight * cellSize)) / 2;
        
        // Draw grid
        for (int y = 0; y < gridHeight; ++y) {
            for (int x = 0; x < gridWidth; ++x) {
                int drawX = offsetX + x * cellSize;
                int drawY = offsetY + y * cellSize;
                
                // Draw cell background
                Color bgColor;
                
                switch (grid[y][x].state) {
                    case CellState::Hidden:
                        bgColor = Color(150, 150, 150); // Grey for hidden cells
                        break;
                    case CellState::Flagged:
                        bgColor = Color(255, 165, 0); // Orange for flagged cells
                        break;
                    case CellState::Revealed:
                        if (grid[y][x].isMine) {
                            bgColor = Color(255, 0, 0); // Red for mines
                        } else {
                            bgColor = Color(220, 220, 220); // Light grey for revealed cells
                        }
                        break;
                }
                
                displayModule.drawRect(drawX, drawY, cellSize, cellSize, bgColor);
                
                // Draw cell content
                if (grid[y][x].state == CellState::Revealed) {
                    if (grid[y][x].isMine) {
                        // Draw mine
                        displayModule.drawCircle(drawX + cellSize / 2, drawY + cellSize / 2, cellSize / 4, Colors::Black);
                    } else if (grid[y][x].adjacentMines > 0) {
                        // Draw number
                        std::string text = std::to_string(grid[y][x].adjacentMines);
                        
                        // Choose color based on number
                        Color textColor;
                        switch (grid[y][x].adjacentMines) {
                            case 1: textColor = Color(0, 0, 255); break;     // Blue
                            case 2: textColor = Color(0, 128, 0); break;     // Green
                            case 3: textColor = Color(255, 0, 0); break;     // Red
                            case 4: textColor = Color(128, 0, 128); break;   // Purple
                            case 5: textColor = Color(128, 0, 0); break;     // Maroon
                            case 6: textColor = Color(0, 128, 128); break;   // Teal
                            case 7: textColor = Color(0, 0, 0); break;       // Black
                            case 8: textColor = Color(128, 128, 128); break; // Grey
                            default: textColor = Colors::Black; break;
                        }
                        
                        displayModule.drawText(drawX + cellSize / 3, drawY + cellSize / 4, text, textColor);
                    }
                } else if (grid[y][x].state == CellState::Flagged) {
                    // Draw flag
                    displayModule.drawRect(drawX + cellSize / 3, drawY + cellSize / 4, cellSize / 10, cellSize / 2, Colors::Black);
                    displayModule.drawRect(drawX + cellSize / 3, drawY + cellSize / 4, cellSize / 3, cellSize / 4, Colors::Red);
                }
                
                // Draw cell border
                displayModule.drawRect(drawX, drawY, cellSize, 1, Colors::Black); // Top
                displayModule.drawRect(drawX, drawY, 1, cellSize, Colors::Black); // Left
                displayModule.drawRect(drawX + cellSize - 1, drawY, 1, cellSize, Colors::Black); // Right
                displayModule.drawRect(drawX, drawY + cellSize - 1, cellSize, 1, Colors::Black); // Bottom
            }
        }
        
        // Draw game info
        std::string timeText = "Time: " + std::to_string(static_cast<int>(timer));
        std::string minesText = "Mines: " + std::to_string(mineCount - flagCount);
        std::string scoreText = "Score: " + std::to_string(score);
        
        displayModule.drawText(10, 10, timeText, Colors::White);
        displayModule.drawText(10, 30, minesText, Colors::White);
        displayModule.drawText(10, 50, scoreText, Colors::White);
        
        // Draw game status
        if (gameOver) {
            std::string statusText = won ? "You Win!" : "Game Over!";
            displayModule.drawText(displayModule.getWidth() / 2 - 50, 20, statusText, won ? Colors::Green : Colors::Red);
            displayModule.drawText(displayModule.getWidth() / 2 - 100, 40, "Press R to restart", Colors::White);
        }
    }

    void Minesweeper::handleEvent(const InputEvent &event)
    {
        if (gameOver) {
            return;
        }
        
        if (event.type == EventType::MouseButtonPressed) {
            // Calculate grid coordinates from mouse position
            int gridX, gridY;
            if (getGridCoords(*displayModule, event.mouseButton.x, event.mouseButton.y, gridX, gridY)) {
                if (event.mouseButton.button == 0) {  // Left click
                    revealCell(gridX, gridY);
                } else if (event.mouseButton.button == 1) {  // Right click
                    flagCell(gridX, gridY);
                }
            }
        } else if (event.type == EventType::KeyPressed) {
            // Handle key press events for testing
            switch (event.key.keyCode) {
                case KeyCode::Space:
                    // Reveal a random cell for testing
                    if (firstClick) {
                        revealCell(gridWidth / 2, gridHeight / 2); // First click in the middle
                    } else {
                        int x = rand() % gridWidth;
                        int y = rand() % gridHeight;
                        if (grid[y][x].state == CellState::Hidden) {
                            revealCell(x, y);
                        }
                    }
                    break;
                case KeyCode::F:
                    // Flag a random cell for testing
                    {
                        int x = rand() % gridWidth;
                        int y = rand() % gridHeight;
                        if (grid[y][x].state != CellState::Revealed) {
                            flagCell(x, y);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // Entry point function implementation
    extern "C" {
        IGameModule *createGameModule() {
            return new Minesweeper();
        }
    }
}