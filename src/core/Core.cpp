/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Tue, Mar, 2025                                                     *
 * Title           - B-OOP-400-TLS-4-1-arcade-nolan.dumoulin [WSL: Debian]              *
 * Description     -                                                                    *
 *     Core                                                                             *
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

#include "Core.hpp"
#include <sstream>

namespace arcade {

    Core::Core(const std::string &initialDisplayLib) : currentDisplayIndex(0), currentGameIndex(0), running(false), inMenu(true), initialDisplayLib(initialDisplayLib) {}

    void Core::init()
    {
        scanLibraries();
        
        if (displayLibraries.empty() || gameLibraries.empty()) {
            throw LibraryLoadException("No display or game libraries found");
        }
        
        std::cerr << "Display libraries found: " << displayLibraries.size() << std::endl;
        for (const auto& lib : displayLibraries) {
            std::cerr << "  - " << lib << std::endl;
        }
        
        std::cerr << "Game libraries found: " << gameLibraries.size() << std::endl;
        for (const auto& lib : gameLibraries) {
            std::cerr << "  - " << lib << std::endl;
        }

        if (!initialDisplayLib.empty()) {
            std::cerr << "Initial display library specified: " << initialDisplayLib << std::endl;
            bool found = false;
            for (size_t i = 0; i < displayLibraries.size(); ++i) {
                std::filesystem::path displayPath(displayLibraries[i]);
                std::filesystem::path initialPath(initialDisplayLib);
                
                std::cerr << "Comparing " << displayPath.filename() << " with " << initialPath.filename() << std::endl;
                
                if (displayPath.filename() == initialPath.filename()) {
                    currentDisplayIndex = i;
                    found = true;
                    std::cerr << "Found matching library at index " << i << std::endl;
                    break;
                }
            }
            
            if (!found) {
                throw LibraryLoadException("Specified display library not found in lib directory");
            }
        }
        
        std::cerr << "Attempting to load display module: " << displayLibraries[currentDisplayIndex] << std::endl;
        if (!loadDisplayModule(displayLibraries[currentDisplayIndex])) {
            throw LibraryLoadException("Failed to load display module: " + displayLibraries[currentDisplayIndex]);
        }
        
        if (!displayModule) {
            throw LibraryLoadException("Display module is null after loading");
        }
        
        std::cerr << "Display module loaded successfully: " << displayModule->getName() << std::endl;
        
        loadScores();
        
        inMenu = true;
        running = true;
    }

