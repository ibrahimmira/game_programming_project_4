#include "LevelB.h"

LevelB::LevelB()                                      : Scene { {0.0f}, nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{
   mGameState.nextSceneID = -1;

   mGameState.bgm = LoadMusicStream("assets/SCP-x7x.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

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
      {mGameState.map->getRightBoundary() - (TILE_DIMENSION / 2), mOrigin.y - 6 * (TILE_DIMENSION / 2)},
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
      "assets/lightning.png",                   // texture file address
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

    mGameState.enemyB = new Entity(
      {mOrigin.x - 200, mOrigin.y}, // position
      {85.0f * sizeRatio, 85.0f},             // scale
      "assets/lightnings_enemyB.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 9 },                                 // atlas dimensions
      AnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

    mGameState.enemyB->setAIType(FOLLOWER);
    mGameState.enemyB->setAIState(IDLE);
    mGameState.enemyB->setSpeed(Entity::DEFAULT_SPEED * 0.50f);

    mGameState.enemyB->setColliderDimensions({
      mGameState.enemyB->getScale().x / 1.5f,
      mGameState.enemyB->getScale().y / 1.2f
   });

    mGameState.enemyB->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemyB->setDirection(RIGHT);

    mGameState.enemyB2 = new Entity(
      {mOrigin.x - 100, mOrigin.y - 500.0f}, // position
      {85.0f * sizeRatio, 85.0f},             // scale
      "assets/lightnings_enemyB.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 4, 9 },                                 // atlas dimensions
      AnimationAtlas,                    // actual atlas
      NPC                                    // entity type
   );

    mGameState.enemyB2->setAIType(FOLLOWER);
    mGameState.enemyB2->setAIState(IDLE);
    mGameState.enemyB2->setSpeed(Entity::DEFAULT_SPEED * 0.50f);

    mGameState.enemyB2->setColliderDimensions({
      mGameState.enemyB2->getScale().x / 1.5f,
      mGameState.enemyB2->getScale().y / 1.2f
   });

    mGameState.enemyB2->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.enemyB2->setDirection(RIGHT);
    

}

void LevelB::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);
   if (!mGameState.displayLoserMessage) {
      
    Entity* heroCollidableEntities[] = {mGameState.enemyB, mGameState.enemyB2, gold};
    Entity* enemyCollidableEntities[] = {mGameState.hero};

    mGameState.hero->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        heroCollidableEntities,        // collidable entities
        3               // col. entity count
    );

    mGameState.enemyB->update(
        deltaTime,      // delta time / fixed timestep
        mGameState.hero,        // player
        mGameState.map, // map
        enemyCollidableEntities,        // collidable entities
        1               // col. entity count
    );

    mGameState.enemyB2->update(
        deltaTime,      // delta time / fixed timestep
        mGameState.hero,        // player
        mGameState.map, // map
        enemyCollidableEntities,        // collidable entities
        1               // col. entity count
    );

    mGameState.damageCooldown = fmaxf(0.0f, mGameState.damageCooldown - deltaTime);

    bool attackedByB  = mGameState.hero->isAttackedbyAI(mGameState.enemyB);
    bool attackedByB2 = mGameState.hero->isAttackedbyAI(mGameState.enemyB2);
      
    bool attacked = attackedByB || attackedByB2;

    if (attacked && mGameState.damageCooldown <= 0.0f) {
         mGameState.livesRemaining--;
         mGameState.damageCooldown = 1.0f;
         mGameState.nextSceneID = 2;
      }
    

    Entity *stomped = mGameState.hero->getLastNPCBottomCollision();

    if (stomped && mGameState.damageCooldown <= 0.0f) {
    stomped->deactivate();
    }

    mGameState.hero->clearLastNPCBottomCollision();
    
    Vector2 currentPlayerPosition = mGameState.hero->getPosition();

    if (mGameState.hero->getPosition().y > 800.0f) {
        mGameState.nextSceneID = 2;
        mGameState.livesRemaining--;
    } 

    panCamera(&mGameState.camera, &currentPlayerPosition);
   

    if (mGameState.hero->isCollidingBLOCK()) {
                mGameState.nextSceneID = 3;
                return;
    } 
    }
   if (mGameState.livesRemaining == 0) mGameState.displayLoserMessage = true;

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

void LevelB::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.map->render();
   mGameState.hero->render();
   mGameState.enemyB->render();
   mGameState.enemyB2->render();
   gold->render();

   if (mGameState.displayLoserMessage) {
    DrawText("You Lose!", mOrigin.x - 80, mOrigin.y, 40, RED);
   }
}


void LevelB::shutdown()
{
   delete mGameState.map;
   delete mGameState.hero;
   delete mGameState.enemyB;
   delete mGameState.enemyB2;
   delete gold;

   UnloadMusicStream(mGameState.bgm);
}