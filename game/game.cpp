#include "game.hpp"

Game::Game(double hexSize, const std::vector<std::string>& asciiMap, int windowWidth, int windowHeight, SDL_Renderer* renderer)
    : grid(hexSize), playerTurn(true), villagerSelected(false) {

    grid.generateFromASCII(asciiMap, windowWidth, windowHeight);

    // Initialize bandit and villager at valid grid positions
    Hex initialBanditHex = grid.getHexes()[0];
    Hex initialVillagerHex = grid.getHexes()[1];
    bandit = Bandit(initialBanditHex);
    villager = Villager(initialVillagerHex);

    // Count the number of unique colors in the grid
    nbplayers = -1;
    std::vector<SDL_Color> uniqueColors;
    for (const auto& pair : grid.getHexColors()) {
        const SDL_Color& color = pair.second;
        bool found = false;
        for (const auto& uniqueColor : uniqueColors) {
            if (color.r == uniqueColor.r && color.g == uniqueColor.g &&
            color.b == uniqueColor.b && color.a == uniqueColor.a) {
            found = true;
            break;
            }
        }
        if (!found) {
            nbplayers++;
            uniqueColors.push_back(color);
        }
    }
    printf("Number of players: %d\n", nbplayers);
    // Load textures
    banditTexture = IMG_LoadTexture(renderer, bandit.getTexturePath().c_str());
    villagerTexture = IMG_LoadTexture(renderer, villager.getTexturePath().c_str());
}

Game::~Game() {
    SDL_DestroyTexture(banditTexture);
    SDL_DestroyTexture(villagerTexture);
}

void Game::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Hex clickedHex = grid.pixelToHex(mouseX, mouseY);

        if (playerTurn) {
            if (!villagerSelected) {
                if (clickedHex == villager.getHex()) {
                    villagerSelected = true;
                }
            } else {
                if (grid.hexExists(clickedHex)) {
                    villager.move(grid, clickedHex);
                    playerTurn = false;
                }
                villagerSelected = false;
            }
        }
    } else if (event.type == SDL_MOUSEMOTION) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        grid.handleMouseMotion(mouseX, mouseY);
    }
}

void Game::update() {
    if (!playerTurn) {
        bandit.move(grid);
        playerTurn = true;
    }
}

void Game::render(SDL_Renderer* renderer) const {
    grid.draw(renderer);

    SDL_Rect banditRect = {
        static_cast<int>(grid.hexToPixel(bandit.getHex()).x - grid.getHexSize() / 2),
        static_cast<int>(grid.hexToPixel(bandit.getHex()).y - grid.getHexSize() / 2),
        static_cast<int>(grid.getHexSize()),
        static_cast<int>(grid.getHexSize())
    };
    SDL_RenderCopy(renderer, banditTexture, NULL, &banditRect);

    SDL_Rect villagerRect = {
        static_cast<int>(grid.hexToPixel(villager.getHex()).x - grid.getHexSize() / 2),
        static_cast<int>(grid.hexToPixel(villager.getHex()).y - grid.getHexSize() / 2),
        static_cast<int>(grid.getHexSize()),
        static_cast<int>(grid.getHexSize())
    };
    SDL_RenderCopy(renderer, villagerTexture, NULL, &villagerRect);
}
