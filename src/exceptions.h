
#include "NCExtension.h"

#include <ncurses.h>
#include <iostream>


struct CException
{
	virtual void print( void ) const = 0;
	virtual ~CException(void) = default;
};




struct invalidMapException : CException
{
	virtual void print( void ) const = 0;
};



struct invalidMapSize : invalidMapException
{
		void print( void ) const
		{
			clear();
			// "magic constants" just to compensate for the text length
			screen_print("Invalid Map Size!", screenWidth()/2-9, screenHeight()/2-3,  NO_REF ); 
			screen_print("Height: odd number ( at least 9 )",  cursorPosX() - 24, cursorPosY()+2, NO_REF );
			screen_print("Width: at least 4x the height", cursorPosX() - 31, cursorPosY()+1, NO_REF );
			refresh();
			hard_delay_s(3);
		};
};


struct invalidObject : invalidMapException
{
		void print( void ) const
		{
			clear();
			screen_print("Invalid map! (unknown object present)", CENTER ); 
			hard_delay_s(2);
		};
};


struct mapInconsistent : invalidMapException
{
	void print( void ) const
	{
		clear();
		screen_print("Invalid map shape!", CENTER ); 
		hard_delay_s(2);
	};
};







// -- -- - -- - - -- -- -  -- - - - - - - 
struct LoadFailException : CException
{
	virtual void print( void ) const = 0;
};




struct savegameNotPresent : LoadFailException
{
	void print( void ) const
	{
		clear();
		screen_print("No savegame available", CENTER ); 
		hard_delay_s(2);
	};
};

struct emptySaveFile : LoadFailException
{
	void print( void ) const
	{
		clear();
		screen_print("Save file empty. Returning to menu..", CENTER ); 
		hard_delay_s(2);
	};
};


struct incompleteSaveFile : LoadFailException
{
	void print( void ) const
	{
		clear();
		screen_print("Invalid savefile! (too few information)", CENTER ); 
		hard_delay_s(2);
	};
};


struct invalidData : LoadFailException
{
	void print( void ) const
	{
		clear();
		screen_print("Savefile corrupted! (invalid data)", CENTER ); 
		hard_delay_s(2);
	};
};


struct checksumError : LoadFailException
{
	void print( void ) const
	{
		clear();
		screen_print("Invalid savefile! (checksum error)", CENTER ); 
		hard_delay_s(2);
	};
};

