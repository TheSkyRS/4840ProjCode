#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdbool.h>

// === 地图常量 ===
#define MAP_WIDTH 40  // tile 横向数量
#define MAP_HEIGHT 30 // tile 纵向数量
#define TILE_SIZE 16  // 每个 tile 像素大小（单位：px）

// === Tile 类型 ===
typedef enum
{
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_FIRE = 2,
    TILE_WATER = 3,
    TILE_GOAL = 4,

    TILE_SLOPE_L_UP = 5, // 地面：左高右低（\）
    TILE_SLOPE_R_UP = 6, // 地面：右高左低（/）

    TILE_CEIL_R = 7, // 天花板：左高右低（\）
    TILE_CEIL_L = 8  // 天花板：左低右高（/）
} tile_type_t;

// === 外部地图数组（只读）===
extern const int tilemap[MAP_HEIGHT][MAP_WIDTH];

// === Tile 碰撞检测函数 ===
// 检查给定区域是否碰到 "墙壁" tile
bool is_tile_blocked(float x, float y, float width, float height);

int get_tile_at_pixel(float x, float y);
#endif // TILEMAP_H
