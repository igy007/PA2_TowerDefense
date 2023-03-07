
#include "game.h"
#include "NCExtension.h"
#include "exceptions.h"
#include "CObject.h"
#include "towers.h"

#include <ncurses.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <fstream>


void GAME::init 
(
	std::vector<char> map,
	short difficulty, 
	short new_map_width, 
	short new_map_height
)
{

	if ( difficulty < 1 ) // default value, shouldn't happen tho
	{
		clear();
		screen_print("Invalid difficulty! Aborting", CENTER);
		hard_delay_s(3);
		return;
	}


	// CHECK MAP SIZE INTEGRITY
	if ( new_map_width < MIN_MAP_WIDTH || new_map_height < MIN_MAP_HEIGHT || new_map_height > (new_map_width/4) )
		throw invalidMapSize();


	// individual value definitions..
	 game_difficulty	= difficulty;
	 map_width  		= new_map_width;
	 map_height 		= new_map_height;
	 money 	   			= DEFAULT_START_MONEY;
	 monster_points		= DEFAULT_START_MP;
	 lives 				= DEFAULT_LIFE_COUNT;
	 round 				= 1;
	// ------------------------------


	// >> MAP INITIALISATION
	for ( int i = 0; i < new_map_width; ++i )
    {
        map_objects.push_back( std::vector<CObject *> () ); // new column of map objects
        for ( int j = 0; j < new_map_height; ++j )
            map_objects[i].push_back( nullptr );
    }
    // << MAP INITIALISED

	// if map data were provided
    if ( !map.empty() ) 
    {
    	
    	for ( int x = 0; x < new_map_width; ++x )
    	{
        	for ( int y = 0; y < new_map_height; ++y )
            {
            	if ( map[x+(y*new_map_width)] == '#' )
            		map_objects[x][y] = new wall(x,y);

            	if ( map[x+(y*new_map_width)] == '~' )
            		map_objects[x][y] = new water(x,y);
            }
    	}

    	game_initialised = true; 	// init done
		game_state = TOWER_BUILD;	// let's start buildin'

		return;

    }

    // create basic borders from walls
	generateBorders( new_map_width, new_map_height );

    // water is added to the map
    generateWater( new_map_width, new_map_height );
   

    // INIT COMPLETE - CONFIRM
    game_initialised = true;

    game_state = TOWER_BUILD;

}


void GAME::display( void )
{

	if ( !game_initialised ) // why would you even..?
		return;

	clear();

	for ( int y = 0; y < map_height; ++y )
	{			// for each row
		for ( int x = 0; x < map_width; ++x )
		{		// ..and each column
			if ( map_objects[x][y] != nullptr ) // if not empty
				 color_print( map_objects[x][y] -> getChar(), x, y, map_objects[x][y] -> getColor(), NO_REF );		// print the object
		}
	}



	screen_print( "Active enemies:", map_width + 1, 0, NO_REF );

	int row = 0; // for active enemies print
	// print all the enemies
	for ( const auto & enemy : active_enemies ) // more convenient
	{
		color_print( enemy -> getChar(), enemy -> getX(), enemy -> getY(), enemy -> getColor(), NO_REF );
			screen_print( enemy -> name(), map_width + 1, row + 1, NO_REF );
			screen_print( " (",  NO_REF );
			screen_print( enemy -> getHP(), NO_REF );
			screen_print( ")",  NO_REF );
		row++;			
	}
	

	//color_print( "«",  		      0, (map_height/2), RED_PAIR, NO_REF );
	//color_print( "«", (map_width-1), (map_height/2), RED_PAIR, NO_REF );

	
	// prints round number at the top of the screen
	screen_print( "Round ", map_width/2 - 3, 0, NO_REF ); // - 4 to compensate for "Round XX" lenght
	screen_print( round, NO_REF );


	screen_print( "Money: ", 3, map_height, NO_REF ); // - 4 to compensate for "Round XX" lenght
	screen_print( money, NO_REF );

	screen_print( "Lives left: ", map_width-16, map_height, NO_REF ); // - 4 to compensate for "Round XX" lenght
	screen_print( lives, NO_REF );

	refresh();

}



