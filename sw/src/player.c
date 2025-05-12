#include "player.h"
#include "joypad_input.h"
#include "tilemap.h"
#include <math.h> // 用于 floor()

// #define GRAVITY 0.4f
// #define JUMP_VELOCITY -5.0f

#include <stdio.h> // 顶部加这个

void debug_print_player_state(player_t *p, const char *tag)
{
    float center_x = p->x + SPRITE_W_PIXELS / 2.0f;
    float foot_y = p->y + PLAYER_HEIGHT_PIXELS + 1;
    int tile = get_tile_at_pixel(center_x, foot_y);
    int tx = (int)(center_x / TILE_SIZE);
    int ty = (int)(foot_y / TILE_SIZE);

    printf("[%s] x=%.1f y=%.1f vx=%.2f vy=%.2f on_ground=%d foot_tile=%d (%d,%d)\n",
           tag, p->x, p->y, p->vx, p->vy, p->on_ground, tile, tx, ty);
}
void player_init(player_t *p, int x, int y,
                 uint8_t upper_index, uint8_t lower_index,
                 player_type_t type)
{
    p->x = x;
    p->y = y;
    p->vx = p->vy = 0;
    p->on_ground = false;
    p->state = STATE_IDLE;
    p->type = type;
    p->frame_timer = 0;
    p->frame_index = 0;
    p->was_on_slope_last_frame = false;

    if (type == PLAYER_FIREBOY)
    {
        sprite_set(&p->upper_sprite, upper_index, 0);
        sprite_set(&p->lower_sprite, lower_index, 0);
    }
    else
    {
        sprite_set(&p->upper_sprite, upper_index, 0);
        sprite_set(&p->lower_sprite, lower_index, 0);
    }
}

void player_handle_input(player_t *p, int player_index)
{
    game_action_t action = get_player_action(player_index);

    // 处理跳跃，必须放前面
    if (action == ACTION_JUMP && p->on_ground)
    {
        p->vy = JUMP_VELOCITY;
        p->on_ground = false;
        p->state = STATE_JUMPING;
    }

    // 处理水平移动
    if (action == ACTION_MOVE_LEFT)
    {
        p->vx = -MOVE_SPEED;
        p->lower_sprite.flip = 1;
        p->upper_sprite.flip = 1;
    }
    else if (action == ACTION_MOVE_RIGHT)
    {
        p->vx = MOVE_SPEED;
        p->lower_sprite.flip = 0;
        p->upper_sprite.flip = 0;
    }
    else if (p->on_ground) // 空中不立即取消横向速度
    {
        p->vx = 0;
    }
}

