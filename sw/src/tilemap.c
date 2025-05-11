// tilemap.c
// 地形图数据与 tile 碰撞检测实现

#include "tilemap.h"
#include <math.h> // 用于 floor()

// === 示例地图数据 ===
// 0: 空地  1: 墙壁  2: 火池  3: 水池  4: 终点 5
const int tilemap[30][40] = {
    //               x              1              5              2              5              3              5              4
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 1
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 1, 1, 1}, // 2
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

#define COLLISION_MARGIN 1.0f

/*
 * is_tile_blocked -- 检查给定区域（x,y,width,height）与 tilemap 中障碍物的碰撞情况
 *
 * 返回值意义：
 *   1.0f  : 完全没有碰撞，可以自由通过；
 *   0.4f  : 处于斜坡区域（减速区域），允许通过，但应减速；
 *   0.0f  : 碰撞发生（墙、天花板或超出地图），不允许通过。
 *
 * 设计思路：采用七点采样法，其中底部中点（脚底采样）用于检测斜坡效果，
 * 其他六点（左上、右上、左中、右中、左下、右下）用于检测普通墙和天花板。
 */
float is_tile_blocked(float x, float y, float width, float height)
{
    float x_left = x + COLLISION_MARGIN;
    float x_right = x + width - 1 - COLLISION_MARGIN;
    float x_mid = (x_left + x_right) / 2.0f;

    float y_top = y + COLLISION_MARGIN;
    float y_mid = y + height / 2.0f;
    float y_bottom = y + height - 1 - COLLISION_MARGIN;
    float y_foot = y + height;

    // 六点采样：左上、右上、左中、右中、左下、右下
    float sample_points[6][2] = {
        {x_left, y_top},
        {x_right, y_top},
        {x_left, y_mid},
        {x_right, y_mid},
        {x_left, y_bottom},
        {x_right, y_bottom}};

    for (int i = 0; i < 6; ++i)
    {
        float sx = sample_points[i][0];
        float sy = sample_points[i][1];
        int tx = (int)(sx / TILE_SIZE);
        int ty = (int)(sy / TILE_SIZE);

        if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT)
            return 0.0f;

        int tile = tilemap[ty][tx];
        float local_x = sx - tx * TILE_SIZE;
        float local_y = sy - ty * TILE_SIZE;

        switch (tile)
        {
        case TILE_WALL:
            return 0.0f;
        case TILE_CEIL_R:
            if (local_y <= TILE_SIZE - local_x)
                return 0.0f;
            break;
        case TILE_CEIL_L:
            if (local_y <= local_x)
                return 0.0f;
            break;
        }
    }

    // ==== 斜坡检测 ====
    float foot_sx = x_mid;
    float foot_sy = y_foot;
    int ftx = (int)(foot_sx / TILE_SIZE);
    int fty = (int)(foot_sy / TILE_SIZE);

    if (ftx < 0 || ftx >= MAP_WIDTH || fty < 0 || fty >= MAP_HEIGHT)
        return 0.0f;

    int ftile = tilemap[fty][ftx];
    float f_local_x = foot_sx - ftx * TILE_SIZE;
    float f_local_y = foot_sy - fty * TILE_SIZE;

    if (ftile == TILE_SLOPE_L_UP)
    {
        float slope_y = TILE_SIZE - f_local_x;
        if (f_local_y <= slope_y)
            return 0.5f; // 落在斜坡上
        else if (f_local_y <= slope_y + 4.0f)
            return 0.75f; // 临近斜坡表面
        else
            return 1.0f; // 未触碰
    }
    else if (ftile == TILE_SLOPE_R_UP)
    {
        float slope_y = f_local_x;
        if (f_local_y <= slope_y)
            return 0.5f;
        else if (f_local_y <= slope_y + 4.0f)
            return 0.75f;
        else
            return 1.0f;
    }

    return 1.0f; // 空中自由
}

float get_slope_height(tile_type_t type, float local_x)
{
    // 确保 local_x 在 [0, TILE_SIZE)
    if (local_x < 0)
        local_x = 0;
    if (local_x >= TILE_SIZE)
        local_x = TILE_SIZE - 1;

    switch (type)
    {
    case TILE_SLOPE_L_UP:
        return TILE_SIZE - local_x; // y = -x + TILE_SIZE
    case TILE_SLOPE_R_UP:
        return local_x; // y = x
    default:
        return 0.0f; // 普通地形无高度
    }
}

bool is_tile_slope(tile_type_t type)
{
    return (type == TILE_SLOPE_L_UP || type == TILE_SLOPE_R_UP);
}

tile_type_t tilemap_get_type_at(float x, float y)
{
    int tx = (int)(x / TILE_SIZE);
    int ty = (int)(y / TILE_SIZE);

    if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT)
        return TILE_WALL; // 越界视作墙壁

    return tilemap[ty][tx];
}