void GAME::generateWater( short _map_width, short _map_height )
{

	short fail_safe; // prevents getting stuck in an infinite loop, if water gets generated very unfortunatelly..

	short maximum_water_count = (_map_width * _map_height) / 60; // ~3% of playfield will be covered in water at most

	short new_water_count = (rand() % maximum_water_count) + (_map_width * _map_height) / 30; // final water count

	std::vector< std::pair<int, int> > water_positions;
 

	// water seed generating loop
	while ( new_water_count > 0 )
	{

		fail_safe = 100;
		water_positions.clear();

		// this is new seed
		short new_x_pos = (rand() % (_map_width - 4)) + 3;	// water will be placed at least 2 blocks from start and dest
		short new_y_pos = rand() % _map_height;

		if ( map_objects[new_x_pos][new_y_pos] != nullptr ) // if position already occupied
				continue; // find new position

		water_positions.push_back( std::make_pair(new_x_pos, new_y_pos) );


		// how many water blocks to be created next to each other
		short neighbour_count = (rand() % new_water_count) + 3;

		// water sections generating loop
		while ( neighbour_count > 0 )
		{
			
			if ( fail_safe-- <= 0 ) break; 

			new_x_pos = (rand() % 3) - 1;	// -1, 0 or +1 X?
			new_y_pos = (rand() % 3) - 1; 	// -1, 0 or +1 Y?

			if ( (new_x_pos != 0) && (new_y_pos != 0) ) // if none of them is zero ( we create direct neighbours )
				continue;

			std::pair <short,short> parent;

			if ( water_positions.size() > 1 )	// if more water blocks to choose from..
				parent = water_positions[ rand() % (water_positions.size()-1) ]; // ..parent water node lotto
			else
				parent = water_positions[0]; // if there's only one water block.. we know which one to choose..

			new_x_pos = parent.first  + new_x_pos;
			new_y_pos = parent.second + new_y_pos;

			if ( new_x_pos < 3 || new_x_pos > (map_width - 4) ) // at least 2 blocks from start and finish
				continue;

			if ( map_objects[new_x_pos][new_y_pos] != nullptr ) // if position already occupied
				continue; // find new position

			map_objects[new_x_pos][new_y_pos] = new water( new_x_pos, new_y_pos ); // place it onto the map
			new_water_count-- ;
			fail_safe = 100; // we successfully added new water, safe to continue

			water_positions.push_back( std::make_pair(new_x_pos, new_y_pos) ); // and take a record of the newly generated water position

			neighbour_count-- ;	 // decrease te number of neghbours to be created

		}

		water_positions.clear(); // room for new water blocks

	}

}



void GAME::generateBorders( short _map_width, short _map_height )
{

	for ( int i = 0; i < _map_width; ++i )
	{
		map_objects[i][0] 				= new wall( i , 0 );
		map_objects[i][(_map_height-1)] = new wall( i, (_map_height -1) );
	}

	for ( int j = 1; j < (_map_height-1); ++j ) // starts form 1, because there already is one wall from previous loop
	{
		if ( j == _map_height / 2 ) continue; // creatures' start and finish position

		map_objects[0][j] 				= new wall( 0 , j );
		map_objects[(_map_width-1)][j]  = new wall( (_map_width-1), j );
	}

}


short GAME::state( void )
{
	return game_state;
}


void GAME::run( void )
{

	if ( !game_initialised )
		return;

	switch(game_state)
	{

		case RUNNING:
			
			if ( getch() == 27 ) // ESC
				{ game_state = PAUSED; break; }

			display();
			hard_delay_ms(200);
			display();
			hard_delay_ms(200);
			update();

		break;

		case DISP_ROUND:
			clear();
			screen_print( "Round", 0, (screenHeight()/2) - 6, CENTER | NO_REF );
			printNewLine();
			screen_print( round, MIDDLE );
			hard_delay_s(2);
			game_state = RUNNING;
			break;

		case TOWER_BUILD:
			towerBuilder();
		break;

		case PAUSED:
			pauseMenu();
		break;

	}


}