void player_update_physics(player_t *p)
{
    p->vy += GRAVITY;
    // 垂直运动
    float new_y = p->y + p->vy;

    if (!is_tile_blocked(p->x, new_y + 1, SPRITE_W_PIXELS, PLAYER_HEIGHT_PIXELS))
    {
        p->y = new_y;
        p->on_ground = false;
    }
    else
    {
        // 调用你写的吸附函数
        adjust_to_platform_y(p);

        if (p->vy > 0)
            p->on_ground = true;

        p->vy = 0;
    }

    // float new_y = p->y + p->vy;
    // if (!is_tile_blocked(p->x, new_y + 1, SPRITE_W_PIXELS, PLAYER_HEIGHT_PIXELS)) // 一步之遥。
    // {
    //     p->y = new_y;
    //     p->on_ground = false;
    // }
    // else
    // {   //debug
    //     if (p->vy < 0 && (p->type == PLAYER_WATERGIRL))
    //     {
    //         printf("[%s] HEAD HIT: vy=%.2f y=%.2f\n",
    //                p->type == PLAYER_FIREBOY ? "FIREBOY" : "WATERGIRL",
    //                p->vy, p->y);
    //     }
    //     else if (p->vy > 0 && (p->type == PLAYER_WATERGIRL))
    //     {
    //         printf("[%s] FOOT LAND: vy=%.2f y=%.2f\n",
    //                p->type == PLAYER_FIREBOY ? "FIREBOY" : "WATERGIRL",
    //                p->vy, p->y);
    //     }
    //     //debug
    //     if (p->vy > 0)
    //         p->on_ground = true;
    //     p->vy = 0;
    // }

    // 水平运动
    float new_x = p->x + p->vx;

    // 计算当前位置和目标位置的脚底中心点
    float cur_foot_x = p->x + SPRITE_W_PIXELS / 2.0f;
    float new_foot_x = new_x + SPRITE_W_PIXELS / 2.0f;
    float foot_y = p->y + PLAYER_HEIGHT_PIXELS;

    // 判断当前位置脚底左右邻近是否在斜坡范围
    bool on_slope = false;
    for (int dx = -1; dx <= 1; ++dx)
    {
        int tile = get_tile_at_pixel(new_foot_x + dx * 8, foot_y);
        if (tile == TILE_SLOPE_L_UP || tile == TILE_SLOPE_R_UP)
        {
            on_slope = true;
            break;
        }
    }
    if (on_slope && p->vy >= 0)
    {
        p->x = new_x;
        adjust_to_slope_y(p);
    }
    else if (!is_tile_blocked(new_x, p->y, SPRITE_W_PIXELS, PLAYER_HEIGHT_PIXELS))
    {
        p->x = new_x;
    }
    else
    {
        p->vx = 0;
    }

    // 状态切换
    if (!p->on_ground)
    {
        if (p->vy < -0.1f)
            p->state = STATE_JUMPING;
        else if (p->vy > 0.1f)
            p->state = STATE_FALLING;
        else
            p->state = STATE_IDLE; // 空中静止（很少见）
    }
    else if (p->vx != 0)
    {
        p->state = STATE_RUNNING;
    }
    else
    {
        p->state = STATE_IDLE;
    }
    // if (p->type == PLAYER_WATERGIRL)
    //     debug_print_player_state(p, p->type == "WATERGIRL");
}
void adjust_to_slope_y(player_t *p)
{
    // 计算角色脚底中心的水平位置
    float center_x = p->x + SPRITE_W_PIXELS / 2.0f;

    // 计算角色当前脚底的 y 坐标（28 高度）
    float base_foot_y = p->y + PLAYER_HEIGHT_PIXELS;

    // 开始在脚底附近做上下小范围搜索，找出脚底是否正好踩在坡道上
    // 搜索 dy 从 -4 到 +2，可以捕捉到上下微小误差（如浮空或压入）
    for (int dy = -4; dy <= 2; ++dy) // 经验偏移量
    {
        float foot_y = base_foot_y + dy; // 当前搜索点在 y 方向的位置

        // 获取该位置所处的 tile 类型
        int tile = get_tile_at_pixel(center_x, foot_y);

        // 如果检测到是斜坡 tile，才进行对齐处理
        if (tile == TILE_SLOPE_L_UP || tile == TILE_SLOPE_R_UP)
        {
            // 计算当前点在 tile 内部的 x 偏移（即左上角为(0,0)，当前 x 在 tile 内多少像素）
            float x_in_tile = fmod(center_x, TILE_SIZE);
            int x_local = (int)x_in_tile;

            // 计算此 x 偏移在当前坡 tile 中应对应的 y 高度
            // 左坡是从左下升到右上：越靠右越高 → y = x
            // 右坡是从右下升到左上：越靠左越高 → y = TILE_SIZE - 1 - x
            int min_y = (tile == TILE_SLOPE_L_UP)
                            ? x_local
                            : TILE_SIZE - 1 - x_local;

            // 计算该 tile 的顶部 y 坐标（tile 是 16×16，找出 tile 所在行的起始 y）
            float tile_top_y = ((int)(foot_y / TILE_SIZE)) * TILE_SIZE;

            // // 设置角色的 y 坐标：
            // // - tile_top_y + min_y：得到坡面的 y 高度
            // // - 减去 PLAYER_HEIGHT_PIXELS：让角色站在坡面上
            // // - 再减 3：一个经验偏移，用于微调贴合（可调试）
            // p->y = tile_top_y + min_y - PLAYER_HEIGHT_PIXELS - 3;

            // // 设置角色落地状态
            // p->on_ground = true;
            // // 停止垂直速度（不会继续下落或上升）
            // p->vy = 0;

            // // 成功处理后退出搜索
            // break;
            // 记录旧 y
            float old_y = p->y;

            // 计算吸附目标 y
            float new_y = tile_top_y + min_y - PLAYER_HEIGHT_PIXELS - 3; // 经验偏移量

            // 若前后 y 差距很小，就保留原 vy，避免动画判定被破坏
            if (fabsf(new_y - old_y) < 0.2f)
            {
                p->y = new_y;
                // 保留 vy，不强制设为 0
            }
            else
            {
                p->y = new_y;
                p->vy = 0; // 只有明显吸附时才归零
            }

            p->on_ground = true;
            break;
        }
    }
}

