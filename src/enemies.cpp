
#include "enemies.h"
#include <stack>
#include <fstream>

/*
*	WARNING:
*	Extremely redundant code ahead!
*	Might cause cancer!
*	Proceed at your own risk!
*/


// mark true if you wanna see graphically represented
// randomised BFS path finding for walking/swimming enemies
const bool showBFS = false;




/* 
    Tower values - might be defined from file
          or used with default values
*/
int   troop_HP     	 = _DEF_TROOP_HP;
short troop_price    = _DEF_TROOP_PRICE;
short troop_speed    = _DEF_TROOP_SPEED;

int   murloc_HP      = _DEF_MURLOC_HP;
short murloc_price   = _DEF_MURLOC_PRICE;
short murloc_speed   = _DEF_MURLOC_SPEED;

int   gargoyle_HP    = _DEF_GARGOYLE_HP;
short gargoyle_price = _DEF_GARGOYLE_PRICE;
short gargoyle_speed = _DEF_GARGOYLE_SPEED;

int   dragon_HP    	 = _DEF_DRAGON_HP;
short dragon_price 	 = _DEF_DRAGON_PRICE ;
short dragon_speed 	 = _DEF_DRAGON_SPEED;



/*
    Reads the enemy config file and assigns
       values for future initialisation
*/
void readConfigEnemies( bool load_from_file )
{

    // user can use define directive at the start of main
    // to effectively turn on/off this feature
    if ( !load_from_file ) 
        return;

    std::ifstream config;
    std::string line;

    config.open( "./examples/config_e", std::fstream::in );

    if ( !config.is_open() ) // file unavailable
        return;

    getline(config, line);  // TROOP
    config >> troop_HP;getline(config, line);
    config >> troop_price;getline(config, line);
    config >> troop_speed;getline(config, line);
    
    getline(config, line);  // MURLOC
    config >> murloc_HP;getline(config, line);
    config >> murloc_price;getline(config, line);
    config >> murloc_speed;getline(config, line);

	getline(config, line);  // GARGOYLE
    config >> gargoyle_HP;getline(config, line);
    config >> gargoyle_price;getline(config, line);
    config >> gargoyle_speed;getline(config, line);

    getline(config, line);  // DRAGON
    config >> dragon_HP;
    config >> dragon_price;
    config >> dragon_speed;

}








/*
	CONSTRUCTORS
*/


CEnemy::CEnemy( short new_x, short new_y, std::vector< std::vector<CObject*> > & map )
	: CObject( new_x, new_y ), slowed(false), on_fire(false) {}


