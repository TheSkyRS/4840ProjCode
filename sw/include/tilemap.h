#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdbool.h>
#include "sprite.h" // 若没有可添加包含 item_t 定义
#include "type.h"
typedef struct item_t item_t;

// === 外部地图数组（只读）===

// === Tile 碰撞检测函数 ===
// 检查给定区域是否碰到 "墙壁" tile
bool is_tile_blocked(float x, float y, float width, float height);

void item_place_on_tile(item_t *item, int tile_x, int tile_y);

int get_tile_at_pixel(float x, float y);
#endif // TILEMAP_H