void GAME::update( void )
{

	static int monster_distance = 0; // space between newly spawned monsters

	if ( !game_initialised ) // .. what should it do when it's not initialised?!
		return;


	// >> CHECK IF ANY ENEMY REACHED DEADLY HP VALUE
	for ( int i = 0; i < active_enemies.size(); ++i ) // for each enemy..
	{
		// ENEMY KILLED
		if  ( active_enemies[i] -> getHP() <= 0 )
		{
			money += active_enemies[i] -> getPrice()/5; // player gets 1/5 price of the enemy unit
			delete active_enemies[i]; // free it's allocated space
			active_enemies.erase(active_enemies.begin()+i); // ..and remove from active enemies list
		}
	}
	// << CHECK IF ANY ENEMY REACHED DEADLY HP VALUE


	// >> CHECK IF ENEMY GOT PAST THE DESTINATION
	for ( int i = 0; i < active_enemies.size(); ++i ) // for each enemy..
	{
		// ENEMY MANAGED TO REACH THE DESTINATION
		if ( active_enemies[i] -> getX() == 0 && active_enemies[i]-> getY() == (map_height/2) ) 
		{
			lives--; // loose a life point
			delete active_enemies[i]; // free it's allocated space
			active_enemies.erase(active_enemies.begin()+i); // ..and remove from active enemies list
		}
	}
	// << CHECK IF ENEMY GOT PAST THE DESTINATION
	

	// calls action on every object on the map
	for ( int y = 0; y < map_height; ++y )
	{			// for each row
		for ( int x = 0; x < map_width; ++x )
		{		// ..and each column
			if ( map_objects[x][y] != nullptr ) // if not empty
				 map_objects[x][y] -> action();
		}
	}


	// calls action on every enemy in the game
	if ( !active_enemies.empty() ) // if enemies exist
	{
		for ( const auto & e : active_enemies )
			e -> action();
	}


	// >> GAME OVER
	if ( lives <= 0 ) // if no more life-points left - GAME OVER
	{
		clear();
		screen_print("GAME OVER", CENTER);
		hard_delay_s(2);
		screen_print("You survived:", CENTER | NO_REF );
		printNewLine();
		screen_print( (round - 1), MIDDLE | NO_REF );
		printNewLine();
		screen_print( "rounds", MIDDLE );
		monster_distance = 0;
		hard_delay_s(2);

		cleanUP();
		return;
	}
	// <<


	// >> ROUND FINISHED
	if ( (monster_points <= 0) && active_enemies.empty() ) // if no more life-points left - GAME OVER
	{
		nextRound();
		return;
	}
	// <<

	
	// >> MONSTER SPAWN SECTION
	if ( (monster_points > 0) && !monster_distance )
	{
		active_enemies.push_back( spawnMonster() );
		monster_distance = (rand()%6) + 3; // randomly generated distance between spawned monsters (3-8)
	}

	if (monster_distance)
		monster_distance--;

}


void GAME::buildTower( short x, short y )
{

	short pos = 0;

	std::vector <std::string> towers = { "Watch Tower", "Oil Tower", "Fire Tower", "Wall" };
	std::vector <CObject*> _tower_p = 
		{ 
			new watch_tower(x,y, &active_enemies), 
			new oil_tower(x,y, &active_enemies), 
			new fire_tower(x,y, &active_enemies), 
			new wall(x,y) 
		};

	
	// PRINTING CUSTOM BACKGROUND WINDOW, BECAUSE NCURSES API IS $#/+ !
	 screen_print("+-----------------------+", (map_width/2-12), (map_height/2)-towers.size()/2-1, NO_REF );
	 color_print("Money: ", (map_width/2-5), (map_height/2)-towers.size()/2-1, ORANGE_PAIR, NO_REF );
	 color_print(money, ORANGE_PAIR, NO_REF ); // prints actual money balance
	 for ( int j = 0; j < towers.size()+1; ++j )
		screen_print("|                       |", (map_width/2-12), (map_height/2)-towers.size()/2+j, NO_REF );

	 screen_print("+-----------------------+", (map_width/2-12), cursorPosY()+1, NO_REF );
	// ====================================================================

	bool tower_selected = false;

	while ( !tower_selected )
	{

		for ( int i = 0; i < towers.size(); ++i )
		{
			screen_print( towers[i].c_str(), (map_width/2-10), (map_height/2)-(towers.size()/2)+i, ( i == pos ) * HIGHLIGHT );
			screen_print( " (" );
			
			if ( _tower_p[i] -> getPrice() <= money )
				color_print( _tower_p[i] -> getPrice(), GREEN_PAIR, NO_REF );
			else
				color_print( _tower_p[i] -> getPrice(), RED_PAIR );

			screen_print( ")" );
		}

		screen_print( "CANCEL", (map_width/2-10), cursorPosY()+1, ( pos == towers.size() ) * HIGHLIGHT );
		refresh();

		while ( !inputPresent() ) { hard_delay_ms(10); }

		switch( getch() )
		{
			case KEY_UP:
			pos--; if ( pos < 0 ) pos = towers.size();
			break;

			case KEY_DOWN:
			pos++; pos %= towers.size()+1; // -1 missing because of CANCEL
			break;

			case 10: // enter
			tower_selected = true;
			break;

			default:
			break;
		}
	}

	if ( pos == towers.size() ) // if CANCEL was chosen
	{
		for ( const auto &x : _tower_p ) // set all towers free
			delete x;
		return;
	}

	if ( _tower_p[pos] -> getPrice() > money ) // if picked tower is too expensive
	{
		for ( const auto &x : _tower_p ) // set all towers free
			delete x;

		beep(); // alert
		return; 
	}

	map_objects[x][y] = _tower_p[pos];

	money -= _tower_p[pos] -> getPrice();

	for ( int j = 0; j < _tower_p.size(); ++j )
		if ( j != pos ) delete _tower_p[j]; // free all towers except the one chosen

	_tower_p.clear();

}