walking_enemy::walking_enemy( short new_x, short new_y, std::vector< std::vector<CObject*> > & map ) 
	: CEnemy( new_x, new_y, map )
{

	short x = new_x; 
	short y = new_y;
	int iteration = 0;	// keeps track of 
	bool found = false;		// flag - if destination found

	field = &map;

	std::vector< std::vector< std::pair<bool, int> > > visited; // holds info about visited nodes
	std::queue< std::pair<short,short> > pending; 	// also will be filled with final path coordinates
	std::stack< std::pair<short,short>> tmp_stack;  // will serve as middle man for data transfer

	if ( x > (map.size()-1) || x < 0 || y > (map[0].size()-1) || y < 0 )
		return; // invalid construction coordinates!!!

	// >> visited vector initialisation
	for ( int i = 0; i < map.size(); ++i )
    {
        visited.push_back( std::vector< std::pair<bool, int> > () ); // new column of map objects
        for ( int j = 0; j < map[0].size(); ++j )
            visited[i].push_back( std::make_pair(false,-1) );
    }
	// <<


	if ( (map[x-1][y] != nullptr) && (map[x-1][y] -> getType() != WATER) ) // if the first block s blocking..
	{
		path = pending; // path will be empty
		return;
	}
	else
	{
		visited[x][y].first = true;
		visited[x][y].second = iteration++;
		visited[x-1][y].first = true;
		visited[x-1][y].second = iteration++;
		pending.push( std::make_pair((x-1),y) );
	}
		
		
	// directions will be generated randomly - to make individual's BFS path unique		
	int xr;	// random X direction
	int yr;	// random Y direction


	while( !pending.empty() )	// until we check neighbours for every possible
	{

		if ( showBFS )
		{
		 for ( int f = 0; f < map.size(); ++f )
		 	for ( int t = 0; t < map[0].size(); ++t)
				if ( visited[f][t].first == true )screen_print('X', f, t, NO_REF );
		 refresh();
		}

		x = pending.front().first;
		y = pending.front().second;
		pending.pop();

		if ( showBFS )
		{
			color_print("X", x, y, GREEN_PAIR); // tohle x ted prohlizi okoli
			hard_delay_ms(6);
		}

		if ( x == 0 )
		{
			found = true; 	// we found the destionation!! YEAH!
			while ( !pending.empty() ) pending.pop(); // pending.clear() doesnt exist:((
			pending.push( std::make_pair(x,y) );
			break;
		}

		xr = rand()%2;
		yr = rand()%2;

		if ( xr == yr ) // if both are the same, start with X coordinates first
		{
			if ( xr ) // if xr is 1, first go right, then left
			{
				// if we can step on and haven't visited yet
				if ( (map[x+1][y] == nullptr) && (visited[x+1][y].first == false) ) 
				{
					pending.push( std::make_pair((x+1),y) );
					visited[x+1][y].first = true;
					visited[x+1][y].second = iteration++;
				}

				if ( (map[x-1][y] == nullptr) && (visited[x-1][y].first == false) )
				{
					pending.push( std::make_pair((x-1),y) );
					visited[x-1][y].first = true;
					visited[x-1][y].second = iteration++;
				}

				if ( yr ) // firt up or down?
				{
					if ( (map[x][y+1] == nullptr) && (visited[x][y+1].first == false) )
					{
						pending.push( std::make_pair(x,(y+1)) );
						visited[x][y+1].first = true;
						visited[x][y+1].second = iteration++;
					}

					if ( (map[x][y-1] == nullptr) && (visited[x][y-1].first == false) )
					{
						pending.push( std::make_pair(x,(y-1)) );
						visited[x][y-1].first = true;
						visited[x][y-1].second = iteration++;
					}
				}
				else
				{
					if ( (map[x][y-1] == nullptr) && (visited[x][y-1].first == false) ) 
					{
						pending.push( std::make_pair(x,(y-1)) );
						visited[x][y-1].first = true;
						visited[x][y-1].second = iteration++;
					}

					if ( (map[x][y+1] == nullptr) && (visited[x][y+1].first == false) ) 
					{
						pending.push( std::make_pair(x,(y+1)) );
						visited[x][y+1].first = true;
						visited[x][y+1].second = iteration++;
					}
				}
			}
			else // if xr is 0, first go left, then right
			{
				if ( (map[x-1][y] == nullptr) && (visited[x-1][y].first == false) ) 
				{
					pending.push( std::make_pair((x-1),y) );
					visited[x-1][y].first = true;
					visited[x-1][y].second = iteration++;
				}
				if ( (map[x+1][y] == nullptr) && (visited[x+1][y].first == false) )
				{
					pending.push( std::make_pair((x+1),y) );
					visited[x+1][y].first = true;
					visited[x+1][y].second = iteration++;
				}
				if ( yr ) // firt up or down?
				{
					if ( (map[x][y+1] == nullptr) && (visited[x][y+1].first == false) ) 
					{
						pending.push( std::make_pair(x,(y+1)) );
						visited[x][y+1].first = true;
						visited[x][y+1].second = iteration++;
					}
					if ( (map[x][y-1] == nullptr) && (visited[x][y-1].first == false) )
					{
						pending.push( std::make_pair(x,(y-1)) );
						visited[x][y-1].first = true;
						visited[x][y-1].second = iteration++;
					}
				}
				else
				{
					if ( (map[x][y-1] == nullptr) && (visited[x][y-1].first == false) ) 
					{
						pending.push( std::make_pair(x,(y-1)) );
						visited[x][y-1].first = true;
						visited[x][y-1].second = iteration++;
					}
					if ( (map[x][y+1] == nullptr) && (visited[x][y+1].first == false) )
					{
						pending.push( std::make_pair(x,(y+1)) );
						visited[x][y+1].first = true;
						visited[x][y+1].second = iteration++;
					}
				}
			}	
		}
		else
		{
			if ( yr ) // if yr is 1, first go UP, then DOWN
			{
				// if we can step on and haven't visited yet
				if ( (map[x][y+1] == nullptr) && (visited[x][y+1].first == false) ) 
				{
					pending.push( std::make_pair(x,(y+1)) );
					visited[x][y+1] = std::make_pair(true, iteration);
					iteration++;
				}
				if ( (map[x][y-1] == nullptr) && (visited[x][y-1].first == false) ) 
				{
					pending.push( std::make_pair(x,(y-1)) );
					visited[x][y-1].first = true;
					visited[x][y-1].second = iteration++;
				}
				if ( xr ) // firt left or right?
				{
					if ( (map[x-1][y] == nullptr) && (visited[x-1][y].first == false) )
					{
						pending.push( std::make_pair((x-1),y) );
						visited[x-1][y].first = true;
						visited[x-1][y].second = iteration++;
					}
					if ( (map[x+1][y] == nullptr) && (visited[x+1][y].first == false) )
					{
						pending.push( std::make_pair((x+1),y) );
						visited[x+1][y].first = true;
						visited[x+1][y].second = iteration++;
					}
				}
				else	
				{
					if ( (map[x+1][y] == nullptr) && (visited[x+1][y].first == false) ) 
					{
						pending.push( std::make_pair((x+1),y) );
						visited[x+1][y].first = true;
						visited[x+1][y].second = iteration++;
					}
					if ( (map[x-1][y] == nullptr) && (visited[x-1][y].first == false) ) 
					{
						pending.push( std::make_pair((x-1),y) );
						visited[x-1][y].first = true;
						visited[x-1][y].second = iteration++;
					}
				}
			}
			else
			{
				if ( (map[x][y-1] == nullptr) && (visited[x][y-1].first == false) ) 
				{
					pending.push( std::make_pair(x,(y-1)) );
					visited[x][y-1].first = true;
					visited[x][y-1].second = iteration++;
				}
				if ( (map[x][y+1] == nullptr) && (visited[x][y+1].first == false) )
				{
					pending.push( std::make_pair(x,(y+1)) );
					visited[x][y+1].first = true;
					visited[x][y+1].second = iteration++;
				}
				if ( xr ) // firt left or right?
				{
					if ( (map[x-1][y] == nullptr) && (visited[x-1][y].first == false) )
					{
						pending.push( std::make_pair((x-1),y) );
						visited[x-1][y].first = true;
						visited[x-1][y].second = iteration++;
					}
					if ( (map[x+1][y] == nullptr) && (visited[x+1][y].first == false) ) 
					{
						pending.push( std::make_pair((x+1),y) );
						visited[x+1][y].first = true;
						visited[x+1][y].second = iteration++;
					}
				}
				else	
				{
					if ( (map[x+1][y] == nullptr) && (visited[x+1][y].first == false) ) 
					{
						pending.push( std::make_pair((x+1),y) );
						visited[x+1][y].first = true;
						visited[x+1][y].second = iteration++;
					}
					if ( (map[x-1][y] == nullptr) && (visited[x-1][y].first == false) ) 
					{
						pending.push( std::make_pair((x-1),y) );
						visited[x-1][y].first = true;
						visited[x-1][y].second = iteration++;
					}	
				}
			}	
		}
	}


	if ( !found ) 	// if our poor monster can not reach the destionation, return empty path
	{
		while ( !pending.empty() ) pending.pop(); // pending.clear() doesnt exist:((
		path = pending; 
		return;
	}

	// if found, there is already the destination in the queue
	while ( true )
	{
		int smallest = 9999;
		std::pair<short, short> next;
		x = pending.back().first;
		y = pending.back().second;

		if ( ( x != (map.size()-1) ) && visited[x+1][y].first == true ) // if we visited given coordinate..
		{
			if ( visited[x+1][y].second < smallest ) // ..if is the oldest visit so far
			{
				next = std::make_pair((x+1), y);	// make note
				smallest = visited[x+1][y].second;
			}
		}
		if ( (x != 0 ) && visited[x-1][y].first == true )
		{
			if ( visited[x-1][y].second < smallest )
			{
				next = std::make_pair((x-1), y);
				smallest = visited[x-1][y].second;
			}
		}
		if ( ( y != (map[0].size()-1) ) && visited[x][y+1].first == true )
		{
			if ( visited[x][y+1].second < smallest )
			{
				next = std::make_pair(x, (y+1));
				smallest = visited[x][y+1].second;
			}
		}
		if ( ( y != 0 ) && visited[x][y-1].first == true )
		{
			if ( visited[x][y-1].second < smallest )
			{
				next = std::make_pair(x, (y-1));
				smallest = visited[x][y-1].second;
			}
		}

		pending.push(next);

		if ( showBFS )
		{
			color_print('X',next.first, next.second, GREEN_PAIR );
			hard_delay_ms(30);
		}

		if ( (pending.back().first == (new_x-1)) && (pending.back().second == new_y) )
			break;

	}

	// reversing the queue and flling the path with newly acquired data
    while ( !pending.empty() ) 
    { 
        tmp_stack.push( pending.front() ); 
        pending.pop(); 
    } 
    while ( !tmp_stack.empty() ) 
    { 
        path.push( tmp_stack.top() ); 
        tmp_stack.pop(); 
    } 

}



