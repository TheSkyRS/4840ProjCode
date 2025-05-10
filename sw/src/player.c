#include "player.h"
#include "joypad_input.h"
#include "tilemap.h"

#define GRAVITY 0.4f
#define JUMP_VELOCITY -5.0f
#define MOVE_SPEED 1.5f
#define MAX_FRAME_TIMER 6 // 控制动画切换速度

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

    if (action == ACTION_MOVE_LEFT)
    {
        p->vx = -MOVE_SPEED;
        p->lower_sprite.flip = 1;
    }
    else if (action == ACTION_MOVE_RIGHT)
    {
        p->vx = MOVE_SPEED;
        p->lower_sprite.flip = 0;
    }
    else
    {
        p->vx = 0;
    }

    if (action == ACTION_JUMP && p->on_ground)
    {
        p->vy = JUMP_VELOCITY;
        p->on_ground = false;
        p->state = STATE_JUMPING;
    }
}

void player_update_physics(player_t *p)
{
    p->vy += GRAVITY;

    // 垂直运动
    float new_y = p->y + p->vy;
    if (!is_tile_blocked(p->x, new_y, SPRITE_W_PIXELS, SPRITE_H_PIXELS * 2))
    {
        p->y = new_y;
        p->on_ground = false;
    }
    else
    {
        if (p->vy > 0)
            p->on_ground = true;
        p->vy = 0;
    }

    // 水平运动
    float new_x = p->x + p->vx;
    if (!is_tile_blocked(new_x, p->y, SPRITE_W_PIXELS, SPRITE_H_PIXELS * 2))
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
        if (p->vy < 0)
            p->state = STATE_JUMPING;
        else
            p->state = STATE_FALLING;
    }
    else if (p->vx != 0)
    {
        p->state = STATE_RUNNING;
    }
    else
    {
        p->state = STATE_IDLE;
    }
}
// // 帧段定义
// #define FB_HEAD_IDLE 0x00        // 站立时头部
// #define FB_HEAD_WALK 0x02        // 行走头部
// #define FB_HEAD_UPDOWN 0x07      // 上跳头部
// #define FB_HEAD_DOWNWALK 0x0C    // 下落头部
// #define FB_LEG_IDLE 0x11         // 站立腿部
// #define FB_LEG_WALK 0x12         // 行走腿部
// #define FB_LEG_UPorDOWNWALK 0x15 // 上跳or下落腿部

// // 水妹头部
// #define WG_HEAD_IDLE 0x16
// #define WG_HEAD_WALK 0x18
// #define WG_HEAD_UPWALK 0x1D
// #define WG_HEAD_DOWNWALK 0x22

// #define WG_LEG_IDLE 0x27
// #define WG_LEG_WALK 0x28
// #define WG_LEG_UPorDOWNWALK 0x2B

// 火男孩
#define FB_HEAD_IDLE 0x0000     // 0x00
#define FB_HEAD_WALK 0x0200     // 0x20
#define FB_HEAD_UPDOWN 0x0700   // 0x70
#define FB_HEAD_DOWNWALK 0x0C00 // 0xC0

#define FB_LEG_IDLE 0x1100         // 0x110
#define FB_LEG_WALK 0x1200         // 0x120
#define FB_LEG_UPorDOWNWALK 0x1500 // 0x150

// 水女孩
#define WG_HEAD_IDLE 0x1600     // 0x160
#define WG_HEAD_WALK 0x1800     // 0x180
#define WG_HEAD_UPWALK 0x2100   // 0x210
#define WG_HEAD_DOWNWALK 0x2200 // 0x220

#define WG_LEG_IDLE 0x2700         // 0x270
#define WG_LEG_WALK 0x2800         // 0x280
#define WG_LEG_UPorDOWNWALK 0x2B00 // 0x2B0

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
        return 5;

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
    // 处理帧动画切换（只在行走时更新）
    if (p->state == STATE_RUNNING)
    {
        p->frame_timer++;
        if (p->frame_timer >= MAX_FRAME_TIMER)
        {
            p->frame_timer = 0;
            p->frame_index = (p->frame_index + 1) % 5;
        }
    }
    else
    {
        p->frame_index = 0;
    }

    // 设置帧 ID
    p->lower_sprite.frame_id = get_frame_id(p, false);
    p->upper_sprite.frame_id = get_frame_id(p, true);

    // 设置位置与启用
    p->lower_sprite.x = p->x;
    p->lower_sprite.y = p->y;
    p->lower_sprite.enable = true;

    p->upper_sprite.x = p->x;
    p->upper_sprite.y = p->y - SPRITE_H_PIXELS;
    p->upper_sprite.enable = true;

    sprite_update(&p->lower_sprite);
    sprite_update(&p->upper_sprite);
}
