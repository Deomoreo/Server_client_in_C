#pragma once
#include <SDL.h>
#include <math.h>
typedef struct vec2
{
    float x;
    float y;
} vec2_t;

typedef struct player
{
    int ID;
    vec2_t position,old_position,latest_position;
    float lerp_accumulator;
    SDL_Rect SDL_Rect;
    unsigned int number_of_lifes;
    unsigned int number_of_bombs;
    unsigned int score;
    float speed;
    float speedX;
    float speedY;
    float speedXnegative;
    float speedYnegative;
    SDL_Texture* texture;
} player_t;

typedef struct bomb
{
    vec2_t position;
    unsigned int range;
    unsigned int cooldown;
} bomb_t;

enum bonus_type
{
    NONE = 0,
    SPEED,
    BOMB2,
    BOMB3
};

enum enemy_type
{
    BAT,
    BALOON,
};

typedef struct enemy
{
    vec2_t position;
    enum enemy_type type;
} enemy_t;

typedef struct game_mode
{
    unsigned int timer;
} game_mode_t;

typedef struct eggbomb
{
    int ID;
    SDL_Rect SDL_Rect_bomb;
    vec2_t position;
    int state;
    struct eggbomb_s* next;
    SDL_Texture* texture;
} eggbomb_t;


vec2_t vector2_lerp(vec2_t a,vec2_t b,float coeff)
{
    float r1 = a.x + (b.x - a.x)*coeff;
    float r2 = a.y + (b.y - a.y)*coeff;
    vec2_t* result = (vec2_t*)calloc(1,sizeof(vec2_t));
    result->x = r1;
    result->y = r2;
    return *result;
}
vec2_t Vec2_towards(vec2_t start,vec2_t end)
{
    vec2_t start_to_end;
    start_to_end.x = end.x - start.x;
    start_to_end.y = end.y - start.y;
    return start_to_end;
}
vec2_t Vec2_multiply(vec2_t vector,float coeff)
{
    vec2_t multiplied;
    multiplied.x = vector.x *coeff;
    multiplied.y = vector.y *coeff;
    return multiplied;
}
vec2_t Vec2_add(vec2_t dest,vec2_t plus)
{
    vec2_t result;
    result.x = dest.x + plus.x;
    result.y = dest.y + plus.y;
    return result;
}
float Vec2_magnitude(vec2_t vector)
{
   return 0+(float)sqrt((vector.x * vector.x)+(vector.y*vector.y));
}
void Normalized2(float* x, float* y)
{
    float magnitude = sqrt((*x) * (*x)+(*y)*(*y));
    *x /= magnitude;
    *y /= magnitude;
    if(*x != *x)
    {
        *x = 0;
    }
    if(*y != *y)
    {
        *y =0;
    }
}
void Vec2_Normalize(vec2_t* ToNormalize)
{
    Normalized2(&ToNormalize->x,&ToNormalize->y);
    //float magnitude = Vec2_magnitude(*ToNormalize);
    //vec2_t result;
    //magnitude = 1/ magnitude;
    //ToNormalize->x = ToNormalize->x*  magnitude;
    //ToNormalize->y = ToNormalize->y*  magnitude;
    
}
