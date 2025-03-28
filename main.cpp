#include "game/game.hpp"

int main() {
    TTF_Init();
    const int windowWidth = 1920;
    const int windowHeight = 1000;
    const double hexSize = 30.0;
    const int cameraSpeed = 10;

    const int FPS = 240;
    const int frameDelay = 1000 / FPS;

    // ASCII map
    std::vector<std::string> asciiMap = {
        ".....................",
        ".....................",
        ".....................",
        ".......r......b......",
        "......rrr....bbb.....",
        ".......r......b......",
        ".....................",
        ".....................",
    };

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
    SDL_Window* window = SDL_CreateWindow("Hexagonal Grid",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          windowWidth, windowHeight,
                                          SDL_WINDOW_SHOWN);

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

    // Create the game instance
    Game game(hexSize, asciiMap, windowWidth, windowHeight, renderer, cameraSpeed);

    Game gamecopy = game;

    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_DELETE) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_e) {
                    game.handleEvent(event);
                    gamecopy = game;
                    continue;
                } else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    game = gamecopy;
                } 
            } else if (event.type == SDL_QUIT) {
                running = false;
            }
            game.handleEvent(event);
        }

        game.update();

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 119, 182, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Render the game
        game.render(renderer);

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