void GAME::towerBuilder( void )
{

	static short cursor_X = map_width/2;
	static short cursor_Y = map_height/2;

	clear();

	// print all objects on the map
	for ( int y = 0; y < map_height; ++y )
	{			// for each row
		for ( int x = 0; x < map_width; ++x )
		{		// ..and each column
			if ( map_objects[x][y] != nullptr ) // if not empty
				 color_print( map_objects[x][y] -> getChar(), x, y, map_objects[x][y] -> getColor(), NO_REF );
		}
	}


	// >> CURSOR PRINT
	if ( (cursor_X == 1) || (cursor_X == map_width-2) || (cursor_Y == 1) || (cursor_Y == map_height-2) ) // can't build next to borders
		color_print( "X", cursor_X, cursor_Y, RED_PAIR, NO_REF ); 
	else if ( map_objects[cursor_X][cursor_Y] == nullptr ) // if tower can be build 
		color_print( "X", cursor_X, cursor_Y, GREEN_PAIR, NO_REF );
	else	// ..if not
		color_print( map_objects[cursor_X][cursor_Y] -> getChar(), cursor_X, cursor_Y, DENIED_PAIR, NO_REF );
	// << 
	
	screen_print( "Round ", map_width/2 - 3, 0, NO_REF );
	screen_print( round, NO_REF );
	screen_print( "PRESS SPACE TO START", map_width/2-10, map_height, NO_REF );

	while( !inputPresent() ) { hard_delay_ms(10); }


	switch ( getch() )
	{

		case KEY_UP:
		if ( cursor_Y > 1 ) cursor_Y--;
		break;

		case KEY_DOWN:
		if ( cursor_Y < (map_height-2) ) cursor_Y++;
		break;

		case KEY_LEFT:
		if ( cursor_X > 1 ) cursor_X--;
		break;


		case KEY_RIGHT:
		if ( cursor_X < (map_width-2) ) cursor_X++;
		break;

		case 10: // enter
		if ( (cursor_X == 1) || (cursor_X == map_width-2) || (cursor_Y == 1) || (cursor_Y == map_height-2) )
			{ beep(); break; } // can't build next to borders
		if ( map_objects[cursor_X][cursor_Y] == nullptr )
			buildTower( cursor_X, cursor_Y );
		else
			beep();
		break;

		case 27: // ESC
		if ( confirmMenu( "Return to main menu?" ) )
		{
			cursor_Y = map_height/2;
			cursor_X = map_width/2;
			cleanUP();
		}
		break; 

		case ' ':
			cursor_Y = map_height/2;
			cursor_X = map_width/2;
			game_state = DISP_ROUND;
		break; 
	}

	refresh();

}


