#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f}, nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
   mGameState.nextSceneID = -1;

   mGameState.bgm = LoadMusicStream("assets/Controlled Chaos.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/jump.wav");
   mGameState.gameOver= LoadSound("assets/game_over.mp3");
   mGameState.lifeLost = LoadSound("assets/life_lost.mp3");
   mGameState.levelWin = LoadSound("assets/win_level.mp3");
   mGameState.gameWin = LoadSound("assets/win_game.mp3");


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
      {mOrigin.x, mOrigin.y - 16 * (TILE_DIMENSION / 2)},
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

    mGameState.enemyC = new Entity(
      {mOrigin.x - 200, mOrigin.y}, // position
      {85.0f * sizeRatio, 85.0f},             // scale
      "assets/heros_enemyC.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 9 },                                 // atlas dimensions
      AnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

    mGameState.enemyC->setAIType(FLYER);

    mGameState.enemyC->setBaseAltitude(mOrigin.y);
    mGameState.enemyC->setHomePosition(mOrigin);

    mGameState.enemyC->setAIState(IDLE);
    mGameState.enemyC->setSpeed(Entity::DEFAULT_SPEED * 0.50f);

    mGameState.enemyC->setColliderDimensions({
      mGameState.enemyC->getScale().x / 1.5f,
      mGameState.enemyC->getScale().y / 1.2f
   });

    mGameState.enemyC->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemyC->setDirection(RIGHT);

}

void LevelC::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);
   if (!mGameState.displayLoserMessage && !mGameState.displayWinnerMessage) {
      
    Entity* heroCollidableEntities[] = {mGameState.enemyC, gold};
    Entity* enemyCollidableEntities[] = {mGameState.hero};

    mGameState.hero->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        heroCollidableEntities,        // collidable entities
        2               // col. entity count
    );

    mGameState.enemyC->update(
        deltaTime,      // delta time / fixed timestep
        mGameState.hero,        // player
        mGameState.map, // map
        enemyCollidableEntities,        // collidable entities
        1               // col. entity count
    );

   if (mGameState.hero->isCollidingBLOCK()) {
                // mGameState.nextSceneID = 3;
                // return;
                mGameState.displayWinnerMessage = true;
                PlaySound(mGameState.gameWin);
                return;
    } 

    mGameState.damageCooldown = fmaxf(0.0f, mGameState.damageCooldown - deltaTime);

    bool attacked  = mGameState.hero->isAttackedbyAI(mGameState.enemyC);
      
    if (attacked && mGameState.damageCooldown <= 0.0f) {
         mGameState.livesRemaining--;
         PlaySound(mGameState.lifeLost);
         mGameState.damageCooldown = 1.0f;
         mGameState.nextSceneID = 3;
      }
    
   // Commented the part below to make level more difficult
   //  Entity *stomped = mGameState.hero->getLastNPCBottomCollision();

   //  if (stomped && mGameState.damageCooldown <= 0.0f) {
   //  stomped->deactivate();
   //  }

    mGameState.hero->clearLastNPCBottomCollision();
    
    Vector2 currentPlayerPosition = mGameState.hero->getPosition();

    if (mGameState.hero->getPosition().y > 800.0f) {
        mGameState.nextSceneID = 2;
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
   if (mGameState.displayWinnerMessage) {
      mGameState.displayMessageTimer -= deltaTime;
   }
   if (mGameState.displayMessageTimer <= 0) {
      mGameState.livesRemaining = 3;
      mGameState.displayLoserMessage = false;
      mGameState.displayWinnerMessage = false;
      mGameState.displayMessageTimer = 3.0f;
      mGameState.nextSceneID = 0;
   } 
  
}

void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.map->render();
   mGameState.hero->render();
   mGameState.enemyC->render();
   gold->render();

   if (mGameState.displayLoserMessage) {
    DrawText("You Lose!", mOrigin.x - 80, mOrigin.y, 80, RED);
   }

   if (mGameState.displayWinnerMessage) {
    DrawText("You Win!", mOrigin.x - 80, 0, 80, GREEN);
   }
}


void LevelC::shutdown()
{
   delete mGameState.map;
   delete mGameState.hero;
   delete mGameState.enemyC;
   delete gold;

   UnloadMusicStream(mGameState.bgm);

   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.levelWin);
   UnloadSound(mGameState.gameOver);
   UnloadSound(mGameState.lifeLost);
   UnloadSound(mGameState.gameWin);
}