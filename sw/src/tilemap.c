// tilemap.c
// 地形图数据与 tile 碰撞检测实现

#include "tilemap.h"
#include <math.h> // 用于 floor()

// === 示例地图数据 ===
// 0: 空地  1: 墙壁  2: 火池  3: 水池  4: 终点
const int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    // 顶部边界一行墙体
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    // 中部区域
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // ...其余行填充空地
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // 底部边界一行墙体
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// === Tile 碰撞检测函数 ===
bool is_tile_blocked(float x, float y, float width, float height)
{
    int left = (int)floor(x / TILE_SIZE);
    int right = (int)floor((x + width - 1) / TILE_SIZE);
    int top = (int)floor(y / TILE_SIZE);
    int bottom = (int)floor((y + height - 1) / TILE_SIZE);

    for (int ty = top; ty <= bottom; ++ty)
    {
        for (int tx = left; tx <= right; ++tx)
        {
            // 越界视为阻挡
            if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT)
                return true;

            int tile = tilemap[ty][tx];
            if (tile == TILE_WALL)
                return true; // 与墙体发生碰撞
        }
    }
    return false; // 未发现碰撞
}

bool is_tile_blocked_precise(float x, float y, float width, float height)
{
    // 每条边检测3个点（左中右 / 上中下）
    float x_left = x;
    float x_center = x + width / 2.0f;
    float x_right = x + width - 1;

    float y_top = y;
    float y_center = y + height / 2.0f;
    float y_bottom = y + height - 1;

    float sample_points[4][2] = {
        {x_left, y_top}, {x_right, y_top}, {x_left, y_bottom}, {x_right, y_bottom}};

    for (int i = 0; i < 4; ++i)
    {
        int tx = (int)(sample_points[i][0] / TILE_SIZE);
        int ty = (int)(sample_points[i][1] / TILE_SIZE);

        if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT)
            return true; // 越界算阻挡

        int tile = tilemap[ty][tx];
        if (tile == TILE_WALL) // 后续可拓展类型
            return true;
    }

    return false; // 没有任意采样点撞墙
}