void GAME::cleanUP( void )
{

	// delete all enemies
	for ( const auto &enemy : active_enemies )
		delete enemy;

	// dele all map objects
	for ( int y = 0; y < map_height; ++y ) // for each row..
	{			
		for ( int x = 0; x < map_width; ++x ) // ..and each column
			delete map_objects[x][y]; // free allocated space
	}

	active_enemies.clear(); // why not..
	map_objects.clear(); 	// why not..

	game_state = GAME_OVER;

	game_initialised = false;	// this game has nothing to offer now.. don't try to run it!

}


void GAME::pauseMenu( void )
{

	static CMENU pause_menu( "GAME PAUSED", {"Resume", "Exit to Menu"} );

	int option_selected = -1;

	pause_menu.highlightPrint();

	while ( !inputPresent() ) { hard_delay_ms(10); }

	switch( getch() )
	{	

		case KEY_UP:
			pause_menu.prev();
		break;

		case KEY_DOWN:
			pause_menu.next();
		break;

		case 10: // 10 == ENTER
			option_selected = pause_menu.option();
		break; // change state

		default:
		break;

	}

	switch ( option_selected )
	{

		case -1:
			pauseMenu();
		break;

		case 0:
			game_state = RUNNING;
		return;

		case 1:
		pause_menu.prev();
		if ( confirmMenu( "Are you sure? (all progress will be lost)" ) )
			cleanUP();
		else
			break;

	}

}


void GAME::nextRound( void )
{

	clear();
	screen_print("Round", 0, screenHeight()/2-2, MIDDLE | NO_REF );
	printNewLine();
	screen_print( round, MIDDLE | NO_REF );
	printNewLine();
	screen_print( "completed", MIDDLE );
	hard_delay_s(2);

	monster_points = DEFAULT_START_MP + (DEFAULT_START_MP/2) * round;
	round++;

	clear();
	screen_print("Game saved", CENTER);
	hard_delay_s(1);
	save();

	game_state = TOWER_BUILD;

}


CEnemy * GAME::spawnMonster( void )
{

	CEnemy * new_monster = nullptr;

	// EXPANDABLE
	while ( true ) // untill new monster is created
	{

		// DRAGON:  ~3% chance of spawn if at least 3rd round - increasingly higher chance after 10th round
		if ( (round >= 5) && ( !(rand()%33) || (rand()%round) > 10 ) ) 
		{ 
			new_monster = new dragon( map_width-1, map_height/2, map_objects );
			break;
		}

		// GARGOYLE
		if ( (round >= 3) && !(rand()%12) ) // ~8% chance of spawn if at least 3rd round
		{ 
			new_monster = new gargoyle( map_width-1, map_height/2, map_objects );
			break;
		}

		// TROOP
		if ( !(rand()%2) || !(rand()%10) ) // ~60% chance of spawn
		{ 
			new_monster = new troop( map_width-1, map_height/2, map_objects );
			break;
		}

		// MURLOC 
		if ( !(rand()%2) ) // ~50% chance of spawn
		{ 
			new_monster = new murloc( map_width-1, map_height/2, map_objects );
			break;
		}

	}

	monster_points -= new_monster -> getPrice(); // subtract price of the monster

	return new_monster;

}

