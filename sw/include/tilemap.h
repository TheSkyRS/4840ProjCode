#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdint.h>

#define TILE_SIZE 32
#define MAP_WIDTH 40
#define MAP_HEIGHT 30

typedef enum
{
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_FIRE = 2,
    TILE_WATER = 3,
    TILE_GOAL = 4,
    TILE_SLOPE_L_UP = 5,
    TILE_SLOPE_R_UP = 6,
    TILE_CEIL_R = 7,
    TILE_CEIL_L = 8
} tile_type_t;

extern const int tilemap[MAP_HEIGHT][MAP_WIDTH];
extern uint8_t collision_map[480][640]; // ✅ ← 加上这行
void collision_map_init();
float is_pixel_blocked(float x, float y, float width, float height);

#endif
