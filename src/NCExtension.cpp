#include "NCExtension.h"


// INITIAL SCREEN SIZE IF NOT ADEQUATE
const unsigned _INIT_SCREEN_W = 60;
const unsigned _INIT_SCREEN_H = 15;		
// -----------------------------------

// ------------- COLOR PAIRS -------------
void init_color_pairs( void )
{
	init_pair( RED_PAIR,	9,   COLOR_BLACK );
	init_pair( GREEN_PAIR,	10,  COLOR_BLACK );
	init_pair( BLUE_PAIR,	33,  COLOR_BLACK );
	init_pair( YELLOW_PAIR,	11,  COLOR_BLACK );
	init_pair( GRAY_PAIR,	250, COLOR_BLACK );
	init_pair( WHITE_PAIR,	15,	 COLOR_BLACK );
	init_pair( ORANGE_PAIR,	208, COLOR_BLACK );
	init_pair( DENIED_PAIR,	0, 9 );
}
// ---------------------------------------


unsigned __SCREEN_WIDTH, __SCREEN_HEIGHT;		// screen width 'n' height

unsigned __CURSOR_POS_X, __CURSOR_POS_Y; 		// cursor position

bool screen_initialised = false;



// probably unnecessary..
unsigned screenHeight( void ) { return __SCREEN_HEIGHT; }

unsigned screenWidth ( void ) { return __SCREEN_WIDTH;  }

unsigned cursorPosX  ( void ) { return __CURSOR_POS_X;  }
//void 	 cursorPosX  ( unsigned _new_pos ) { __CURSOR_POS_X = _new_pos };

unsigned cursorPosY  ( void ) { return __CURSOR_POS_Y;  }
//void 	 cursorPosY  ( unsigned _new_pos ) { __CURSOR_POS_Y = _new_pos };


void screenInit( void )
{
	
	__CURSOR_POS_X = 0;
	__CURSOR_POS_Y = 0;

	getmaxx(stdscr) >= _INIT_SCREEN_W ? ( __SCREEN_WIDTH  = getmaxx(stdscr) ) : ( __SCREEN_WIDTH  = _INIT_SCREEN_W );
	getmaxy(stdscr) >= _INIT_SCREEN_H ? ( __SCREEN_HEIGHT = getmaxy(stdscr) ) : ( __SCREEN_HEIGHT = _INIT_SCREEN_H );
	
	screen_initialised = true;

}



void screenResize(  unsigned __new_width_, unsigned __new_height_ )
{

	// new screen size must be at least same as initial size
	__new_width_  >= _INIT_SCREEN_W ? ( __SCREEN_WIDTH  = __new_width_  ) : ( __SCREEN_WIDTH  = _INIT_SCREEN_W );
	__new_height_ >= _INIT_SCREEN_H ? ( __SCREEN_HEIGHT = __new_height_ ) : ( __SCREEN_HEIGHT = _INIT_SCREEN_H );

}



