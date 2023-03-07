// ------------------- DEVELOPER MODE -------------------
//						#define DEV
// ------------------------------------------------------

// Using ncurses for the graphics.
#include <ncurses.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "NCExtension.h"
#include "CObject.h"
#include "game.h"
#include "exceptions.h"
		
#define GAME_NAME "Tower Defense "
#define GAME_VERSION "1.0"


const  bool READ_CONFIG = false;	 	  // shall the stats be read from a file ?
extern void readConfigTowers ( bool );      /*  These functions are defined  */
extern void readConfigEnemies( bool );	    /*     in other source files     */


using namespace std;


// POSSIBLE PROGRAM SATES
enum STATES
{
	SPLASH,
	MENU,
	SETTINGS,
	NEW_GAME,
	GAME_LOAD,
	ACTIVE,
	EXIT
};


STATES GLOBAL_STATE = SPLASH;





// ===================================================== GENERIC FUNCTIONS =====================================================



void init()
{

	initscr();							// NCURSES init
	screenInit();						// NCExtension init
	curs_set(0); 						// disables the cursor
	keypad( stdscr, true );				// allows ncurses to use direction keys
	nodelay(stdscr, TRUE); 				// makes wgetch(stdscr) a non-bocking call (which is awesome!)
	set_escdelay(0);					// zero delay after ESC key press ( default is ~2000 fr some reason..)

	srand( time(NULL) ); 		// init pseudo-random number generator with time as a seed

	readConfigTowers ( READ_CONFIG );
	readConfigEnemies( READ_CONFIG );


	// checks if character color can be tweaked, text to be erased
	if ( has_colors() )
	{
		start_color();
		init_color_pairs();
	}

	noecho();	// ctrl + C won't be printed

	// cbreak is default, doesn't hurt tho
	#if defined DEV
		cbreak(); 	// ctrl + C exits (WARNING: may result in memory leaks!)
	#endif

	#ifndef DEV
		raw(); 		// ctrl + C won't exit the game
	#endif

}




void splashScreen( void )
{

	clear(); refresh();
	screen_print(GAME_NAME, 			0, (screenHeight()/2)-3,  MIDDLE | NO_REF );
	screen_print("Dominik Igerský",  	0, (screenHeight()/2)-2,  MIDDLE | NO_REF );
	screen_print("© 2020 ", 			0, (screenHeight()/2)-1,  MIDDLE | NO_REF );
	screen_print("Version: ",	cursorPosX()-11,   (screenHeight()/2),	   NO_REF );
	screen_print(GAME_VERSION);

	printNewLine();
	printNewLine();

	for ( unsigned i = 0; i < screenWidth(); ++i )
	{
		if ( inputPresent() ) // press any key to interrupt the splash screen dragon fly-by
		{
			getch(); // prevent unwanted pre-input
			return;
		}

		move(cursorPosY(), 0); 
		clrtoeol(); // clears line
		
		color_print("v", i++, cursorPosY(), GRAY_PAIR );
		hard_delay_ms(100);
		
		move(cursorPosY(), 0);
		clrtoeol(); // also clears line

		color_print("^", i, cursorPosY(), GRAY_PAIR );
		hard_delay_ms(100); 
	}

}




void mainMenu( void )
{

	static CMENU main_menu( "Tower Defense", {"New Game", "Load Game", "Settings", "Exit"} );
	
	main_menu.arrowPrint();

	while ( !inputPresent() )
	{ hard_delay_ms(15); }
	
	switch( getch() )
	{	
		case KEY_UP:
			main_menu.prev();
		return;

		case KEY_DOWN:
			main_menu.next();
		return;

		case 10: // 10 == ENTER
		break; // change state

		default:
		return;

	}

	// change state
	switch ( main_menu.option() )
	{
		case 0:
			GLOBAL_STATE = NEW_GAME;
		break;
		
		case 1:
			GLOBAL_STATE = GAME_LOAD;
		break;
		
		case 2:
			GLOBAL_STATE = SETTINGS;
		break;
		
		case 3:
			GLOBAL_STATE = EXIT;
		break;
	
		default:
		 // do nothing 
		break;

	}


}




int ExitGame() // destruct da game
{
	endwin();
	return 0;
}




