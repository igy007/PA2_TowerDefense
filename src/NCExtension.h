// NCurses Extension 

#ifndef __NCExt_H
#define __NCExt_H

#include <ncurses.h>
#include <unistd.h> // time handling
#include <list>
#include <vector>
#include <iostream>

// NExtension Version (string/f.point)
#define _NCExt_VER_S "1.0"
#define _NCExt_VER_F  1.0


// PRINTING ATTRIBUTES
#define HIGHLIGHT 1
#define	NO_REF	  2
#define CENTER    4
#define MIDDLE    8
// -------------------


// ---------- COLOR DEFINITIONS ----------
#define RED_PAIR 	1
#define GREEN_PAIR 	2
#define BLUE_PAIR 	3  /* CYAN ACTUALLY */
#define YELLOW_PAIR 4
#define GRAY_PAIR 	5
#define WHITE_PAIR 	6
#define ORANGE_PAIR 7
#define DENIED_PAIR 8  // used with tower builder
// ---------------------------------------


// screen initialisation
void screenInit( void );

// default color pairs initialisation
void init_color_pairs( void );

// returns the actual postion of a cursor
unsigned cursorPosX( void );
unsigned cursorPosY( void );

// return actual screen width/height
unsigned screenHeight( void );
unsigned screenWidth ( void );

void screenResize( unsigned, unsigned );

// Regular printing:
void screen_print( const char *, int = 0 );
void screen_print( const char *, unsigned, unsigned, int = 0 );

void screen_print( char, int = 0 );
void screen_print( char, unsigned, unsigned, int = 0 );

void screen_print( const int, int = 0 );
void screen_print( const int, unsigned, unsigned, int = 0 );

void screen_print( const unsigned, int = 0 );
void screen_print( const unsigned, unsigned, unsigned, int = 0 );
// -----------------


// Color printing:
void color_print( const char *, unsigned, unsigned, int, int = 0 );
void color_print( const char *, int, int = 0 );

void color_print( char, unsigned, unsigned, int, int = 0 );
void color_print( char, int, int = 0 );

void color_print( const unsigned, unsigned, unsigned, int, int = 0 );
void color_print( const unsigned, int, int = 0 );

void color_print( const int, unsigned, unsigned, int, int = 0 );
void color_print( const int, int, int = 0 );

// ---------------


// return true if any key press has occured
bool inputPresent( void );

void printNewLine( void );


// ==================== TIME HANDLING SECTION ====================

void hard_delay_ms( int ); 
void hard_delay_s ( int );

// ===============================================================

class CMENU
{

	private:
    
    std::vector <std::string> items;
    int position;
    std::string name;

	public:

    CMENU( void ) : position(0) {};
    CMENU( const std::list<std::string> & menu_items );
    CMENU( const std::string, const std::list<std::string> & menu_items );

    void arrowPrint( void );
    void highlightPrint( void );

    void print( void );

    void addOption( std::string, int = -1 );
    void addOption( const char *, int = -1 );

    void next( void ) { position++; position %= items.size(); }
    void prev( void ) { position--; if (position < 0) ( position = (items.size()-1) ); }

    unsigned size( void ) { return items.size(); }

    unsigned option( void ) { return position; };

};

bool confirmMenu( std::string );


#endif
