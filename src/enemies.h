
#ifndef __ENEMIES_H
#define	__ENEMIES_H

#include "CObject.h"
#include <queue>


/*
        DEFAULT VALUES FOR ENEMIES
     (if not loaded from config file)
*/
const int   _DEF_TROOP_HP     = 20;
const int   _DEF_MURLOC_HP    = 10;
const int   _DEF_GARGOYLE_HP  = 35;
const int   _DEF_DRAGON_HP    = 100;

const short _DEF_TROOP_PRICE     = 200;
const short _DEF_MURLOC_PRICE    = 100;
const short _DEF_GARGOYLE_PRICE  = 500;
const short _DEF_DRAGON_PRICE    = 1000;

const short _DEF_TROOP_SPEED     = 1;
const short _DEF_MURLOC_SPEED    = 1;
const short _DEF_GARGOYLE_SPEED  = 1;
const short _DEF_DRAGON_SPEED    = 2;



class CEnemy : public CObject
{
    
protected:
    
    int HP;             // can go to negative numbers - then it's dead

    short speed;        // enemy's actual movement speed - can get affected
    short cooldown;     // movement cooldown

    bool slowed;        // enemy's possible handicap.. in real world, universal class would be better, but no time RN
    bool on_fire;       // enemy's possible handicap.. in real world, universal class would be better, but no time RN

    std::vector< std::vector< CObject * > > * field; // playing field (map)
    std::queue< std::pair<short, short> > path; // coordinates for movement
    
public:
    
    CEnemy( short, short, std::vector< std::vector<CObject*> > & );

    int getHP( void ) const { return HP; };
    
    virtual void takeDamage( short raw_damage, int special );  // special attribute affects enemy´s abilities 

    const short getColor( void ) const;

    virtual void move( void ) = 0;  

    virtual const char * name( void ) const = 0;    // each enemy has to have it's own name defined

    void action( void );
    
};









/* 
 *  WALKING 
 *       ENEMY 
 */ 

class walking_enemy: public CEnemy
{

public:
    
    walking_enemy( short, short, std::vector< std::vector<CObject*> > & );

    void move( void );
    
    virtual const char * name( void ) const { return "UNKNOWN WALKING UNIT"; };
    
};



/* 
 *  SWIMMING 
 *       ENEMY 
 */ 

class swimming_enemy : public CEnemy
{   
    
public:
    
    swimming_enemy( short, short, std::vector< std::vector<CObject*> > & );

    void move( void );

    virtual const char * name( void ) const { return "UNKNOWN SWIMMING UNIT"; };
    
};



/* 
 *  FLYING 
 *     ENEMY 
 */ 

class flying_enemy : public CEnemy
{
    
    
public:
    
    flying_enemy( short, short, std::vector< std::vector<CObject*> > & );
   
    void move( void ) override; // has it's own distinctive movement

    virtual const char * name( void ) const { return "UNKNOWN FLYING UNIT"; };
    
};



// ---------------------------------------------------------------------------------------------------



class troop : public walking_enemy
{
    
public:

    troop( short, short, std::vector< std::vector<CObject*> > & );

    const char * name( void ) const { return "TROOP"; };

};




class murloc : public swimming_enemy
{
    
public:

    murloc( short, short, std::vector< std::vector<CObject*> > & );

    const char * name( void ) const { return "MURLOC"; };
    
};



class gargoyle: public flying_enemy
{
   
public:
    
    gargoyle( short, short, std::vector< std::vector<CObject*> > & );

    const char * name( void ) const { return "GARGOYLE"; };
    
};



class dragon : public flying_enemy
{

public:
    
    dragon( short, short, std::vector< std::vector<CObject*> > & );
	
    // dragons are immune to fire, oil towers heal them ( dragons are badass! )
	void takeDamage( short raw_damage, int special ) override;

    const char * name( void ) const { return "DRAGON"; };
    
};


#endif
