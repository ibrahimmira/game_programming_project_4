#include "LevelA.h"

constexpr int LEVEL_WIDTH = 16,
              LEVEL_HEIGHT = 9;

class LevelB : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        0,  7,  0,  41,  0,  0,  41,  0,  0,  41, 0,  0,  0,  0,  0,  7,
        0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  7,
        0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  41, 0,  0,  0,  7,
        0,  7,  0,  0,  41, 41, 41, 41, 41, 41, 0,  0,  0,  0,  0,  7,
        0,  7,  0,  41, 0,  0,  0,  0,  0,  0,  0, 0, 0, 41,  0,  7,
        0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,
        0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 35, 35, 35, 35, 35,
        15, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35
    };

public:
    static constexpr float TILE_DIMENSION       = 70.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    Entity* gold = nullptr;
    LevelB();
    LevelB(Vector2 origin, const char *bgHexCode);
    ~LevelB();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};