#include "LevelC.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 2;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;
Menu *gMenuScreen = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    int carriedLives = (gCurrentScene) ? gCurrentScene->getLives() : scene->getLives();
    gCurrentScene = scene;
    gCurrentScene->initialise();
    gCurrentScene->setLives(carriedLives);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gMenuScreen = new Menu(ORIGIN,"#000000ff");

    gLevelA = new LevelA(ORIGIN, "#b1eafaff");
    gLevelB = new LevelB(ORIGIN, "#ffddb3ff");
    gLevelC = new LevelC(ORIGIN, "#9d8cfdff");

    gLevels.push_back(gMenuScreen);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);

    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);

}

void processInput() 
{

    if (gCurrentScene == gMenuScreen) {

        if (IsKeyPressed(KEY_ENTER)) gMenuScreen->setGameCondition();
    }
    if (gCurrentScene != gMenuScreen) {
        gCurrentScene->getState().hero->resetMovement();

        
        if      (IsKeyDown(KEY_A)) gCurrentScene->getState().hero->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().hero->moveRight();

        if (IsKeyPressed(KEY_W) && 
            gCurrentScene->getState().hero->isCollidingBottom())
        {
            gCurrentScene->getState().hero->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (GetLength(gCurrentScene->getState().hero->getMovement()) > 1.0f) 
            gCurrentScene->getState().hero->normaliseMovement();
    }
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D (gCurrentScene->getState().camera);

    gCurrentScene->render();

    EndMode2D();
    if (gCurrentScene != gMenuScreen){
    DrawText(TextFormat("Lives Left: %d", gCurrentScene->getState().livesRemaining), 25, 25, 20, RED);
    }
    EndDrawing();
}

void shutdown() 
{
    delete gMenuScreen;
    delete gLevelA;
    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID >= 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}