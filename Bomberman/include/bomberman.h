typedef struct vec2
{
    float x;
    float y;
} vec2_t;

typedef struct player
{
    vec2_t pos;
    unsigned int number_of_lifes;
    unsigned int number_of_bombs;   
    unsigned int score;
    float speed;
} player_t;

typedef struct bomb
{
    vec2_t pos;
    unsigned int range;
    unsigned int cooldown;
} bomb_t;

enum cell_type
{
    GROUND=0,
    WALL,
    UNDESTROYABLE_WALL,
    TELEPORT
};

enum bonus_type
{
    NONE=0,
    SPEED,
    BOMB2,
    BOMB3
};

enum enemy_type
{
    BAT,
    BALOON,
};

typedef struct cell
{
    enum cell_type type;
    enum bonus_type bonus;
} cell_t;

typedef struct enemy
{
    vec2_t pos;
    enum enemy_type type;
} enemy_t;

typedef struct game_mode
{
    float timer;
} game_mode_t;
