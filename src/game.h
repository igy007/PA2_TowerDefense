
#ifndef __GAME_H
#define __GAME_H

#include <vector>
#include "CObject.h"
#include "enemies.h"
#include "towers.h"

/* -------------------------------------------------
    DEFAULT VALUES FOR GAME INITIALISATION
        - map width must be at least 40 tiles long
        - map height must be: 
            > an odd number 
            > 1/4 size the width at max
            > 9 tiles in minimum
*/
const unsigned MIN_MAP_WIDTH    = 36;
const unsigned MIN_MAP_HEIGHT   = 9 ;

const unsigned DEFAULT_GAME_DIFF    = 1;              
const unsigned DEFAULT_MAP_WIDTH    = MIN_MAP_WIDTH ;
const unsigned DEFAULT_MAP_HEIGHT   = MIN_MAP_HEIGHT; 

const unsigned DEFAULT_START_MONEY   = 500; 
const int      DEFAULT_START_MP      = 1000; 
const short    DEFAULT_LIFE_COUNT    = 5;



class GAME
{
    
    /* 
        General fail-safe variable - without this
        option set to TRUE, game will not proceed
        to operate. 
    */
    bool game_initialised;

    short map_width, map_height;

    unsigned money;
    int monster_points; 

    short lives;
    short round;
    short game_difficulty;
    short game_state;
    
    std::vector < std::vector<CObject *> > map_objects;
    std::vector < CEnemy * > active_enemies;


public:
    
    GAME( void ) : game_initialised(false), round(1), game_state(INIT) {}; // default constructor
    
    void init( std::vector<char> = std::vector<char>(), short = DEFAULT_GAME_DIFF, short = DEFAULT_MAP_WIDTH, short = DEFAULT_MAP_HEIGHT ) noexcept(false); 

    void  display( void );  // prints all objects on the map 
    void  update ( void );  // calls action method on every object to interact with other objects
    void  run    ( void );     
    short state  ( void );
    bool  load   ( void ) noexcept(false);
    bool  save   ( void );

    void generateBorders( short = DEFAULT_MAP_WIDTH, short = DEFAULT_MAP_HEIGHT ); 
    void generateWater  ( short = DEFAULT_MAP_WIDTH, short = DEFAULT_MAP_HEIGHT ); 

    void towerBuilder( void );
    void buildTower  ( short, short );  

    CEnemy * spawnMonster( void );
    
    void nextRound( void );
    void pauseMenu( void );

    void cleanUP( void ); // frees-up all allocated space for objects and enemies

    enum { INIT, RUNNING, PAUSED, TOWER_BUILD, DISP_ROUND, GAME_OVER };

    
};

#endif