void screen_print( const char * __CHAR_STR_ARR_, unsigned __X_CORD, unsigned __Y_CORD, int __ATTR_ )
{

	if ( !screen_initialised )
		return;

	int text_length = 0;

	// strlen(ch) workaround
	if ( __CHAR_STR_ARR_ != NULL )
		while (__CHAR_STR_ARR_[text_length]) text_length++; 

	if ( __ATTR_ & CENTER )
	{
		if ( ((int)( __SCREEN_WIDTH  / 2 ) - ( text_length / 2 )) < 0 )
			return; // text too long!!

		__X_CORD = ( __SCREEN_WIDTH  / 2 ) - ( text_length / 2 );
		__Y_CORD = ( __SCREEN_HEIGHT / 2 );
	}


	if ( __ATTR_ & MIDDLE )
		__X_CORD = ( __SCREEN_WIDTH  / 2 ) - ( text_length / 2 ) ;


	if ( __X_CORD >= __SCREEN_WIDTH ) // also works when text too long -> unsigned underflow
	{
		for ( int i = 0; i < __SCREEN_HEIGHT; ++i )
			mvprintw( i, (__SCREEN_WIDTH-1), ">" );
			refresh();

			return; // no point in printing outside the broders
	}


	if ( __ATTR_ & HIGHLIGHT )
		attron(A_REVERSE);


	for ( int i = 0; i < text_length ; ++i )
	{
		
		char TMP[2]; TMP[0] = __CHAR_STR_ARR_[i]; TMP[1] = '\0';

		// if there is more than 2 chars of text to be printed but only 2 chars free
		if ( (text_length - i) >= 2 && ((__SCREEN_WIDTH - __X_CORD) <= 2) ) 
		{
			mvprintw( __Y_CORD, __X_CORD, ".." );
			__X_CORD += 2;
			break;
		} 

		mvprintw( __Y_CORD, __X_CORD++, TMP ); 	// STANDARD NCURSES PRINT FUNCTION

	}


	__CURSOR_POS_X = __X_CORD;
	__CURSOR_POS_Y = __Y_CORD;


	attroff(A_REVERSE);


	if ( !(__ATTR_ & NO_REF) )
		refresh();

}



void screen_print( const char * __CHAR_STR_ARR_, int __ATTR_ )
{
	screen_print( __CHAR_STR_ARR_, __CURSOR_POS_X, __CURSOR_POS_Y, __ATTR_ );
}



void screen_print( char __CHAR_, int __ATTR_ )
{
	screen_print ( __CHAR_, __CURSOR_POS_X, __CURSOR_POS_Y, __ATTR_ );
}



void screen_print( char __CHAR_, unsigned __X_CORD, unsigned __Y_CORD, int __ATTR_ )
{

	if ( !screen_initialised )
		return;

	char _CharArr[2];
	
	_CharArr[0] = __CHAR_;
	_CharArr[1] = '\0';


	if ( __X_CORD >= __SCREEN_WIDTH ) 
		return; 


	if ( __ATTR_ & HIGHLIGHT )
		attron(A_REVERSE);


	if ( __ATTR_ & CENTER )
	{
		__X_CORD = ( __SCREEN_WIDTH  / 2 );
		__Y_CORD = ( __SCREEN_HEIGHT / 2 );
	}


	mvprintw( __Y_CORD, __X_CORD++, _CharArr ); 	// STANDARD NCURSES PRINT FUNCTION

	
	__CURSOR_POS_X = __X_CORD;
	__CURSOR_POS_Y = __Y_CORD;


	attroff(A_REVERSE);


	if ( !(__ATTR_ & NO_REF) )
		refresh();


}



void screen_print( const int __NUMBER_, unsigned __X_CORD, unsigned __Y_CORD, int __ATTR_ )
{

	char __NUM_CHAR_BUFF_ [20]; // 20 sounds enought to me

  	sprintf( __NUM_CHAR_BUFF_, "%d", __NUMBER_ );

  	screen_print( __NUM_CHAR_BUFF_, __X_CORD, __Y_CORD, __ATTR_ ); // already defined char string screen_print

}



void screen_print( const int __NUMBER_, int __ATTR_ )
{

	char __NUM_CHAR_BUFF_ [20]; // 20 sounds enought to me

  	sprintf( __NUM_CHAR_BUFF_, "%d", __NUMBER_ );

	screen_print( __NUM_CHAR_BUFF_, __CURSOR_POS_X, __CURSOR_POS_Y, __ATTR_ ); // already defined char string screen_print

}



void screen_print( const unsigned __NUMBER_, unsigned __X_CORD, unsigned __Y_CORD, int __ATTR_ )
{

	char __NUM_CHAR_BUFF_ [20]; // 20 sounds enought to me

  	sprintf( __NUM_CHAR_BUFF_, "%u", __NUMBER_ );

  	screen_print( __NUM_CHAR_BUFF_, __X_CORD, __Y_CORD, __ATTR_ ); // already defined char string screen_print

}