swimming_enemy::swimming_enemy( short new_x, short new_y, std::vector< std::vector<CObject*> > & map ) 
	: CEnemy( new_x, new_y, map ) 
{

	short x = new_x; 
	short y = new_y;
	int iteration = 0;	// keeps track of 
	bool found = false;		// flag - if destination found

	field = &map;

	std::vector< std::vector< std::pair<bool, int> > > visited; // holds info about visited nodes
	std::queue< std::pair<short,short> > pending; 	// also will be filled with final path coordinates
	std::stack< std::pair<short,short>> tmp_stack;  // will serve as middle man for data transfer

	if ( x > (map.size()-1) || x < 0 || y > (map[0].size()-1) || y < 0 )
		return; // invalid construction coordinates!!!

	// >> visited vector initialisation
	for ( int i = 0; i < map.size(); ++i )
    {
        visited.push_back( std::vector< std::pair<bool, int> > () ); // new column of map objects
        for ( int j = 0; j < map[0].size(); ++j )
            visited[i].push_back( std::make_pair(false,-1) );
    }
	// <<


	if ( (map[x-1][y] != nullptr) && (map[x-1][y] -> getType() != WATER) ) // if the first block s blocking..
	{
		path = pending; // path will be empty
		return;
	}
	else
	{
		visited[x][y].first = true;
		visited[x][y].second = iteration++;
		visited[x-1][y].first = true;
		visited[x-1][y].second = iteration++;
		pending.push( std::make_pair((x-1),y) );
	}
		
		
	// directions will be generated randomly - to make individual's BFS path unique		
	int xr;	// random X direction
	int yr;	// random Y direction


	while( !pending.empty() )	// until we check neighbours for every possible
	{

		if ( showBFS )
		{
		 for ( int f = 0; f < map.size(); ++f )
		 	for ( int t = 0; t < map[0].size(); ++t)
				if ( visited[f][t].first == true )screen_print('X', f, t, NO_REF );
		 refresh();
		}

		x = pending.front().first;
		y = pending.front().second;
		pending.pop();

		if ( showBFS )
		{
			color_print("X", x, y, GREEN_PAIR); // tohle x ted prohlizi okoli
			hard_delay_ms(6);
		}

		if ( x == 0 )
		{
			found = true; 	// we found the destionation!! YEAH!
			while ( !pending.empty() ) pending.pop(); // pending.clear() doesnt exist:((
			pending.push( std::make_pair(x,y) );
			break;
		}

		xr = rand()%2;
		yr = rand()%2;

		if ( xr == yr ) // if both are the same, start with X coordinates first
		{
			if ( xr ) // if xr is 1, first go right, then left
			{
				// if we can step on and haven't visited yet
				if ( (map[x+1][y] == nullptr || map[x+1][y] -> getType() == WATER) && (visited[x+1][y].first == false) ) 
				{
					pending.push( std::make_pair((x+1),y) );
					visited[x+1][y].first = true;
					visited[x+1][y].second = iteration++;
				}

				if ( (map[x-1][y] == nullptr || map[x-1][y] -> getType() == WATER) && (visited[x-1][y].first == false) )
				{
					pending.push( std::make_pair((x-1),y) );
					visited[x-1][y].first = true;
					visited[x-1][y].second = iteration++;
				}

				if ( yr ) // firt up or down?
				{
					if ( (map[x][y+1] == nullptr || map[x][y+1] -> getType() == WATER) && (visited[x][y+1].first == false) )
					{
						pending.push( std::make_pair(x,(y+1)) );
						visited[x][y+1].first = true;
						visited[x][y+1].second = iteration++;

					}

					if ( (map[x][y-1] == nullptr || map[x][y-1] -> getType() == WATER) && (visited[x][y-1].first == false) )
					{
						pending.push( std::make_pair(x,(y-1)) );
						visited[x][y-1].first = true;
						visited[x][y-1].second = iteration++;
					}
				}
				else
				{
					if ( (map[x][y-1] == nullptr || map[x][y-1] -> getType() == WATER) && (visited[x][y-1].first == false) ) 
					{
						pending.push( std::make_pair(x,(y-1)) );
						visited[x][y-1].first = true;
						visited[x][y-1].second = iteration++;
					}

					if ( (map[x][y+1] == nullptr || map[x][y+1] -> getType() == WATER) && (visited[x][y+1].first == false) ) 
					{
						pending.push( std::make_pair(x,(y+1)) );
						visited[x][y+1].first = true;
						visited[x][y+1].second = iteration++;
					}
				}
			}
			else // if xr is 0, first go left, then right
			{
				if ( (map[x-1][y] == nullptr || map[x-1][y] -> getType() == WATER) && (visited[x-1][y].first == false) ) 
				{
					pending.push( std::make_pair((x-1),y) );
					visited[x-1][y].first = true;
					visited[x-1][y].second = iteration++;
				}
				if ( (map[x+1][y] == nullptr || map[x+1][y] -> getType() == WATER) && (visited[x+1][y].first == false) )
				{
					pending.push( std::make_pair((x+1),y) );
					visited[x+1][y].first = true;
					visited[x+1][y].second = iteration++;
				}
				if ( yr ) // firt up or down?
				{
					if ( (map[x][y+1] == nullptr || map[x][y+1] -> getType() == WATER) && (visited[x][y+1].first == false) ) 
					{
						pending.push( std::make_pair(x,(y+1)) );
						visited[x][y+1].first = true;
						visited[x][y+1].second = iteration++;
					}
					if ( (map[x][y-1] == nullptr || map[x][y-1] -> getType() == WATER) && (visited[x][y-1].first == false) )
					{
						pending.push( std::make_pair(x,(y-1)) );
						visited[x][y-1].first = true;
						visited[x][y-1].second = iteration++;
					}
				}
				else
				{
					if ( (map[x][y-1] == nullptr || map[x][y-1] -> getType() == WATER) && (visited[x][y-1].first == false) ) 
					{
						pending.push( std::make_pair(x,(y-1)) );
						visited[x][y-1].first = true;
						visited[x][y-1].second = iteration++;
					}
					if ( (map[x][y+1] == nullptr || map[x][y+1] -> getType() == WATER) && (visited[x][y+1].first == false) )
					{
						pending.push( std::make_pair(x,(y+1)) );
						visited[x][y+1].first = true;
						visited[x][y+1].second = iteration++;
					}
				}
			}	
		}
		else
		{
			if ( yr ) // if yr is 1, first go UP, then DOWN
			{
				// if we can step on and haven't visited yet
				if ( (map[x][y+1] == nullptr || map[x][y+1] -> getType() == WATER) && (visited[x][y+1].first == false) ) 
				{
					pending.push( std::make_pair(x,(y+1)) );
					visited[x][y+1] = std::make_pair(true, iteration);
					iteration++;
				}
				if ( (map[x][y-1] == nullptr || map[x][y-1] -> getType() == WATER) && (visited[x][y-1].first == false) ) 
				{
					pending.push( std::make_pair(x,(y-1)) );
					visited[x][y-1].first = true;
					visited[x][y-1].second = iteration++;
				}
				if ( xr ) // firt left or right?
				{
					if ( (map[x-1][y] == nullptr || map[x-1][y] -> getType() == WATER) && (visited[x-1][y].first == false) )
					{
						pending.push( std::make_pair((x-1),y) );
						visited[x-1][y].first = true;
						visited[x-1][y].second = iteration++;
					}
					if ( (map[x+1][y] == nullptr || map[x+1][y] -> getType() == WATER) && (visited[x+1][y].first == false) )
					{
						pending.push( std::make_pair((x+1),y) );
						visited[x+1][y].first = true;
						visited[x+1][y].second = iteration++;
					}
				}
				else	
				{
					if ( (map[x+1][y] == nullptr || map[x+1][y] -> getType() == WATER) && (visited[x+1][y].first == false) ) 
					{
						pending.push( std::make_pair((x+1),y) );
						visited[x+1][y].first = true;
						visited[x+1][y].second = iteration++;
					}
					if ( (map[x-1][y] == nullptr || map[x-1][y] -> getType() == WATER) && (visited[x-1][y].first == false) ) 
					{
						pending.push( std::make_pair((x-1),y) );
						visited[x-1][y].first = true;
						visited[x-1][y].second = iteration++;
					}
				}
			}
			else
			{
				if ( (map[x][y-1] == nullptr || map[x][y-1] -> getType() == WATER) && (visited[x][y-1].first == false) ) 
				{
					pending.push( std::make_pair(x,(y-1)) );
					visited[x][y-1].first = true;
					visited[x][y-1].second = iteration++;
				}
				if ( (map[x][y+1] == nullptr || map[x][y+1] -> getType() == WATER) && (visited[x][y+1].first == false) )
				{
					pending.push( std::make_pair(x,(y+1)) );
					visited[x][y+1].first = true;
					visited[x][y+1].second = iteration++;
				}
				if ( xr ) // firt left or right?
				{
					if ( (map[x-1][y] == nullptr || map[x-1][y] -> getType() == WATER) && (visited[x-1][y].first == false) )
					{
						pending.push( std::make_pair((x-1),y) );
						visited[x-1][y].first = true;
						visited[x-1][y].second = iteration++;
					}
					if ( (map[x+1][y] == nullptr || map[x+1][y] -> getType() == WATER) && (visited[x+1][y].first == false) ) 
					{
						pending.push( std::make_pair((x+1),y) );
						visited[x+1][y].first = true;
						visited[x+1][y].second = iteration++;
					}
				}
				else	
				{
					if ( (map[x+1][y] == nullptr || map[x+1][y] -> getType() == WATER) && (visited[x+1][y].first == false) ) 
					{
						pending.push( std::make_pair((x+1),y) );
						visited[x+1][y].first = true;
						visited[x+1][y].second = iteration++;
					}
					if ( (map[x-1][y] == nullptr || map[x-1][y] -> getType() == WATER) && (visited[x-1][y].first == false) ) 
					{
						pending.push( std::make_pair((x-1),y) );
						visited[x-1][y].first = true;
						visited[x-1][y].second = iteration++;
					}	
				}
			}	
		}
	}


	if ( !found ) 	// if our poor monster can not reach the destionation, return empty path
	{
		while ( !pending.empty() ) pending.pop(); // pending.clear() doesnt exist:((
		path = pending; 
		return;
	}

	// if found, there is already the destination in the queue
	while ( true )
	{
		int smallest = 9999;
		std::pair<short, short> next;
		x = pending.back().first;
		y = pending.back().second;

		if ( ( x != (map.size()-1) ) && visited[x+1][y].first == true ) // if we visited given coordinate..
		{
			if ( visited[x+1][y].second < smallest ) // ..if is the oldest visit so far
			{
				next = std::make_pair((x+1), y);	// make note
				smallest = visited[x+1][y].second;
			}
		}
		if ( (x != 0 ) && visited[x-1][y].first == true )
		{
			if ( visited[x-1][y].second < smallest )
			{
				next = std::make_pair((x-1), y);
				smallest = visited[x-1][y].second;
			}
		}
		if ( ( y != (map[0].size()-1) ) && visited[x][y+1].first == true )
		{
			if ( visited[x][y+1].second < smallest )
			{
				next = std::make_pair(x, (y+1));
				smallest = visited[x][y+1].second;
			}
		}
		if ( ( y != 0 ) && visited[x][y-1].first == true )
		{
			if ( visited[x][y-1].second < smallest )
			{
				next = std::make_pair(x, (y-1));
				smallest = visited[x][y-1].second;
			}
		}

		pending.push(next);

		if ( showBFS )
		{
			color_print('X',next.first, next.second, BLUE_PAIR );
			hard_delay_ms(30);
		}

		if ( (pending.back().first == (new_x-1)) && (pending.back().second == new_y) )
			break;

	}
	

	// reversing the queue and flling the path with newly acquired data
    while ( !pending.empty() ) 
    { 
        tmp_stack.push( pending.front() ); 
        pending.pop(); 
    } 
    while ( !tmp_stack.empty() ) 
    { 
        path.push( tmp_stack.top() ); 
        tmp_stack.pop(); 
    } 

}



