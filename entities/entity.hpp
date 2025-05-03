#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "../core/grid.hpp"

class Entity {
    protected:
        Hex hex;
        int protection_level;
        bool moved;
        std::string name;
        int upkeep;
        float yOffset;
        float jumpSpeed;
        bool jumping;
        bool falling;

    public:
        Entity(Hex hex, int protection_level, std::string name, int upkeep = 0);
        virtual ~Entity();
    
        // Copy constructor
        Entity(const Entity& other) : hex(other.hex), protection_level(other.protection_level), moved(other.moved), name(other.name), upkeep(other.upkeep),
            yOffset(other.yOffset),
            jumpSpeed(other.jumpSpeed),
            jumping(other.jumping)
         {}
        
        // Assignment operator
        Entity& operator=(const Entity& other) {
            if (this != &other) {
                hex = other.hex;
                protection_level = other.protection_level;
                moved = other.moved;
                name = other.name;
                upkeep = other.upkeep;
                yOffset = other.yOffset;
                jumpSpeed = other.jumpSpeed;
                jumping = other.jumping;
            }
            return *this;
        }
    
        // Virtual clone method
        virtual std::shared_ptr<Entity> clone() const {
            return std::make_shared<Entity>(*this);
        }
    
        // Getters
        Hex getHex() const { return hex; }
        std::string getName() const { return name; }
        int getProtectionLevel() const { return protection_level; }
        bool hasMoved() const { return moved; }
        int getUpkeep() const { return upkeep; }
        float getYOffset() const { return yOffset; }
        float getJumpSpeed() const { return jumpSpeed; }
        bool isJumping() const { return jumping; }
        bool isFalling() const { return falling; }

        // Setters
        void setMoved(bool moved) { this->moved = moved; }
        void setHex(Hex hex) { this->hex = hex; }
        void setYOffset(const float& newYOffset) { yOffset = newYOffset; }
        void setJumpSpeed(const float& newJumpSpeed) { jumpSpeed = newJumpSpeed; }
        void setJumping(const bool& newJumping) { jumping = newJumping; }
        void setFalling(const bool& newFalling) { falling = newFalling; }
        
        virtual bool move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor);
};
    

class Bandit : public Entity {
public:
    Bandit();
    Bandit(Hex hex);
    Bandit(const Bandit& other) : Entity(other) {}
    Bandit& operator=(const Bandit& other) {
        if (this != &other) {
            Entity::operator=(other);
        }
        return *this;
    }
    virtual ~Bandit();

    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Bandit>(*this);
    }
    bool moveBandit(HexagonalGrid& grid, Hex target);
};
        
    
class Villager : public Entity {
public:
    Villager();
    Villager(Hex hex);
    Villager(const Villager& other) : Entity(other) {}
    Villager& operator=(const Villager& other) {
        if (this != &other) {
            Entity::operator=(other);
        }
        return *this;
    }
    virtual ~Villager();
    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Villager>(*this);
    }
};

class Pikeman : public Entity {
public:
    Pikeman();
    Pikeman(Hex hex);
    Pikeman(const Pikeman& other) : Entity(other) {}
    Pikeman& operator=(const Pikeman& other) {
        if (this != &other) {
            Entity::operator=(other);
        }
        return *this;
    }
    virtual ~Pikeman();
    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Pikeman>(*this);
    }
};

class Knight : public Entity {
    public:
        Knight();
        Knight(Hex hex);
        Knight(const Knight& other) : Entity(other) {}
        Knight& operator=(const Knight& other) {
            if (this != &other) {
                Entity::operator=(other);
            }
            return *this;
        }
        virtual ~Knight();
    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Knight>(*this);
    }
};
    
class Hero : public Entity {
    public:
        Hero();
        Hero(Hex hex);
        Hero(const Hero& other) : Entity(other) {}
        Hero& operator=(const Hero& other) {
            if (this != &other) {
                Entity::operator=(other);
            }
            return *this;
        }
        virtual ~Hero();
    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Hero>(*this);
    }
};

class Devil : public Entity {
    public:
        Devil();
        Devil(Hex hex);
        Devil(const Devil& other) : Entity(other) {}
        Devil& operator=(const Devil& other) {
            if (this != &other) {
                Entity::operator=(other);
            }
            return *this;
        }
        virtual ~Devil();
    std::shared_ptr<Entity> clone() const override {
        return std::make_shared<Devil>(*this);
    }
};

#endif // ENTITY_HPP
