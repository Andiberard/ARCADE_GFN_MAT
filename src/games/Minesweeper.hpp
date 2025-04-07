/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Sat, Mar, 2025                                                     *
 * Title           - B-OOP-400-TLS-4-1-arcade-nolan.dumoulin [WSL: Debian]              *
 * Description     -                                                                    *
 *     Minesweeper                                                                      *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *         ░        ░       ░░        ░        ░        ░░      ░░  ░░░░  ░             *
 *         ▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒  ▒▒▒▒  ▒             *
 *         ▓      ▓▓▓       ▓▓▓▓▓  ▓▓▓▓▓▓▓  ▓▓▓▓      ▓▓▓  ▓▓▓▓▓▓▓        ▓             *
 *         █  ███████  ██████████  ███████  ████  ███████  ████  █  ████  █             *
 *         █        █  ███████        ████  ████        ██      ██  ████  █             *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

// src/games/Minesweeper.hpp
#pragma once

#include "../interfaces/AGameModule.hpp"
#include <vector>
#include <ctime>
#include <cstdlib>

namespace arcade {
    enum class CellState {
        Hidden,
        Revealed,
        Flagged
    };

    struct Cell {
        bool isMine;
        CellState state;
        int adjacentMines;
        Cell() : isMine(false), state(CellState::Hidden), adjacentMines(0) {}
    };

    class Minesweeper : public AGameModule {
    private:
        static constexpr int BEGINNER_WIDTH = 9;
        static constexpr int BEGINNER_HEIGHT = 9;
        static constexpr int BEGINNER_MINES = 10;

        IDisplayModule *displayModule;
        
        int gridWidth;
        int gridHeight;
        int mineCount;
        int revealedCount;
        int flagCount;
        bool firstClick;
        bool won;
        float timer;
        
        std::vector<std::vector<Cell>> grid;
        
        // Initialize the game grid
        void initGrid();
        
        // Place mines on the grid (avoiding firstClickPos)
        void placeMines(int firstClickX, int firstClickY);
        
        // Calculate adjacent mines for each cell
        void calculateAdjacentMines();
        
        // Reveal a cell at the given position
        void revealCell(int x, int y);
        
        // Recursively reveal cells with no adjacent mines
        void revealEmptyCells(int x, int y);
        
        // Flag a cell at the given position
        void flagCell(int x, int y);
        
        // Check if the player has won
        bool checkWin();
        
        // Get cell size based on display dimensions
        int getCellSize(const IDisplayModule &display) const;
        
    public:
        Minesweeper();
        ~Minesweeper() override;
        void init() override;
        void stop() override;
        void update(float deltaTime) override;
        void render(IDisplayModule &displayModule) override;
        void handleEvent(const InputEvent &event) override;
        void reset() override;
        // Convert display coordinates to grid coordinates
        bool getGridCoords(const IDisplayModule &display, int displayX, int displayY, int &gridX, int &gridY) const;
    };

    // Entry point function
    extern "C" {
        IGameModule *createGameModule();
    }
}

/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * MIT License                                                                          *
 * Copyright (c) 2025 Anonymous                                                         *
 *                                                                                      *
 * Permission is hereby granted, free of charge, to any person obtaining a copy         *
 * of this software and associated documentation files (the "Software"), to deal        *
 * in the Software without restriction, including without limitation the rights         *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            *
 * copies of the Software, and to permit persons to whom the Software is                *
 * furnished to do so, subject to the following conditions:                             *
 *                                                                                      *
 * The above copyright notice and this permission notice shall be included in all       *
 * copies or substantial portions of the Software.                                      *
 *                                                                                      *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        *
 * SOFTWARE.                                                                            *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */
