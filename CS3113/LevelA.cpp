#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.nextSceneID = 0;

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/master-tileset.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      10, 5,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */

   std::map<Direction, std::vector<int>> charsAnimationAtlas = {
      { DOWN,     { 18, 19, 20, 21, 22, 23, 24, 25, 26 } },
      { UP,       {  0,  1,  2,  3,  4,  5, 6, 7, 8 } },
      { RIGHT,    { 27, 28, 29, 30, 31, 32, 33, 34, 35 } },
      { LEFT,     {  9,  10, 11, 12, 13, 14, 15, 16, 17 } }
   };

   float sizeRatio  = 1.0f;

   mGameState.chars = new Entity(
      {mOrigin.x, mOrigin.y}, // position
      {100.0f * sizeRatio, 100.0f},             // scale
      "assets/lightning.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 9 },                                 // atlas dimensions
      charsAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.chars->setJumpingPower(550.0f);
   mGameState.chars->setColliderDimensions({
      mGameState.chars->getScale().x / 3.0f,
      mGameState.chars->getScale().y / 1.1f
   });
   mGameState.chars->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.chars->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
  
   /* 
      ----------- ENEMY AI -----------
   */

   std::map<Direction, std::vector<int>> ghostAnimationAtlas = {
         {DOWN,  { 0,   1,   2,    3 }},
         {RIGHT, { 4,   5,   6,    7 }},
         {UP,    { 8,   9,   10,   11 }},
         {LEFT,  { 12,  13,  14,   15 }}
    };

    mGameState.enemy = new Entity(
      {mOrigin.x, mOrigin.y}, // position
      {100.0f * sizeRatio, 100.0f},             // scale
      "assets/lightningsenemy.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 9 },                                 // atlas dimensions
      charsAnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

    mGameState.enemy->setAIType(WANDERER);
    mGameState.enemy->setAIState(IDLE);
    mGameState.enemy->setSpeed(Entity::DEFAULT_SPEED * 0.50f);

    mGameState.enemy->setColliderDimensions({
      mGameState.enemy->getScale().x / 3.0f,
      mGameState.enemy->getScale().y / 1.1f
   });

    mGameState.enemy->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemy->setDirection(RIGHT);
    mGameState.enemy->render();
}

void LevelA::update(float deltaTime)
{
   // UpdateMusicStream(mGameState.bgm);

   mGameState.chars->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   mGameState.enemy->update(
      deltaTime,      // delta time / fixed timestep
      mGameState.chars,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   // Vector2 currentPlayerPosition = { mGameState.chars->getPosition().x, mOrigin.y };
   Vector2 currentPlayerPosition = mGameState.chars->getPosition();

   if (mGameState.chars->getPosition().y > 800.0f) mGameState.nextSceneID = 1;

   panCamera(&mGameState.camera, &currentPlayerPosition);
  
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.map->render();
   mGameState.chars->render();
   mGameState.enemy->render();
}

void LevelA::shutdown()
{
   delete mGameState.map;
   delete mGameState.chars;
   delete mGameState.enemy;
}