void adjust_to_platform_y(player_t *p)
{
    float center_x = p->x + SPRITE_W_PIXELS / 2.0f;
    float base_foot_y = p->y + PLAYER_HEIGHT_PIXELS;

    for (int dy = -8; dy <= 8; ++dy) // 经验偏移量
    {
        float foot_y = base_foot_y + dy;
        int tile = get_tile_at_pixel(center_x, foot_y);

        if (tile == 1) // 平台 tile
        {
            float tile_top_y = ((int)(foot_y / TILE_SIZE)) * TILE_SIZE;
            float new_y = tile_top_y - PLAYER_HEIGHT_PIXELS - 1; // 经验偏移量

            float old_y = p->y;

            if (fabsf(new_y - old_y) < 0.2f)
            {
                p->y = new_y;
                // vy 保留
            }
            else
            {
                p->y = new_y;
                p->vy = 0;
            }

            p->on_ground = true;
            break;
        }
    }
}
// 火男孩
#define FB_HEAD_IDLE ((uint8_t)0)      // 0x0000 >> 8 = 0
#define FB_HEAD_WALK ((uint8_t)2)      // 0x0200 >> 8 = 2
#define FB_HEAD_UPDOWN ((uint8_t)7)    // 0x0700 >> 8 = 7
#define FB_HEAD_DOWNWALK ((uint8_t)12) // 0x0C00 >> 8 = 12

#define FB_LEG_IDLE ((uint8_t)17)         // 0x1100 >> 8 = 17
#define FB_LEG_WALK ((uint8_t)18)         // 0x1200 >> 8 = 18
#define FB_LEG_UPorDOWNWALK ((uint8_t)21) // 0x1500 >> 8 = 21

// 水女孩
#define WG_HEAD_IDLE ((uint8_t)22)     // 0x1600 >> 8 = 22
#define WG_HEAD_WALK ((uint8_t)24)     // 0x1800 >> 8 = 24
#define WG_HEAD_UPWALK ((uint8_t)29)   // 0x2100 >> 8 = 29
#define WG_HEAD_DOWNWALK ((uint8_t)34) // 0x2200 >> 8 = 34

#define WG_LEG_IDLE ((uint8_t)39)         // 0x2700 >> 8 = 39
#define WG_LEG_WALK ((uint8_t)40)         // 0x2800 >> 8 = 40
#define WG_LEG_UPorDOWNWALK ((uint8_t)43) // 0x2B00 >> 8 = 43

