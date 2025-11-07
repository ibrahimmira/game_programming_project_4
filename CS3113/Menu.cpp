#include "Menu.h"

Menu::Menu()                                      : Scene { {0.0f}, nullptr   } {}
Menu::Menu(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Menu::~Menu() { shutdown(); }

void Menu::initialise()
{
    mGameState.nextSceneID = 0;

    /*
      ----------- CAMERA -----------
    */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin; // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
   
   
}

void Menu::update(float deltaTime)
{
   // UpdateMusicStream(mGameState.bgm);
   // printf("Menu's update got called!\n");
   // DrawText(TextFormat("Press enter to start"), 1500 - 300, 50, 20, WHITE);
   // DrawText(TextFormat("Press enter to start"), mOrigin.x, mOrigin.y, 50, WHITE);
   // printf("Menu's DrawText got called!\n");

   if (mCondition) mGameState.nextSceneID = 1;
}

void Menu::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   DrawText(TextFormat("IbraGame"), mOrigin.x - 90, mOrigin.y - 70, 40, WHITE);
   DrawText(TextFormat("Press enter to start"), mOrigin.x - 150, mOrigin.y + 50, 30, WHITE);
}

void Menu::shutdown() {}