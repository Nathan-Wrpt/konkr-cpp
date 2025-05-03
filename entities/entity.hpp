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
    
        //Virtual clone method
        virtual std::shared_ptr<Entity> clone() const {
            return std::make_shared<Entity>(*this);
        }
    
        // Getter for hex
        Hex getHex() const { return hex; }
    
        // Getter for name
        std::string getName() const { return name; }
    
        // Getter for protection level
        int getProtectionLevel() const { return protection_level; }
    
        // Getter for moved
        bool hasMoved() const { return moved; }
    
        // Setter for moved
        void setMoved(bool moved) { this->moved = moved; }
        
        virtual bool move(HexagonalGrid& grid, Hex target, const SDL_Color& ownerColor);
    
        // Getter for upkeep
        int getUpkeep() const { return upkeep; }
    
        // Setter for hex
        void setHex(Hex hex) { this->hex = hex; }
    
        float getYOffset() const { return yOffset; }
        void setYOffset(const float& newYOffset) { yOffset = newYOffset; }
        float getJumpSpeed() const { return jumpSpeed; }
        void setJumpSpeed(const float& newJumpSpeed) { jumpSpeed = newJumpSpeed; }
        bool isJumping() const { return jumping; }
        void setJumping(const bool& newJumping) { jumping = newJumping; }
        bool isFalling() const { return falling; }
        void setFalling(const bool& newFalling) { falling = newFalling; }
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
