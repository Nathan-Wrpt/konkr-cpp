#ifndef RENDERGAME_HPP
#define RENDERGAME_HPP

#include <SDL2/SDL_ttf.h>

#include "../constants/constants.hpp"
#include "../entities/entity.hpp"
#include "../entities/entitymanager.hpp"
#include "../players/player.hpp"
#include "../ui/button.hpp"
#include "../ui/data.hpp"

class RenderGame {
public:
    SDL_Rect entityToRect(const Entity& entity, const HexagonalGrid& grid, int cameraX, int cameraY) const;
    void render_entity(SDL_Renderer* renderer, const Entity& entity, SDL_Texture* texture, const HexagonalGrid& grid, int cameraX, int cameraY) const;
    void renderButton(SDL_Renderer* renderer, const Button& button, const std::vector<SDL_Texture*>& textures) const;
    void renderTurnButton(SDL_Renderer* renderer, const Button& turnButton, const std::vector<SDL_Texture*>& textures, const std::vector<std::shared_ptr<Player>>& players, size_t playerTurn) const;
    void render(SDL_Renderer* renderer, 
                const HexagonalGrid& grid, 
                const std::vector<std::shared_ptr<Bandit>>& bandits, 
                const std::vector<std::shared_ptr<BanditCamp>>& banditCamps, 
                const std::vector<std::shared_ptr<Treasure>>& treasures, 
                const std::vector<std::shared_ptr<Devil>>& devils, 
                const std::vector<std::shared_ptr<Player>>& players, 
                size_t playerTurn, 
                int cameraX, 
                int cameraY, 
                const std::vector<SDL_Texture*>& textures,
                const std::vector<Button>& unitButtons,
                const Button& turnButton,
                bool entitySelected, int selectedEntityIndex
            ) const;
private:
    EntityManager entityManager;
};

#endif // RENDERGAME_HPP