flying_enemy::flying_enemy( short new_x, short new_y, std::vector< std::vector<CObject*> > & map ) 
	: CEnemy( new_x, new_y, map ) 
{
	field = &map;
}


// ----- individual subclass constructors


troop::troop( short new_x, short new_y, std::vector< std::vector<CObject*> > & map ) 
	: walking_enemy( new_x, new_y, map )
{
	HP = troop_HP;
	speed = troop_speed;
	cooldown = speed;
	object_chars = {'i'};
	color = GRAY_PAIR;
	price = troop_price;
}

murloc::murloc( short new_x, short new_y, std::vector< std::vector<CObject*> > & map ) 
	: swimming_enemy( new_x, new_y, map )
{
	HP = murloc_HP;
	speed = murloc_speed;
	cooldown = speed;
	object_chars = {'o','O'};
	color = GREEN_PAIR;
	price = murloc_price;
}


gargoyle::gargoyle( short new_x, short new_y, std::vector< std::vector<CObject*> > & map ) 
	: flying_enemy( new_x, new_y, map )
{
	HP = gargoyle_HP;
	speed = gargoyle_speed;
	cooldown = speed;
	object_chars = {'v','^'};
	color = GRAY_PAIR;
	price = gargoyle_price;
}


dragon::dragon( short new_x, short new_y, std::vector< std::vector<CObject*> > & map ) 
	: flying_enemy( new_x, new_y, map )
{
	HP = dragon_HP;
	speed = dragon_speed;
	cooldown = speed;
	object_chars = {'v','v','^','^'};
	color = RED_PAIR;
	price = dragon_price;
}













