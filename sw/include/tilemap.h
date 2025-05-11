#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdbool.h>

// === 地图常量 ===
#define MAP_WIDTH 40  // tile 横向数量
#define MAP_HEIGHT 30 // tile 纵向数量
#define TILE_SIZE 16  // 每个 tile 像素大小（单位：px）

// === Tile 类型枚举 ===
typedef enum
{
    TILE_EMPTY = 0, // 空地
    TILE_WALL = 1,  // 墙壁
    TILE_FIRE = 2,  // 火池
    TILE_WATER = 3, // 水池
    TILE_GOAL = 4,  // 终点

    TILE_SLOPE_L_UP = 5, // 斜坡（左高右低）
    TILE_SLOPE_R_UP = 6, // 斜坡（右高左低）

    TILE_CEIL_R = 7, // 天花板（左高右低）
    TILE_CEIL_L = 8  // 天花板（右高左低）

} tile_type_t;

// === 外部地图数组（只读）===
extern const int tilemap[MAP_HEIGHT][MAP_WIDTH];

// === Tile 碰撞检测函数 ===
// 检查给定区域是否碰到 "阻挡" tile（墙壁或不可穿越斜面）
// 修改函数签名，新增 vx 参数用于判断移动方向
float is_tile_blocked(float x, float y, float width, float height, float vx);

tile_type_t tilemap_get_type_at(float x, float y);

#endif // TILEMAP_H
