#include "game/game.hpp"
#include <fstream>

// Function to load both maps from a single file
bool loadMapsFromFile(const std::string& filename,
                     std::vector<std::string>& asciiMap,
                     std::vector<std::string>& entityMap) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::string line;
    bool readingAsciiMap = false;
    bool readingEntityMap = false;

    // Clear the maps
    asciiMap.clear();
    entityMap.clear();

    while (std::getline(file, line)) {
        // Check for section markers
        if (line == "map") {
            readingAsciiMap = true;
            readingEntityMap = false;
            continue;
        } else if (line == "entity") {
            readingAsciiMap = false;
            readingEntityMap = true;
            continue;
        }

        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Add line to the appropriate map
        if (readingAsciiMap) {
            asciiMap.push_back(line);
        } else if (readingEntityMap) {
            entityMap.push_back(line);
        }
    }

    file.close();

    // Verify that both maps were loaded and have the same dimensions
    if (asciiMap.empty() || entityMap.empty()) {
        std::cerr << "Error: One or both maps are empty in file " << filename << std::endl;
        return false;
    }

    if (asciiMap.size() != entityMap.size()) {
        std::cerr << "Error: Maps have different number of rows in file " << filename << std::endl;
        return false;
    }

    for (size_t i = 0; i < asciiMap.size(); ++i) {
        if (asciiMap[i].size() != entityMap[i].size()) {
            std::cerr << "Error: Maps have different row lengths at row " << i << " in file " << filename << std::endl;
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[]) {
    TTF_Init();
    const double hexSize = 30.0;
    const int cameraSpeed = 20;

    const int FPS = 240;
    const int frameDelay = 1000 / FPS;

    // default ASCII and entity maps
    std::vector<std::string> asciiMap;

    std::vector<std::string> entityMap;

    // Check if a map file is provided as a command-line argument
    std::string defaultMapFile = "maps/1v1_close";
    if (argc >= 2) {
        std::string mapFile = argv[1];
        if (loadMapsFromFile(mapFile, asciiMap, entityMap)) {
            std::cout << "Successfully loaded map from " << mapFile << std::endl;
        } else {
            std::cout << "Couldn't load map, using default one" << std::endl;
            if (loadMapsFromFile(defaultMapFile, asciiMap, entityMap)) {
                std::cout << "Successfully loaded map from " << defaultMapFile << std::endl;
            } else {
                std::cerr << "Error: Could not load default map file " << defaultMapFile << std::endl;
                return 1;
            }
        }
    } else {
        std::cout << "No map file specified. Using default map." << std::endl;
        if (!loadMapsFromFile(defaultMapFile, asciiMap, entityMap)) {
            std::cerr << "Error: Could not load default map file " << defaultMapFile << std::endl;
            return 1;
        }
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        SDL_Log("SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
        return 1;
    }

    // Create the SDL window
    SDL_Window* window = SDL_CreateWindow("C++ Konkr by Nathan W & Antoine B",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          0, 0,
                                          SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create the SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Get the window size for game initialization
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Create the game instance
    Game game(hexSize, asciiMap, entityMap, windowWidth, windowHeight, renderer, cameraSpeed);

    Game gamecopy = game;
    Game gameinit = game;

    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        if (game.getTurnButtonClicked()) {
            gamecopy = game;
            game.setTurnButtonClicked(false);
        }
        if (game.getUndo()) {
            game = gamecopy;
            game.setUndo(false);
        }
        if (game.getReplayButtonClicked()) {
            game = gameinit;
            gamecopy = gameinit;
            game.setReplayButtonClicked(false);
            game.setEndGame(false);
        }
        Uint32 frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_DELETE) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_e) {
                    game.handleEvent(event);
                    gamecopy = game;
                    continue;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_r) {
                    game = gamecopy;
                }
            } else if (event.type == SDL_QUIT) {
                running = false;
            }
            game.handleEvent(event);
            if(game.getEndGame()) {
                running = false;
            }
        }

        game.update();

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 119, 182, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Render the game
        game.renderAll(renderer);

        // Present the rendered frame
        SDL_RenderPresent(renderer);

        // Cap the frame rate
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}