/*
	DAMAGE LOGIC
*/

/*
special:
oil  - reduces speed of the enemy, also takes down flying units
fire - burns the s**t out of enemies
else - ?
*/
void CEnemy::takeDamage( short raw_damage, int special = 0 )
{
	
	// include tower's special ability to enemy's handicaps
	if ( special & 1 )
		slowed = true;

	if ( special & 2 )
		on_fire = true;

	HP -= raw_damage; // TODO - special

}



void dragon::takeDamage( short raw_damage, int special = 0 ) 
{

	// if oil hits the dragon, dragon heals by ~1/2 it's power
	// if fire hits the dragon, it does nothing to it

	if ( special & 1 ) // if accack comes from oil tower
	{
		HP += (raw_damage+1)/2;
		return;
	}

	if ( special & 2 ) // if attack comes from fire tower
		return;

	HP -= raw_damage;

}



/*
	MOVEMENT
*/

void CEnemy::action( void )
{
	
	// fail safe if enemy gets 
	// stuck due to screen resize
	if ( cooldown > 100 ) 
		HP -= 100;

	if ( (X > 0) && (X < (field -> size()-1)) ) // watchout for out of vector reach
	{
		// if enemy gets close to the water, it is no longer on fire
		if ( (field[0][X][Y] != nullptr) && field[0][X][Y] -> getType() == WATER )
			on_fire = false;
		if ( (field[0][X+1][Y] != nullptr) && field[0][X+1][Y] -> getType() == WATER )
			on_fire = false;
		if ( (field[0][X-1][Y] != nullptr) && field[0][X-1][Y] -> getType() == WATER )
			on_fire = false;
		if ( (field[0][X][Y+1] != nullptr) && field[0][X][Y+1] -> getType() == WATER )
			on_fire = false;
		if ( (field[0][X][Y-1] != nullptr) && field[0][X][Y-1] -> getType() == WATER )
			on_fire = false;
	}


	// if monster on fire and covered in oil
	// at the same time -> every round -1 HP
	if ( slowed && on_fire )
	{
		HP --;
		if ( rand()%2 )
			return;
	}

	// while on fire - every round ~ 30% chance of getting burned
	if ( on_fire && rand()%3 )
		HP--;

	// reduces speed by ~50% (roughly.. very woughly..)
	if ( slowed && rand()%2 )
		return;
	
	move();

}



