
#ifndef __TOWERS_H
#define	__TOWERS_H

#include <vector>
#include "CObject.h"
#include "enemies.h"    


/* default tower prices - to be rewritten by config file */
const short _DEF_WATCHTOWER_PRICE = 200;
const short _DEF_OILTOWER_PRICE   = 500;
const short _DEF_FIRETOWER_PRICE  = 750;
const short _DEF_WALL_PRICE       = 50;

const short _DEF_WATCHTOWER_DMG   = 5;
const short _DEF_OILTOWER_DMG     = 1;
const short _DEF_FIRETOWER_DMG    = 2;

const short _DEF_WATCHTOWER_RANGE = 2;
const short _DEF_OILTOWER_RANGE   = 1;
const short _DEF_FIRETOWER_RANGE  = 1;

const short _DEF_WATCHTOWER_FR    = 2;
const short _DEF_OILTOWER_FR      = 4;
const short _DEF_FIRETOWER_FR     = 3;


// basic abstract tower
class CTower : public CObject
{
    
protected:
    
    short damage;        // amount of damage a tower inflicts
    short range;         // tile range
    
    short fire_rate;     // how long will it take to get ready to shoot again (cooldown init value)
    short cooldown;      // actual cooldown ^^^

    int special;         // defines special abilities of particular tower (in real word, I'd use a special class, but few hours 'till deadline say NO)
    
    std::vector< CEnemy * > * enemies; // pointer to a vector of enemies on the map provided by the game
    
public:
    
    CTower( void );
    CTower( short, short, std::vector<CEnemy*> * );
    
};




class single_shot : public CTower
{

public:
    
    single_shot( short new_x, short new_y, std::vector<CEnemy*> * enemies ) 
        : CTower( new_x, new_y, enemies ){}

    void action( void );

};



class spread_shot : public CTower
{

public:
    
    spread_shot( short new_x, short new_y, std::vector<CEnemy*> * enemies ) 
        : CTower( new_x, new_y, enemies ){}
        
    void action( void );

};



// basic tower
class watch_tower : public single_shot
{
    
public:
    
    watch_tower( void );
    watch_tower( short, short, std:: vector< CEnemy*> * );

};


// slows down enemies, takes off flying ones
class oil_tower : public spread_shot
{
    
    
public:
    
    oil_tower( void );
    oil_tower( short, short, std:: vector< CEnemy*> * );
    
};



// inflicts damage to enemies that lasts, until passing water (if capable)
class fire_tower : public single_shot
{
    
    
public:
    
    fire_tower( void );
    fire_tower( short, short, std::vector<CEnemy*> * );
    
};



// just a regular wall - useful for blocking enemies
class wall : public CObject
{

    
public:
    
    wall( void );
    wall( short, short );

    void action( void ) {};
    ~wall( void ) = default;

};

#endif