    void Core::run()
    {
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        if (!displayModule) {
            std::cerr << "Error: Display module is null before entering main loop" << std::endl;
            return;
        }
        
        try {
            bool isModuleOpen = displayModule->isOpen();
            std::cerr << "Display module isOpen() check before loop: " << (isModuleOpen ? "true" : "false") << std::endl;
            
            if (!isModuleOpen) {
                std::cerr << "Warning: Display module reports as not open, initializing again..." << std::endl;
                displayModule->init();
                isModuleOpen = displayModule->isOpen();
                std::cerr << "Display module isOpen() after re-init: " << (isModuleOpen ? "true" : "false") << std::endl;
            }
            
            while (running && displayModule && displayModule->isOpen()) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
                lastTime = currentTime;
                
                processEvents();
                update(deltaTime);
                render();
                
                int frameDelay = 16; // ~60 FPS
                std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
            }
            
            std::cerr << "Exited main loop: running=" << (running ? "true" : "false")
                    << ", displayModule=" << (displayModule ? "valid" : "null");
                    
            if (displayModule) {
                std::cerr << ", isOpen=" << (displayModule->isOpen() ? "true" : "false");
            }
            
            std::cerr << std::endl;
        } 
        catch (const std::exception& e) {
            std::cerr << "Exception in main loop: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "Unknown exception in main loop" << std::endl;
        }
    }

    void Core::stop()
    {
        saveScores();
        if (gameModule) {
            gameModule->stop();
            gameModule.reset();
        }
        
        if (displayModule) {
            displayModule->stop();
            displayModule.reset();
        }
        
        running = false;
    }

    void Core::scanLibraries()
    {
        displayLibraries.clear();
        gameLibraries.clear();
        
        try {
            std::vector<LibraryInfo> libraries = LibraryScanner::scanLibraries();
            
            for (const auto &library : libraries) {
                if (library.type == LibraryInfo::Type::Display) {
                    displayLibraries.push_back(library.path);
                } else if (library.type == LibraryInfo::Type::Game) {
                    gameLibraries.push_back(library.path);
                }
            }
        } catch (const LibraryLoadException &e) {
            std::cerr << "Warning during library scanning: " << e.what() << std::endl;
        }
    }

    bool Core::loadDisplayModule(const std::string &libraryPath)
    {
        try {
            if (displayModule) {
                displayModule->stop();
                displayModule.reset();
            }
            
            if (!std::filesystem::exists(libraryPath)) {
                throw LibraryLoadException("Display library file not found: " + libraryPath);
            }
            
            DynamicLoader<IDisplayModule> loader(libraryPath);
            auto newModule = loader.getInstance("createDisplayModule");
            
            if (!newModule) {
                throw LibraryLoadException("Failed to create display module instance");
            }
            
            displayModule.reset(newModule);
            
            displayModule->init();
            
            return true;
        } catch (const LibraryLoadException &e) {
            std::cerr << "Error loading display module: " << e.what() << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Unexpected error loading display module: " << e.what() << std::endl;
            return false;
        }
    }

    bool Core::loadGameModule(const std::string &libraryPath)
    {
        try {
            if (gameModule) {
                gameModule->stop();
                gameModule.reset();
            }
            
            if (!std::filesystem::exists(libraryPath)) {
                throw LibraryLoadException("Game library file not found: " + libraryPath);
            }
            
            DynamicLoader<IGameModule> loader(libraryPath);
            auto newModule = loader.getInstance("createGameModule");
            
            if (!newModule) {
                throw LibraryLoadException("Failed to create game module instance");
            }
            
            gameModule.reset(newModule);
            
            gameModule->init();
            
            return true;
        } catch (const LibraryLoadException &e) {
            std::cerr << "Error loading game module: " << e.what() << std::endl;
            return false;
        } catch (const std::exception &e) {
            std::cerr << "Unexpected error loading game module: " << e.what() << std::endl;
            return false;
        }
    }

    void Core::processEvents()
    {
        InputEvent event;
        
        while (displayModule && displayModule->pollEvent(event)) {
            if (event.type == EventType::KeyPressed) {
                if (inMenu) {
                    if (event.key.keyCode >= KeyCode::A && event.key.keyCode <= KeyCode::Z) {
                        char c = 'a' + (static_cast<int>(event.key.keyCode) - static_cast<int>(KeyCode::A));
                        if (playerName.length() < 20) {
                            playerName += c;
                        }
                    } 
                    else if (event.key.keyCode >= KeyCode::Num0 && event.key.keyCode <= KeyCode::Num9) {
                        char c = '0' + (static_cast<int>(event.key.keyCode) - static_cast<int>(KeyCode::Num0));
                        if (playerName.length() < 20) {
                            playerName += c;
                        }
                    }
                    else if (event.key.keyCode == KeyCode::Space) {
                        if (playerName.length() < 20 && !playerName.empty()) {
                            playerName += ' ';
                        }
                    }
                    else if (event.key.keyCode == KeyCode::Enter) {
                        if (!playerName.empty() && !gameLibraries.empty()) {
                            inMenu = false;
                            loadGameModule(gameLibraries[currentGameIndex]);
                        }
                    }
                }
                
                switch (event.key.keyCode) {
                    case KeyCode::Escape:
                        if (!inMenu) {
                            returnToMenu();
                        } else {
                            running = false;
                        }
                        break;
                        
                    case KeyCode::Tab:
                        nextDisplayModule();
                        break;
                        
                    case KeyCode::M:
                        nextGameModule();
                        break;
                        
                    case KeyCode::R:
                        restartGame();
                        break;
                        
                    case KeyCode::Backspace:
                        if (inMenu && !playerName.empty()) {
                            playerName.pop_back();
                        }
                        break;
                        
                    default:
                        if (!inMenu && gameModule) {
                            gameModule->handleEvent(event);
                        }
                        break;
                }
            } 
            else if (event.type == EventType::MouseButtonPressed) {
                if (inMenu) {
                    int width = displayModule->getWidth();
                    int height = displayModule->getHeight();
                    
                    if (event.mouseButton.x >= 70 && event.mouseButton.x <= width / 2 &&
                        event.mouseButton.y >= 140 && event.mouseButton.y < 140 + static_cast<int>(displayLibraries.size() * 30)) {
                        
                        int index = (event.mouseButton.y - 140) / 30;
                        if (index >= 0 && index < static_cast<int>(displayLibraries.size())) {
                            currentDisplayIndex = index;
                            loadDisplayModule(displayLibraries[currentDisplayIndex]);
                        }
                    }
                    
                    if (event.mouseButton.x >= width - 230 && event.mouseButton.x <= width &&
                        event.mouseButton.y >= 140 && event.mouseButton.y < 140 + static_cast<int>(gameLibraries.size() * 30)) {
                        
                        int index = (event.mouseButton.y - 140) / 30;
                        if (index >= 0 && index < static_cast<int>(gameLibraries.size())) {
                            currentGameIndex = index;
                        }
                    }
                    
                    int nameInputY = 350;
                    if (highScores.size() > 0) {
                        nameInputY = 270 + std::min(5, static_cast<int>(highScores.size())) * 25;
                    }
                    
                    if (event.mouseButton.x >= width / 2 - 100 && event.mouseButton.x <= width / 2 + 150 &&
                        event.mouseButton.y >= nameInputY && event.mouseButton.y <= nameInputY + 40) {
                    }
                    
                    if (!playerName.empty() &&
                        event.mouseButton.x >= width / 2 - 150 && event.mouseButton.x <= width / 2 + 150 &&
                        event.mouseButton.y >= height - 70 && event.mouseButton.y <= height - 45) {
                        
                        inMenu = false;
                        loadGameModule(gameLibraries[currentGameIndex]);
                    }
                }
                else if (gameModule) {
                    gameModule->handleEvent(event);
                }
            }
            else if (event.type == EventType::WindowClosed) {
                running = false;
            }
            else if (!inMenu && gameModule) {
                gameModule->handleEvent(event);
            }
        }
    }

    void Core::update(float deltaTime)
    {
        if (!inMenu && gameModule) {
            gameModule->update(deltaTime);
            
            if (gameModule->isGameOver()) {
                if (!playerName.empty()) {
                    PlayerScore newScore;
                    newScore.playerName = playerName;
                    newScore.gameName = LibraryScanner::getLibraryName(gameLibraries[currentGameIndex]);
                    newScore.score = gameModule->getScore();
                    
                    highScores.push_back(newScore);
                    
                    std::sort(highScores.begin(), highScores.end(), 
                            [](const PlayerScore &a, const PlayerScore &b) {
                                return a.score > b.score;
                            });
                    
                    if (highScores.size() > 10) {
                        highScores.resize(10);
                    }
                    
                    saveScores();
                }
            }
        }
    }

    void Core::render()
    {
        if (displayModule) {
            displayModule->clear();
            if (inMenu) {
                renderMenu();
            } else if (gameModule) {
                gameModule->render(*displayModule);
            }
            displayModule->display();
        }
    }

    void Core::renderMenu()
    {
        if (!displayModule) return;
        
        int width = displayModule->getWidth();
        int height = displayModule->getHeight();

        bool isNcurses = (displayModule->getName() == "NCurses");
        
        int titleX = isNcurses ? width / 2 - 3 : width / 2 - 50;
        int titleY = isNcurses ? 1 : 50;
        displayModule->drawText(titleX, titleY, "ARCADE", Colors::Red);
        
        int leftHeaderX = isNcurses ? 2 : 50;
        int rightHeaderX = isNcurses ? width / 2 + 5 : width - 250;
        int headerY = isNcurses ? 3 : 100;
        
        displayModule->drawText(leftHeaderX, headerY, "Display Libraries:", Colors::Blue);
        
        displayModule->drawText(rightHeaderX, headerY, "Game Libraries:", Colors::Blue);
        
        int leftItemsX = isNcurses ? 4 : 70;
        int rightItemsX = isNcurses ? width / 2 + 7 : width - 230;
        int itemsStartY = isNcurses ? 4 : 140;
        int itemSpacing = isNcurses ? 1 : 30;
        
        for (size_t i = 0; i < displayLibraries.size(); ++i) {
            std::string name = LibraryScanner::getLibraryName(displayLibraries[i]);
            Color color = (i == currentDisplayIndex) ? Colors::Green : Colors::White;
            displayModule->drawText(leftItemsX, itemsStartY + i * itemSpacing, name, color);
        }
        
        for (size_t i = 0; i < gameLibraries.size(); ++i) {
            std::string name = LibraryScanner::getLibraryName(gameLibraries[i]);
            Color color = (i == currentGameIndex) ? Colors::Green : Colors::White;
            displayModule->drawText(rightItemsX, itemsStartY + i * itemSpacing, name, color);
        }
        
        int scoresHeaderX = isNcurses ? width / 2 - 5 : width / 2 - 50;
        int scoresHeaderY = isNcurses ? 6 + displayLibraries.size() : 200;
        displayModule->drawText(scoresHeaderX, scoresHeaderY, "High Scores:", Colors::Yellow);

        if (!highScores.empty()) {
            std::vector<PlayerScore> sortedScores = highScores;
            std::sort(sortedScores.begin(), sortedScores.end(), 
                    [](const PlayerScore &a, const PlayerScore &b) {
                        return a.score > b.score;
                    });

            int scoreCount = std::min(5, static_cast<int>(sortedScores.size()));
            int scoresX = isNcurses ? width / 2 - 15 : width / 2 - 150;
            int scoresStartY = isNcurses ? 7 + displayLibraries.size() : 230;
            int scoreSpacing = isNcurses ? 1 : 25;
            
            for (int i = 0; i < scoreCount; ++i) {
                std::string scoreText = sortedScores[i].playerName + " - " + 
                                        sortedScores[i].gameName + " - " + 
                                        std::to_string(sortedScores[i].score);
                displayModule->drawText(scoresX, scoresStartY + i * scoreSpacing, scoreText, Colors::White);
            }
        }
        
        int nameInputBaseY = std::min(5, static_cast<int>(highScores.size()));
        int nameInputY = isNcurses ? 13 + displayLibraries.size() + nameInputBaseY : 350 + nameInputBaseY * 25;
        int namePromptX = isNcurses ? width / 2 - 5 : width / 2 - 100;
        
        displayModule->drawText(namePromptX, nameInputY, "Your Name:", Colors::Yellow);
        
        int nameInputFieldY = isNcurses ? nameInputY + 1 : nameInputY + 30;
        int nameInputFieldX = isNcurses ? namePromptX : width / 2 - 95;
        
        if (!isNcurses) {
            int nameWidth = playerName.length() * 10 + 20;
            displayModule->drawRect(width / 2 - 100, nameInputY + 25, nameWidth, 25, Color(30, 30, 30));
        }
        
        displayModule->drawText(nameInputFieldX, nameInputFieldY, playerName + "_", Colors::White);
        
        int instructionsX = isNcurses ? 2 : width / 2 - 150;
        int instructionsStartY = isNcurses ? height - 5 : height - 120;
        int instructionSpacing = isNcurses ? 1 : 25;
        
        displayModule->drawText(instructionsX, instructionsStartY, "Press Tab to change graphics library", Colors::Yellow);
        displayModule->drawText(instructionsX, instructionsStartY + instructionSpacing, "Press M to change game library", Colors::Yellow);
        displayModule->drawText(instructionsX, instructionsStartY + 2 * instructionSpacing, "Press Enter to start game", Colors::Yellow);
        displayModule->drawText(instructionsX, instructionsStartY + 3 * instructionSpacing, "Press Escape to exit", Colors::Yellow);
    }

    void Core::nextDisplayModule()
    {
        if (displayLibraries.empty()) {
            return;
        }
        
        currentDisplayIndex = (currentDisplayIndex + 1) % displayLibraries.size();
        loadDisplayModule(displayLibraries[currentDisplayIndex]);
    }

    void Core::nextGameModule()
    {
        if (gameLibraries.empty()) {
            return;
        }
        
        currentGameIndex = (currentGameIndex + 1) % gameLibraries.size();
        
        if (!inMenu) {
            loadGameModule(gameLibraries[currentGameIndex]);
        }
    }

    void Core::restartGame()
    {
        if (!inMenu && gameModule) {
            gameModule->reset();
        }
    }

    void Core::returnToMenu()
    {
        if (gameModule) {
            gameModule->stop();
            gameModule.reset();
        }
        
        inMenu = true;
    }
    
    void Core::loadScores()
    {
        highScores.clear();
        std::ifstream file("scores.txt");
        if (!file.is_open()) {
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            PlayerScore score;
            
            iss >> score.playerName;
            
            std::replace(score.playerName.begin(), score.playerName.end(), '_', ' ');
            
            iss >> score.gameName;
            
            iss >> score.score;
            
            if (score.score > 0) {
                bool isDuplicate = false;
                for (const auto& existingScore : highScores) {
                    if (existingScore.playerName == score.playerName && 
                        existingScore.gameName == score.gameName && 
                        existingScore.score == score.score) {
                        isDuplicate = true;
                        break;
                    }
                }
                
                if (!isDuplicate) {
                    highScores.push_back(score);
                }
            }
        }
        file.close();
    }

    void Core::saveScores()
    {
        std::ofstream file("scores.txt");
        if (!file.is_open()) {
            std::cerr << "Failed to save scores" << std::endl;
            return;
        }
        
        std::vector<PlayerScore> sortedScores = highScores;
        std::sort(sortedScores.begin(), sortedScores.end(), 
                [](const PlayerScore &a, const PlayerScore &b) {
                    return a.score > b.score;
                });
        
        int scoreCount = std::min(10, static_cast<int>(sortedScores.size()));
        
        for (int i = 0; i < scoreCount; i++) {
            const auto &score = sortedScores[i];
            std::string safePlayerName = score.playerName;
            std::replace(safePlayerName.begin(), safePlayerName.end(), ' ', '_');
            file << safePlayerName << " " << score.gameName << " " << score.score << std::endl;
        }
        file.close();
    }
}