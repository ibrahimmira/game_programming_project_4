#include "LevelA.h"

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
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");

    gMenuScreen = new Menu(ORIGIN,"#000000ff");

    //printf("I reached post menu init !!!!!!!!\n");

    gLevelA = new LevelA(ORIGIN, "#b1eafaff");

    gLevels.push_back(gMenuScreen);
    gLevels.push_back(gLevelA);

    //printf("I reached most menu push back!!!!!!!!\n");

    switchToScene(gLevels[0]);

    //printf("I reached post switchtoscene!!!!!!!!\n");

    SetTargetFPS(FPS);

    //printf("I reached post settargetfps!!!!!!!\n");
}

void processInput() 
{
    // printf("I reached process input!!!!!!!!");

    // if (IsKeyPressed(KEY_ENTER)) gMenuScreen->
    if (gCurrentScene == gMenuScreen) {

        if (IsKeyPressed(KEY_ENTER)) gMenuScreen->setGameCondition();
    }
    if (gCurrentScene != gMenuScreen) {
        gCurrentScene->getState().chars->resetMovement();

        
        if      (IsKeyDown(KEY_A)) gCurrentScene->getState().chars->moveLeft();
        else if (IsKeyDown(KEY_D)) gCurrentScene->getState().chars->moveRight();

        if (IsKeyPressed(KEY_W) && 
            gCurrentScene->getState().chars->isCollidingBottom())
        {
            gCurrentScene->getState().chars->jump();
            // PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (GetLength(gCurrentScene->getState().chars->getMovement()) > 1.0f) 
            gCurrentScene->getState().chars->normaliseMovement();
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

    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID > 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}