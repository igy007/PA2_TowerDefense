#include "towers.h"
#include <fstream>



/* 
    Tower values - might be defined from file
          or used with default values
*/
short watch_tower_price     = _DEF_WATCHTOWER_PRICE;
short oil_tower_price       = _DEF_OILTOWER_PRICE;
short fire_tower_price      = _DEF_FIRETOWER_PRICE;
short wall_price            = _DEF_WALL_PRICE;

short watch_tower_damage    = _DEF_WATCHTOWER_DMG;
short oil_tower_damage      = _DEF_OILTOWER_DMG;
short fire_tower_damage     = _DEF_FIRETOWER_DMG;

short watch_tower_range     = _DEF_WATCHTOWER_RANGE;
short oil_tower_range       = _DEF_OILTOWER_RANGE;
short fire_tower_range      = _DEF_FIRETOWER_RANGE;

short watch_tower_fire_rate = _DEF_WATCHTOWER_FR;
short oil_tower_fire_rate   = _DEF_OILTOWER_FR;
short fire_tower_fire_rate  = _DEF_FIRETOWER_FR;


/*
    Reads the tower config file and assigns
       values for future initialisation
*/
void readConfigTowers( bool load_from_file )
{

    // user can use define directive at the start of main
    // to effectively turn on/off this feature
    if ( !load_from_file ) 
        return;

    std::ifstream config;
    std::string line;

    config.open( "./examples/config_t", std::fstream::in );

    if ( !config.is_open() ) // file unavailable
        return;

    getline(config, line);  // WATCH TOWER
    config >> watch_tower_price;getline(config, line);
    config >> watch_tower_damage;getline(config, line);
    config >> watch_tower_range;getline(config, line);
    config >> watch_tower_fire_rate;getline(config, line);
    
    getline(config, line);  // OIL TOWER
    config >> oil_tower_price;getline(config, line);
    config >> oil_tower_damage;getline(config, line);
    config >> oil_tower_range;getline(config, line);
    config >> oil_tower_fire_rate;getline(config, line);

    getline(config, line);  // FIRE TOWER
    config >> fire_tower_price;
    config >> fire_tower_damage;
    config >> fire_tower_range;
    config >> fire_tower_fire_rate;

}





/*
    -------------------------------------------- CONSTRUCTORS --------------------------------------------
*/



CTower::CTower( short x, short y, std::vector<CEnemy*> * new_en_p ) 
    : CObject( x, y, CObject::OBSTACLE ), cooldown(0), enemies(new_en_p) {}


watch_tower::watch_tower( short x, short y, std::vector<CEnemy*> * new_en_p ) : single_shot( x, y, new_en_p )
{
    object_chars = {'W'};
    damage = watch_tower_damage;
    color = WHITE_PAIR;
    range = watch_tower_range;
    fire_rate = watch_tower_fire_rate;
    price = watch_tower_price;
    special = 0;
}


oil_tower::oil_tower( short x, short y, std::vector<CEnemy*> * new_en_p ) : spread_shot( x, y, new_en_p )
{
    object_chars = {'O'};
    damage = oil_tower_damage;
    color = YELLOW_PAIR;
    range = oil_tower_range;
    fire_rate = oil_tower_fire_rate;
    price = oil_tower_price;
    special = 1;
}


fire_tower::fire_tower( short x, short y, std::vector<CEnemy*> * new_en_p ) : single_shot( x, y, new_en_p )
{
    object_chars = {'F'};
    damage = fire_tower_damage;
    color = ORANGE_PAIR;
    range = fire_tower_range;
    fire_rate = fire_tower_fire_rate;
    price = fire_tower_price;
    special = 2;

}


wall::wall( short x, short y ) : CObject( x, y, OBSTACLE, WHITE_PAIR )
{ 
        object_chars = {'#'}; 
        price = wall_price; 
}











/*
    ----------------------------------------- ACTION DEFINITION -----------------------------------------
*/



void single_shot::action( void )
{

    if ( cooldown )
    {
        cooldown--;
        return;
    }


    CEnemy * most_progressive_enemy = nullptr; // the one closest to the destination

    std::vector<CEnemy*> enemies_in_sight; // list of enemies in sight

    for ( const auto &e : *enemies ) // if any enemy..
        if ( (e -> getX()) <= (X + range) && (e -> getX()) >= (X - range) ) // ..is in range on X axys..
            if ( (e -> getY()) <= (Y + range) && (e -> getY()) >= (Y - range) ) // ..and in range on Y axys..
                enemies_in_sight.push_back(e); // add to the list of enemies in sight


    for ( const auto &enemy : enemies_in_sight )
    {
        
        if ( most_progressive_enemy == nullptr ) // if no enemy yet most progressive
            most_progressive_enemy = enemy; // make it

        if ( (most_progressive_enemy -> getY()) < (enemy -> getY()) ) // if more progressive enemy found
            most_progressive_enemy = enemy; // is now the most progressive so far

    }

    if ( most_progressive_enemy != nullptr ) 
    {
        most_progressive_enemy -> takeDamage( damage, special ); // TODO - special
        cooldown = fire_rate; // shots fired!
    }

        

}



void spread_shot::action( void )
{

    if ( cooldown )
    {
        cooldown--;
        return;
    }

    for ( const auto &e : *enemies ) // if any enemy..
        if ( (e -> getX()) <= (X + range) && (e -> getX()) >= (X - range) ) // ..is in range on X axys..
            if ( (e -> getY()) <= (Y + range) && (e -> getY()) >= (Y - range) ) // ..and in range on Y axys..
            {
                e -> takeDamage( damage, special ); // TODO - special
                cooldown = fire_rate;
            }

}



// ------------------------------------------------------------------------------------------------------



