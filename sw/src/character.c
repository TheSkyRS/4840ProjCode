// character.c
// 角色控制逻辑模块实现：运动控制、动画播放、精灵同步

#include "character.h" // 包含角色结构体与函数声明
#include "vgasys.h"
// === 动画控制参数 ===
#define NUM_HEAD_FRAMES 8   // 头部动画帧总数
#define NUM_BODY_FRAMES 8   // 身体动画帧总数
#define FRAME_INTERVAL 0.1f // 每帧动画持续时间（秒）
#define HEAD_OFFSET_Y 0     // 头部相对角色位置的 Y 偏移
#define BODY_OFFSET_Y 12    // 身体相对角色位置的 Y 偏移

// 更新角色动画帧（依据当前状态决定帧区间）
void update_character_animation(character_t *ch, float delta_time)
{
    int start_frame_head = 0, end_frame_head = 0; // 头部动画帧范围
    int start_frame_body = 0, end_frame_body = 0; // 身体动画帧范围

    // 根据角色状态选择对应的动画帧范围
    switch (ch->state)
    {
    case STATE_IDLE: // 站立状态使用前两帧
        start_frame_head = 0;
        end_frame_head = 1;
        start_frame_body = 0;
        end_frame_body = 1;
        break;
    case STATE_MOVING_LEFT:
    case STATE_MOVING_RIGHT: // 行走状态使用帧2~5
        start_frame_head = 2;
        end_frame_head = 3;
        start_frame_body = 2;
        end_frame_body = 5;
        break;
    case STATE_JUMPING:
    case STATE_FALLING: // 跳跃/下落状态固定帧6
        start_frame_head = 6;
        end_frame_head = 6;
        start_frame_body = 6;
        end_frame_body = 6;
        break;
    case STATE_DEAD: // 死亡状态使用帧7
        start_frame_head = 7;
        end_frame_head = 7;
        start_frame_body = 7;
        end_frame_body = 7;
        break;
    default: // 默认情况保持第一帧
        start_frame_head = 0;
        end_frame_head = 0;
        start_frame_body = 0;
        end_frame_body = 0;
        break;
    }

    // === 头部动画更新 ===
    ch->anim_timer_head += delta_time;         // 累加头部动画时间
    if (ch->anim_timer_head >= FRAME_INTERVAL) // 到达切换时间
    {
        ch->frame_head++; // 切换到下一帧
        if (ch->frame_head > end_frame_head || ch->frame_head < start_frame_head)
            ch->frame_head = start_frame_head; // 超出范围则回到起始帧
        ch->anim_timer_head = 0.0f;            // 重置计时器
    }

    // === 身体动画更新 ===
    ch->anim_timer_body += delta_time;
    if (ch->anim_timer_body >= FRAME_INTERVAL)
    {
        ch->frame_body++;
        if (ch->frame_body > end_frame_body || ch->frame_body < start_frame_body)
            ch->frame_body = start_frame_body;
        ch->anim_timer_body = 0.0f;
    }
}

// 根据输入更新角色状态和速度
void update_character_state(character_t *ch, game_action_t input, float dt)
{
    if (!ch->alive) // 死亡角色不处理
        return;

    // === 水平运动控制 ===
    if (input == ACTION_MOVE_LEFT)
    {
        ch->vx = -MOVE_SPEED;     // 向左移动
        ch->facing_right = false; // 朝左
        ch->state = STATE_MOVING_LEFT;
    }
    else if (input == ACTION_MOVE_RIGHT)
    {
        ch->vx = MOVE_SPEED;     // 向右移动
        ch->facing_right = true; // 朝右
        ch->state = STATE_MOVING_RIGHT;
    }
    else // 无方向输入
    {
        ch->vx = 0;
        ch->state = STATE_IDLE;
    }

    // === 垂直运动控制 ===
    if (input == ACTION_JUMP)
    {
        ch->vy = JUMP_VELOCITY; // 起跳速度
        ch->state = STATE_JUMPING;
    }
    else
    {
        ch->vy += GRAVITY * dt; // 始终施加重力
        if (ch->vy > 0.1f && ch->state != STATE_JUMPING)
            ch->state = STATE_FALLING; // 下落状态识别
    }

    update_character_animation(ch, dt); // 最后更新动画帧
}

// 根据速度更新角色位置（简单积分）
void update_character_position(character_t *ch, float dt)
{
    if (!ch->alive)
        return;
    ch->x += ch->vx * dt; // 更新水平位置
    ch->y += ch->vy * dt; // 更新垂直位置
}
/**
 * @brief 将角色的两个精灵帧（头部和身体）写入 sprite 缓存数组
 *
 * @param ch           指向角色结构体的指针
 * @param sprite_words 缓存数组，存放将要写入硬件的 attr_word
 * @param count        指向当前写入个数的指针，会被更新
 * @param max_count    数组容量（用于防止越界）
 */
void character_push_sprites(character_t *ch, uint32_t *sprite_words, int *count, int max_count)
{
    if (!ch || !ch->alive || *count >= max_count)
        return;

    // === 写入头部 sprite attr_word ===
    if (*count < max_count)
    {
        uint32_t head = make_attr_word(1, !ch->facing_right,
                                       (uint16_t)ch->x,
                                       (uint16_t)(ch->y + HEAD_OFFSET_Y),
                                       ch->frame_head);
        sprite_words[(*count)++] = head;
    }

    // === 写入身体 sprite attr_word ===
    if (*count < max_count)
    {
        uint32_t body = make_attr_word(1, !ch->facing_right,
                                       (uint16_t)ch->x,
                                       (uint16_t)(ch->y + BODY_OFFSET_Y),
                                       ch->frame_body);
        sprite_words[(*count)++] = body;
    }
}

// 初始化一个角色实例（位置、类型、默认状态）
void init_character(character_t *ch, float x, float y, character_type_t type)
{
    ch->x = x;
    ch->y = y;
    ch->vx = 0;
    ch->vy = 0;
    ch->width = 16;  // 默认宽度（可根据实际精灵调整）
    ch->height = 32; // 默认高度
    ch->frame_head = 0;
    ch->frame_body = 0;
    ch->anim_timer_head = 0.0f;
    ch->anim_timer_body = 0.0f;
    ch->facing_right = true;
    ch->alive = true;
    ch->state = STATE_IDLE;
    ch->type = type;

    // 设置绘制优先级（让水女孩在火男孩上层）
    ch->priority_offset = (type == TYPE_WATERGIRL) ? 1 : 0;
}
