// tilemap.h
// 地形图（Tile Map）模块头文件

#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdbool.h>

// === 地图常量定义 ===
#define MAP_WIDTH 40  // 地图宽度（单位：tile）
#define MAP_HEIGHT 30 // 地图高度（单位：tile）
#define TILE_SIZE 16  // 每格 tile 的像素尺寸

// Tile 编码约定（根据地图设计者定义）
#define TILE_EMPTY 0 // 可通行
#define TILE_WALL 1  // 实体墙体（不可通行）
#define TILE_FIRE 2  // 火池（火男孩可通过，水女孩死亡）
#define TILE_WATER 3 // 水池（水女孩可通过，火男孩死亡）
#define TILE_EXIT 4  // 出口终点

// 地图数据数组（由 tilemap.c 定义）
extern const int tilemap[MAP_HEIGHT][MAP_WIDTH];

/**
 * 检查指定矩形区域是否碰撞任何阻挡 tile
 *
 * 参数：
 * - x, y          ：左上角坐标（单位：像素）
 * - width, height ：待检测区域大小
 * 返回值：true 表示有碰撞，false 表示无碰撞
 */
bool is_tile_blocked(float x, float y, float width, float height);

#endif // TILEMAP_H