bool GAME::load()
{

	std::ifstream savefile;
	int length, desired_size;
	int checksum = 0;
	char tmp; 

	savefile.open("./examples/save/savegame", std::ifstream::binary | std::ifstream::in); // tries to open savefile

	if ( !savefile.is_open() )
		throw new savegameNotPresent();	


	// get the length of savefile
	savefile.seekg (0, savefile.end);
    length = savefile.tellg();
    savefile.seekg (0, savefile.beg);
    
	if ( length == 0 )
    	throw new emptySaveFile();

    desired_size			=
    	sizeof(map_width) 	+
    	sizeof(map_height)  ; 	// so far, just to read width and heigt to determine final desired size
    	
    if ( length < desired_size )
    	throw new incompleteSaveFile();


	// load map WIDTH and HEIGHT
	savefile.read( (char*)&map_width,  sizeof(map_width)  );  // width
	savefile.read( (char*)&map_height, sizeof(map_height) );  // height

	desired_size 				+= 	
    	sizeof(money) 			+
    	sizeof(lives)			+
    	sizeof(round)			+
    	sizeof(game_difficulty) +
    	sizeof(checksum)		;

    	desired_size += ( sizeof(char) * map_width * map_height ); // final desired size (without checksum)

   
    if ( length < desired_size )
		throw new incompleteSaveFile();


	// >> checksum validation
    int pos_bckp = savefile.tellg(); // backup the current position
    savefile.seekg(length-sizeof(int), savefile.beg); // go find the saved checksum
	savefile.read( (char*)&checksum, sizeof(int) ); // read it
	savefile.seekg(0, savefile.beg); // go to the start of the file
	for (int i = 0; i < length-sizeof(int); ++i) // for each byte but the last few ones for the checksum..
	{
		savefile.read( (char*)&tmp, sizeof(char) );
		checksum -= ( tmp & 37 ); //.. subtract their last bit form the checksum
	}
	if ( checksum != 0 ) // and if checksum is not 0 in the end
		throw new checksumError(); // throw an enception

	savefile.seekg(pos_bckp, savefile.beg); // return to the backed-up position
	// << 


	savefile.read( (char*)&money,  			 sizeof(money)  );
	savefile.read( (char*)&lives,  			 sizeof(lives) 	);
	savefile.read( (char*)&round,  			 sizeof(round)  );
	savefile.read( (char*)&game_difficulty,  sizeof(game_difficulty) );


	for ( int i = 0; i < map_width; ++i )
    {
        map_objects.push_back( std::vector<CObject *> () ); // new column of map objects
        for ( int j = 0; j < map_height; ++j )
            map_objects[i].push_back( nullptr );
    }

	// lead the map!
	for ( int y = 0; y < map_height; ++y )
	{
		for ( int x = 0; x < map_width; ++x )
		{

			savefile.read( &tmp, sizeof(char) );

			switch (tmp)
			{

				case 0:
					map_objects[x][y] = nullptr;
					break;

				case '#':
					map_objects[x][y] = new wall(x,y);
					break;

				case '~':
					map_objects[x][y] = new water(x,y);
					break;

				case 'W':
					map_objects[x][y] = new watch_tower(x, y, &active_enemies);
					break;

				case 'O':
					map_objects[x][y] = new oil_tower(x, y, &active_enemies);
					break;

				case 'F':
					map_objects[x][y] = new fire_tower(x, y, &active_enemies);
					break;

				default: // unknown object detected! clean up all allocated objects and abort
				{
					cleanUP();
					throw new invalidData();
				}

			}
		}
	}

		
	monster_points = DEFAULT_START_MP + (DEFAULT_START_MP/2) * round;
	game_initialised = true;
	game_state = TOWER_BUILD;

	return true;

}


bool GAME::save( void )
{

	std::fstream savefile; 

	char tmp, zero = 0;
	int checksum = 0;

	// opens the file for r/w and erases all content
	savefile.open("./examples/save/savegame", 
		std::ofstream::binary | 
		std::ofstream::trunc  | 
		std::fstream::in 	  | 
		std::fstream::out	  );


	// write game stats to the file
	savefile.write( (char*)(&map_width), 		sizeof(map_width)		);
	savefile.write( (char*)(&map_height), 		sizeof(map_height)		);
	savefile.write( (char*)(&money), 			sizeof(money)			);
	savefile.write( (char*)(&lives), 			sizeof(lives)			);
	savefile.write( (char*)(&round), 			sizeof(round)			);
	savefile.write( (char*)(&game_difficulty), 	sizeof(game_difficulty)	);


	// write all the map objects to the file
	for ( int y = 0; y < map_height; ++y )
	{
		for ( int x = 0; x < map_width; ++x )
		{
			if ( map_objects[x][y] == nullptr ) 	// if nothing on the given location..
				savefile.write( (char*)&zero, sizeof(char)); 	// ..save zero
			else
			{
				tmp = map_objects[x][y] -> getChar();
				savefile.write( reinterpret_cast<const char *>(&tmp), sizeof(char) ); 		
			}
		}

	}

	// >> Generating checksum
	savefile.seekg (0, savefile.end);
    int length = savefile.tellg();
    savefile.seekg (0, savefile.beg);

    for ( int i = 0; i < length; ++i )
    {
    	savefile.read( (char*)(&tmp), sizeof(char));
    	checksum += ( tmp & 37 ); // use 37 as the checksum constant - pretty accurate and space efficient
    }

    savefile.write( (char*)(&checksum), sizeof(checksum) );
    // <<


	return true;

}