void screen_print( const unsigned __NUMBER_, int __ATTR_ )
{

	char __NUM_CHAR_BUFF_ [20]; // 20 sounds enought to me

  	sprintf( __NUM_CHAR_BUFF_, "%u", __NUMBER_ );

	screen_print( __NUM_CHAR_BUFF_, __CURSOR_POS_X, __CURSOR_POS_Y, __ATTR_ ); // already defined char string screen_print

}


// COLOR PRINT - COLOR PRINT - COLOR PRINT - COLOR PRINT - COLOR PRINT


void color_print( const char * __CHAR_STR_ARR_, unsigned __X_CORD, unsigned __Y_CORD, int USER_COLOR, int __ATTR_ )
{

	attron( COLOR_PAIR(USER_COLOR) );	

	// has to NO_REF even if NO_REF originally not set to avoid double-refresh
	screen_print (__CHAR_STR_ARR_, __X_CORD, __Y_CORD, __ATTR_ | NO_REF ); 

	attroff(COLOR_PAIR(USER_COLOR));

	if ( !(__ATTR_ & NO_REF) )
	refresh();

}


void color_print( const char * __CHAR_STR_ARR_, int USER_COLOR, int __ATTR_ )
{
	color_print( __CHAR_STR_ARR_, __CURSOR_POS_X, __CURSOR_POS_Y, USER_COLOR, __ATTR_ );
}



void color_print( char __CHAR_, unsigned __X_CORD, unsigned __Y_CORD, int USER_COLOR, int __ATTR_ )
{
	
	attron( COLOR_PAIR(USER_COLOR) );	

	// has to NO_REF even if NO_REF originally not set to avoid double-refresh
	screen_print ( __CHAR_, __X_CORD, __Y_CORD, __ATTR_ | NO_REF ); 

	attroff(COLOR_PAIR(USER_COLOR));

	if ( !(__ATTR_ & NO_REF) )
	refresh();

}



void color_print( char __CHAR_, int USER_COLOR, int __ATTR_ )
{
	color_print( __CHAR_, __CURSOR_POS_X, __CURSOR_POS_Y, USER_COLOR, __ATTR_ );
}



void color_print( const unsigned __NUMBER_, unsigned __X_CORD, unsigned __Y_CORD, int USER_COLOR, int __ATTR_ )
{
	
	attron( COLOR_PAIR(USER_COLOR) );	

	// has to NO_REF even if NO_REF originally not set to avoid double-refresh
	screen_print ( __NUMBER_, __X_CORD, __Y_CORD, __ATTR_ | NO_REF ); 

	attroff(COLOR_PAIR(USER_COLOR));

	if ( !(__ATTR_ & NO_REF) )
	refresh();

}



void color_print( const unsigned __NUMBER_, int USER_COLOR, int __ATTR_ )
{
	color_print( __NUMBER_, __CURSOR_POS_X, __CURSOR_POS_Y, USER_COLOR, __ATTR_ );
}



void color_print( const int __NUMBER_, unsigned __X_CORD, unsigned __Y_CORD, int USER_COLOR, int __ATTR_ )
{

	attron( COLOR_PAIR(USER_COLOR) );	

	// has to NO_REF even if NO_REF originally not set to avoid double-refresh
	screen_print ( __NUMBER_, __X_CORD, __Y_CORD, __ATTR_ | NO_REF ); 

	attroff(COLOR_PAIR(USER_COLOR));

	if ( !(__ATTR_ & NO_REF) )
	refresh();

}



void color_print( const int __NUMBER_, int USER_COLOR, int __ATTR_ )
{
	color_print( __NUMBER_, __CURSOR_POS_X, __CURSOR_POS_Y, USER_COLOR, __ATTR_ );
}



void printNewLine( void )
{
	__CURSOR_POS_Y++; // EZ PEASY
}