void walking_enemy::move( void )
{


	if ( cooldown > 1 )
	{
		cooldown--;
		return;
	}

	if ( !path.empty() )
	{
		X = path.front().first;
		Y = path.front().second;
		path.pop();
		cooldown = speed;
		return;
	}

	// >> random movement section - if user has blocked the way out
	int randomY = 1 + ((-2)*(rand()%2)); // 1 || -1

	if ( (X != 0) && field[0][X-1][Y] == nullptr )
		X--;

	else if ( ((Y + randomY) > 0) && ((Y + randomY) < field[0].size()-1) 
			&& (field[0][X][Y + randomY] == nullptr) )
		Y += randomY;

	else if ( (X != field->size()-1) && field[0][X+1][Y] == nullptr )
		X++;

	else return;

	HP--; // monster will die eventually.. 

	cooldown = speed;
	// << 

}



void swimming_enemy::move( void )
{

	if ( cooldown > 1 )
	{
		cooldown--;
		return;
	}

	if ( !path.empty() )
	{
		X = path.front().first;
		Y = path.front().second;
		path.pop();
		cooldown = speed;
		return;
	}

	// >> random movement section - if user has blocked the way out
	int randomY = 1 + ((-2)*(rand()%2)); // 1 || -1

	if ( (X != 0) && (( field[0][X-1][Y] == nullptr) || (field[0][X-1][Y]->getType() == WATER)) )
		X--;

	else if ( ((Y + randomY) > 0) && ((Y + randomY) < field[0].size()-1) 
			&& (field[0][X][Y+randomY] == nullptr 
				|| field[0][X][Y+randomY]->getType() == WATER) )
		Y += randomY;

	else if ( (X != field->size()-1) && field[0][X+1][Y] == nullptr )
		X++;
	else return;

	HP--; // monster will die eventually.. 

	cooldown = speed;
	// << 

}



void flying_enemy::move( void )
{
	X--;
}


const short CEnemy::getColor( void ) const
{

	// if both slowed and on fire, return 
	//       random handicap color
	if ( on_fire && slowed ) 
	{
		if ( rand()%2 )
			return ORANGE_PAIR;
		else
			return YELLOW_PAIR;
	}


	if ( on_fire )
		return ORANGE_PAIR;

	if ( slowed )
		return YELLOW_PAIR;

	return color;

}