// 每帧调用：自动在行走帧之间循环
static int get_frame_id(player_t *p, bool is_upper)
{
    int base = 0;

    if (p->type == PLAYER_FIREBOY)
    {
        if (p->state == STATE_IDLE)
            base = is_upper ? FB_HEAD_IDLE : FB_LEG_IDLE;
        else if (p->state == STATE_RUNNING)
            base = is_upper ? FB_HEAD_WALK : FB_LEG_WALK;
        else if (p->state == STATE_JUMPING)
            base = is_upper ? FB_HEAD_UPDOWN : FB_LEG_UPorDOWNWALK;
        else if (p->state == STATE_FALLING)
            base = is_upper ? FB_HEAD_DOWNWALK : FB_LEG_UPorDOWNWALK;
    }
    else // WATERGIRL
    {
        if (p->state == STATE_IDLE)
            base = is_upper ? WG_HEAD_IDLE : WG_LEG_IDLE;
        else if (p->state == STATE_RUNNING)
            base = is_upper ? WG_HEAD_WALK : WG_LEG_WALK;
        else if (p->state == STATE_JUMPING)
            base = is_upper ? WG_HEAD_UPWALK : WG_LEG_UPorDOWNWALK;
        else if (p->state == STATE_FALLING)
            base = is_upper ? WG_HEAD_DOWNWALK : WG_LEG_UPorDOWNWALK;
    }

    int frame_count = get_frame_count(p, is_upper);
    return base + (p->frame_index % frame_count);
}

int get_frame_count(player_t *p, bool is_upper)
{
    if (p->state == STATE_RUNNING)
        return is_upper ? 5 : 3;

    if (p->type == PLAYER_FIREBOY)
    {
        if (p->state == STATE_IDLE)
            return is_upper ? 2 : 1;
        else if (p->state == STATE_JUMPING || p->state == STATE_FALLING)
            return is_upper ? 5 : 3;
    }
    else
    {
        if (p->state == STATE_IDLE)
            return is_upper ? 2 : 1;
        else if (p->state == STATE_JUMPING || p->state == STATE_FALLING)
            return is_upper ? 5 : 3;
    }

    return 1; // fallback
}

void player_update_sprite(player_t *p)
{
    // 判断是否需要动画
    bool animate = false;

    switch (p->state)
    {
    case STATE_RUNNING:
    case STATE_IDLE:
        animate = true; // 行走和站立都可以轮播动画（如眨眼）
        break;
    case STATE_JUMPING:
    case STATE_FALLING:
    default:
        animate = false; // 空中或未知状态不动
        break;
    }

    if (animate)
    {
        p->frame_timer++;
        if (p->frame_timer >= MAX_FRAME_TIMER)
        {
            p->frame_timer = 0;
            int frame_count = get_frame_count(p, true); // 上半身决定帧长
            p->frame_index = (p->frame_index + 1) % frame_count;
        }
    }
    else
    {
        p->frame_index = 0;
    }

    if (p->type == PLAYER_FIREBOY)
    {
        // 设置帧 ID
        p->lower_sprite.frame_id = get_frame_id(p, false);
        p->upper_sprite.frame_id = get_frame_id(p, true);

        // 设置位置与启用
        // 身子
        p->lower_sprite.x = p->x;
        p->lower_sprite.y = p->y + SPRITE_H_PIXELS - 1;
        p->lower_sprite.enable = true;
        // 头
        p->upper_sprite.x = p->x;
        p->upper_sprite.y = p->y + 5;
        p->upper_sprite.enable = true;
    }
    if (p->type == PLAYER_WATERGIRL)
    { // 设置帧 ID
        p->lower_sprite.frame_id = get_frame_id(p, false);
        p->upper_sprite.frame_id = get_frame_id(p, true);

        // 设置位置与启用
        // 身子
        p->lower_sprite.x = p->x;
        p->lower_sprite.y = p->y + SPRITE_H_PIXELS - 2;
        p->lower_sprite.enable = true;
        // 头
        p->upper_sprite.x = p->x;
        p->upper_sprite.y = p->y + 4;
        p->upper_sprite.enable = true;
    }

    sprite_update(&p->lower_sprite);
    sprite_update(&p->upper_sprite);
}
