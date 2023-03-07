
#ifndef _CObject_H
#define	_CObject_H

#include "NCExtension.h"
#include <vector>



class CObject
{

public:

    // type determines, wether enemy units can walk or fly over the object
    enum OBJECT_TYPE { CREATURE, LAND, WATER, OBSTACLE };

    
protected:

    short  X, Y;                      // position
    short  color;                     // defines color of given object
    std::vector<char> object_chars;   // symbols representing given object 
    OBJECT_TYPE object_type;          // type of given object (tower/creature..)

    short char_used;                  // used for getChar function if more chars for one object are set
    short price;

public:
    
    CObject( void ) : X(0), Y(0), color(WHITE_PAIR), object_chars({'?'}), object_type(LAND) {};

    CObject( short x, short y, OBJECT_TYPE type = LAND, short new_color = WHITE_PAIR ) 
        : X(x), Y(y), color( new_color ), object_chars({'?'}), object_type( type ), char_used(0), price(9999)  {}
  
    OBJECT_TYPE  getType( void ) const { return this -> object_type; } 
    short       getPrice( void ) const { return price; }

    virtual void action( void ) = 0; 

    const char  getChar ( void ) { char_used++; if ( char_used >= (short)object_chars.size() ) char_used = 0; return object_chars[char_used]; };
    const short getColor( void ) const { return color; };

    const short getX( void ) const { return X; };
    const short getY( void ) const { return Y; };

    virtual  ~CObject( void ) = default;

};



// to be used in a map as a field where player cannot build
// and some monsters cannot pass through 
class water : public CObject
{
    
public:
    
    water ( short x, short y ) : CObject( x, y, WATER, BLUE_PAIR ) 
    { 
        object_chars = {'~'}; 
    }

    void action( void ) { };
    ~water( void ) = default;

};



#endif
