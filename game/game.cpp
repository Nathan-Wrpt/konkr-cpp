#include "game.hpp"
#include <random> // Required for random number generation
#include <map>

// Custom comparison function for SDL_Color
struct SDL_Color_Compare {
    bool operator()(const SDL_Color& a, const SDL_Color& b) const {
        // Compare colors based on their individual components
        if (a.r != b.r) return a.r < b.r;
        if (a.g != b.g) return a.g < b.g;
        if (a.b != b.b) return a.b < b.b;
        return a.a < b.a;
    }
};

Game::Game(double hexSize, const std::vector<std::string>& asciiMap,
           int windowWidth, int windowHeight, SDL_Renderer* renderer)
    : grid(hexSize),
      playerTurn(true),
      villagerSelected(false),
      banditTexture(nullptr),
      villagerTexture(nullptr),
      villagerSelectedIndex(-1)
{
    grid.generateFromASCII(asciiMap, windowWidth, windowHeight);

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

    // Initialize bandit at a random white hex
    Hex initialBanditHex = grid.getHexes()[0];
    bool banditHexFound = false;
    for (const auto& pair : grid.getHexColors()) {
        if (pair.second.r == 255 && pair.second.g == 255 &&
            pair.second.b == 255) {
            initialBanditHex = pair.first;
            banditHexFound = true;
            break;
        }
    }
    if (!banditHexFound) {
        // Handle the case where no white hex is found (e.g., use the first hex)
        if (!grid.getHexes().empty()) {
            initialBanditHex = grid.getHexes()[0];
        } else {
            // Handle the case where the grid is empty
            std::cerr << "Error: Grid is empty!" << std::endl;
            return; // Or throw an exception
        }
    }

    bandit = Bandit(initialBanditHex);
    banditTexture = IMG_LoadTexture(renderer, bandit.getTexturePath().c_str());
    if (!banditTexture) {
        std::cerr << "Error loading bandit texture: " << IMG_GetError()
                  << std::endl;
    }

    // Create a map to store hexes by color
    std::map<SDL_Color, std::vector<Hex>, SDL_Color_Compare> hexesByColor;
    for (const auto& pair : grid.getHexColors()) {
        SDL_Color color = pair.second;
        hexesByColor[color].push_back(pair.first);
    }

    // Initialize villagers (one for each color, excluding white)
    std::random_device rd;
    std::mt19937 gen(rd());

    for (auto& [color, hexes] : hexesByColor) {
        if (color.r != 255 || color.g != 255 || color.b != 255) {
            if (!hexes.empty()) {
                // Choose a random hex of this color
                std::uniform_int_distribution<> distrib(0, hexes.size() - 1);
                int randomIndex = distrib(gen);
                Hex randomHex = hexes[randomIndex];

                villagers.emplace_back(randomHex); // Create a villager on the random hex

                SDL_Texture* villagerTexture = IMG_LoadTexture(
                    renderer, villagers.back().getTexturePath().c_str());
                if (!villagerTexture) {
                    std::cerr << "Error loading villager texture: "
                              << IMG_GetError() << std::endl;
                }
                villagerTextures.push_back(villagerTexture);
            }
        }
    }
}

Game::~Game() {
    SDL_DestroyTexture(banditTexture);

    for (SDL_Texture* texture : villagerTextures) {
        SDL_DestroyTexture(texture);
    }
    villagerTextures.clear();
}

void Game::handleEvent(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Hex clickedHex = grid.pixelToHex(mouseX, mouseY);

        if (playerTurn) {
            if (!villagerSelected) {
                // Check if any villager is selected
                for (size_t i = 0; i < villagers.size(); ++i) {
                    if (clickedHex == villagers[i].getHex()) {
                        villagerSelectedIndex = i;  // Store the index of the selected villager
                        villagerSelected = true;
                        break;
                    }
                }
            } else {
                if (grid.hexExists(clickedHex)) {
                    villagers[villagerSelectedIndex].move(grid, clickedHex);
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
        static_cast<int>(grid.hexToPixel(bandit.getHex()).x -
                         grid.getHexSize() / 2),
        static_cast<int>(grid.hexToPixel(bandit.getHex()).y -
                         grid.getHexSize() / 2),
        static_cast<int>(grid.getHexSize()),
        static_cast<int>(grid.getHexSize())};
    SDL_RenderCopy(renderer, banditTexture, NULL, &banditRect);

    // Render all villagers
    for (size_t i = 0; i < villagers.size(); ++i) {
        SDL_Rect villagerRect = {
            static_cast<int>(grid.hexToPixel(villagers[i].getHex()).x -
                             grid.getHexSize() / 2),
            static_cast<int>(grid.hexToPixel(villagers[i].getHex()).y -
                             grid.getHexSize() / 2),
            static_cast<int>(grid.getHexSize()),
            static_cast<int>(grid.getHexSize())};
        SDL_RenderCopy(renderer, villagerTextures[i], NULL, &villagerRect);
    }
}