bool mapSelection( GAME & game ) noexcept(false)
{

	ifstream map;
	char route[30];
	static bool maps_checked = false; // fail safe
	string map_name;
	int difficulty = 1; // TODO: ingame difficulty not yet implemented - using 1 as a default value

	static CMENU maps_menu( "Choose a map:", {"Randomly generated"} ); // new CMenu

	if ( !maps_checked )
	{
		// check available maps
		for ( int i = 1; i < 6; ++i )
		{
			sprintf ( route, "./examples/maps/map%d", i );
			map.open( route, std::fstream::in );

			if ( map.is_open() ) // if map file exists
			{
				getline(map, map_name); // read the first line of the map file to acquire map's name
				if ( map_name.size() > 20 ) // 20 characters for the name at max
					maps_menu.addOption("Map name too long..");
				else
					maps_menu.addOption(map_name);

				map.close();
			} 
			else
				break; // if there is less then 5 maps or maps are named in incorrect succession..
		}
	}

	maps_checked = true;

	maps_menu.highlightPrint();

	while ( !inputPresent() ) 
	{ hard_delay_ms(15); }
	
	switch( getch() )
		{	
			
			case KEY_UP:
				maps_menu.prev();
			return false;

			case KEY_DOWN:
				maps_menu.next();
			return false;

			case 27: // ESC
				GLOBAL_STATE = MENU;
			return false;

			case 10: // 10 == ENTER
			break; // change state

			default:
			return false;	

		}

		// if "random" selected..
		if ( maps_menu.option() == 0 )
		{
			game.init(); // ..initialise with default parameters
			return true;
		}


		// >> check validity of chosen map
		string data;			// temporary data storage 
		vector <char> objects; 	// will be used for new game initialisation
		int width  = 0;
		int height = 0;

		sprintf( route, "./examples/maps/map%d", maps_menu.option() );

		map.open( route, std::fstream::in );
		getline(map,data); // first line - name, discard

		while ( getline(map,data) ) // while data still available
		{

			int new_width = 0;
			for ( const auto & object : data )
			{
				if ( object != ' ' && object != '~' && object != '#' ) 		// if object isn't an empty space, wall or water..
					throw new invalidObject();

				objects.push_back(object);	// if valid, insert it to vector

				new_width++;
			}

			if ( width == 0 ) // if width not yet initialised
				width = new_width; // initialise
			else if ( new_width != width )	// else check if width sizes corelate
				throw new mapInconsistent(); // if not - throw exception

			height++;

		}

		if ( height < 9 )
			throw new invalidMapSize();

		if ( height%2 != 1 ) // if map height is not an odd number
			throw new invalidMapSize();

		if ( (height * 4) > width ) // if height is larger than 1/4 of the width
			throw new invalidMapSize();

		// << map vlidity check 


		// Just a safety feature. If custom map
		// had no enter and exit, it was now.
		objects[width*(height/2)] 		= ' ';
		objects[width*((height/2)+1)-1] = ' ';
		
		game.init(objects, difficulty, width, height); // initialise the map with user's custom map

		clear();
		screen_print("Map loaded successfully", CENTER);
		hard_delay_s(1);

		return true;

}




















// -------------------------------------- MAIN --------------------------------------




int main( int argc, const char * argv[] ) 
{

	init();
	
	GAME game;

	splashScreen();


// MAIN PROGRAM LOOP
while( true )
{

		switch( GLOBAL_STATE )
		{	

			case MENU:
				mainMenu(); 
			break;

			case SETTINGS:
				clear();
				screen_print("To be added..", CENTER);
				GLOBAL_STATE = MENU;
				hard_delay_s(1);
			break;

			case NEW_GAME:
				try { if ( mapSelection( game ) ) GLOBAL_STATE = ACTIVE; }
				catch( const invalidMapException * e )
					{ e -> print(); delete e; }
			break;

			case GAME_LOAD:
				try { game.load(); } 
				catch( const LoadFailException * e )
						{ e -> print(); delete e; GLOBAL_STATE = MENU; break; };
				clear(); screen_print("Load successful", CENTER); hard_delay_s(2);
				GLOBAL_STATE = ACTIVE;
			break;

			case ACTIVE:
				if ( game.state() == GAME::GAME_OVER ) 
					GLOBAL_STATE = MENU;
				else
					game.run();
			break;


			case EXIT:
				clear();
				if ( confirmMenu( "Exit Game?") )
					return ExitGame();

				else GLOBAL_STATE = MENU;
			break;

			default:
				clear();
				screen_print("How did you get here?", CENTER);
				GLOBAL_STATE = MENU;
			break;
		}
}

    return 0;

}
