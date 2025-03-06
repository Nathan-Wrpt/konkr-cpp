#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "core/grid.hpp"
#include <vector>
#include <string>

int main() {
    const int windowWidth = 800;
    const int windowHeight = 600;
    const double hexSize = 30.0;

    // ASCII map
    std::vector<std::string> asciiMap = {
        "  . . .",
        " . . . .",
        "  . . .",
        " . . . .",
        "  . . ."
    };

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        SDL_Log("Failed to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Create the SDL window
    SDL_Window* window = SDL_CreateWindow("Hexagonal Grid",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          windowWidth, windowHeight,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create the SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create the hexagonal grid
    HexagonalGrid grid(hexSize);
    grid.generateFromASCII(asciiMap, windowWidth, windowHeight);

    // Load textures
    if (!grid.loadTexture("grass", renderer, "images/grass.png") ||
        !grid.loadTexture("water", renderer, "images/grass.png")) { // Same texture for now
        SDL_Log("Failed to load textures");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Draw the hexagonal grid
        grid.draw(renderer);

        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
