#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.nextSceneID = -1;

   mGameState.bgm = LoadMusicStream("assets/Mountain Emperor.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/jump.wav");
   mGameState.gameOver= LoadSound("assets/game_over.mp3");
   mGameState.lifeLost = LoadSound("assets/life_lost.mp3");
   mGameState.levelWin = LoadSound("assets/win_level.mp3");

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
      ----------- GOLD -----------
   */
   gold = new Entity(
      {mOrigin.x, mGameState.map->getBottomBoundary() - 3 * (TILE_DIMENSION / 2)},
      { TILE_DIMENSION, TILE_DIMENSION },
      "assets/gold.png",
      BLOCK
   );

   /*
      ----------- PROTAGONIST -----------
   */

   std::map<Direction, std::vector<int>> AnimationAtlas = {
      { DOWN,     {  9,  10, 11, 12, 13, 14, 15, 16, 17 } },
      { UP,       {  27, 28, 29, 30, 31, 32, 33, 34, 35 } },
      { RIGHT,    {  0,  1,  2,  3,  4,  5, 6, 7, 8} },
      { LEFT,     {  18, 19, 20, 21, 22, 23, 24, 25, 26 } }
   };
   
   float sizeRatio  = 36.0f / 55.0f;

   mGameState.hero = new Entity(
      {mOrigin.x, mOrigin.y}, // position
      {85.0f * sizeRatio, 85.0f},             // scale
      "assets/hero.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 9 },                                 // atlas dimensions
      AnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.hero->setJumpingPower(550.0f);
   mGameState.hero->setColliderDimensions({
      mGameState.hero->getScale().x / 1.5f,
      mGameState.hero->getScale().y / 1.2f
   });

   mGameState.hero->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.hero->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
  
   /* 
      ----------- ENEMY AI -----------
   */

    mGameState.enemyA = new Entity(
      {mOrigin.x - 200, mOrigin.y}, // position
      {85.0f * sizeRatio, 85.0f},             // scale
      "assets/heros_enemyA.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 9 },                                 // atlas dimensions
      AnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

    mGameState.enemyA->setAIType(WANDERER);
    mGameState.enemyA->setAIState(IDLE);
    mGameState.enemyA->setSpeed(Entity::DEFAULT_SPEED * 0.50f);

    mGameState.enemyA->setColliderDimensions({
      mGameState.enemyA->getScale().x / 1.5f,
      mGameState.enemyA->getScale().y / 1.2f
   });

    mGameState.enemyA->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemyA->setDirection(RIGHT);
    
}

void LevelA::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);
   if (!mGameState.displayLoserMessage) {
      
      Entity* heroCollidableEntities[] = {mGameState.enemyA, gold};
      Entity* enemyCollidableEntities[] = {mGameState.hero};

      mGameState.hero->update(
         deltaTime,      // delta time / fixed timestep
         nullptr,        // player
         mGameState.map, // map
         heroCollidableEntities,        // collidable entities
         2               // col. entity count
      );

      mGameState.enemyA->update(
         deltaTime,      // delta time / fixed timestep
         mGameState.hero,        // player
         mGameState.map, // map
         enemyCollidableEntities,        // collidable entities
         1               // col. entity count
      );

      if (mGameState.hero->isCollidingBLOCK()) {
         mGameState.nextSceneID = 2;
         PlaySound(mGameState.levelWin);
         return;
      } 

      mGameState.damageCooldown = fmaxf(0.0f, mGameState.damageCooldown - deltaTime);

      bool attacked = mGameState.hero->isAttackedbyAI(mGameState.enemyA);

      if (attacked && mGameState.damageCooldown <= 0.0f) {
         mGameState.livesRemaining--;
         PlaySound(mGameState.lifeLost);
         mGameState.damageCooldown = 1.0f;
         mGameState.nextSceneID = 1;
      }

      bool defeatedEnemy = mGameState.hero->isCollidingBottomAI();
      if (defeatedEnemy && mGameState.damageCooldown <= 0.0f) {
         mGameState.enemyA->deactivate();
      }

      Vector2 currentPlayerPosition = mGameState.hero->getPosition();

      if (mGameState.hero->getPosition().y > 800.0f) {
         mGameState.nextSceneID = 1;
         mGameState.livesRemaining--;
      } 

      panCamera(&mGameState.camera, &currentPlayerPosition);

   }

   if (mGameState.livesRemaining == 0) {

      mGameState.displayLoserMessage = true;
      PlaySound(mGameState.gameOver);

   }

   if (mGameState.displayLoserMessage) {
      mGameState.displayMessageTimer -= deltaTime;
   }
   if (mGameState.displayMessageTimer <= 0) {
      mGameState.livesRemaining = 3;
      mGameState.displayLoserMessage = false;
      mGameState.displayMessageTimer = 3.0f;
      mGameState.nextSceneID = 0;
   } 
  
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.map->render();
   mGameState.hero->render();
   mGameState.enemyA->render();
   gold->render();

   if (mGameState.displayLoserMessage) {
    DrawText("You Lose!", mOrigin.x - 80, mOrigin.y, 40, RED);
   }
}


void LevelA::shutdown()
{
   delete mGameState.map;
   delete mGameState.hero;
   delete mGameState.enemyA;
   delete gold;

   UnloadMusicStream(mGameState.bgm);

   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.levelWin);
   UnloadSound(mGameState.gameOver);
   UnloadSound(mGameState.lifeLost);
}