// ==================== INPUT MANIP ====================



bool inputPresent( void ) 
{

    int input = getch();

    if ( input != ERR ) 
    {
        ungetch( input );
        return true;
    } 
    
    return false;

}



// ==================== TIME HANDLING SECTION ====================


void hard_delay_ms( int __MILLI_S_ ) 
{ 
	usleep( 1000 * __MILLI_S_ );
} 

void hard_delay_s( int __SEC_COUNT_ ) 
{ 
    usleep( 1000000 * __SEC_COUNT_ );
} 


// ======================== CMENU ========================



CMENU::CMENU( const std::list<std::string> & menu_items ) : position(0)
{
	
	name = "";

    for ( const auto &x : menu_items )
    	items.push_back(x);

}

CMENU::CMENU( const std::string menu_name, const std::list<std::string> & menu_items ) : position(0)
{
	
	name = menu_name;

    for ( const auto &x : menu_items )
    	items.push_back(x);

}


void CMENU::print( void )
{
	return;
}


void CMENU::addOption( std::string text, int position )
{
	if ( position < 0 ) position = items.size(); // fail-safe 
	items.insert( items.begin()+position, text );
}

void CMENU::addOption( const char * text, int position )
{
	if ( position < 0 ) position = items.size(); // fail-safe
	items.insert( items.begin()+position, text );
}



/**
	prints the menu with every item aligned 
	to the center. Item that's currently
	marked is highlighted.
*/
void CMENU::highlightPrint( void )
{

	clear();
	
	if ( name != "" )
	{
		attron(A_UNDERLINE);
		screen_print( name.c_str(), screenWidth()/2 - name.length()/2-1, (screenHeight()/2)-(items.size()+1) );
		attroff(A_UNDERLINE);
	}

	for ( int i = 0; i < items.size(); ++i )
	{
		int pos = (screenHeight()/2)-(items.size()-i) + 1;

		if ( i == position )
			screen_print( items[i].c_str(), screenWidth()/2 - items[i].length()/2, pos, HIGHLIGHT );
		else
			screen_print( items[i].c_str(), screenWidth()/2 - items[i].length()/2, pos );
	}

}


/**
	prints the menu equaly stacked upon each other
	with an arrow next to the item that's currently
	marked
*/
void CMENU::arrowPrint( void )
{

	clear();

	if ( name != "" )
	{
		attron(A_UNDERLINE);
		screen_print( name.c_str(), screenWidth()/2 - name.length()/2 + 1, (screenHeight()/2)-(items.size()) );
		attroff(A_UNDERLINE);
	}


	for ( int i = 0; i < items.size(); ++i )
	{
		int pos = (screenHeight()/2)-(items.size()-i) + 2;

		if ( i == position )
		{
			screen_print( "> ", screenWidth()/2 - 4, pos);
			screen_print( items[i].c_str() );
		}
		else
		{
			screen_print( "  ", screenWidth()/2 - 4, pos);
			screen_print( items[i].c_str() );
		}
	}

}


/**
	Prints a simple YES/NO prompt.
	Returns true if YES selected, false if not.
*/
bool confirmMenu( std::string text )
{

	bool option = 0;
	int input;

	clear();

	while ( true )
	{
		screen_print( text.c_str(), __SCREEN_WIDTH/2 - text.length()/2, (__SCREEN_HEIGHT/2) - 2 );
		printNewLine();
		screen_print("YES", (__SCREEN_WIDTH/2)-4, cursorPosY(), (!option)*HIGHLIGHT );
		screen_print("   ");
		screen_print("NO", (option)*HIGHLIGHT );
		
		while ( !inputPresent() ) { hard_delay_ms(10); }
		input = getch();

		if ( input == KEY_LEFT || input == KEY_RIGHT )
			option = !option;

		if ( input == 10 ) // enter
			return !option; // inverted option - 1 would mean NO
	}

}