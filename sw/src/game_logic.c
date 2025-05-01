/*
 * Forest Fire and Ice 人物行为重构模块
 * 本模块定义了Sprite。
 * 本模块实现两个Sprite（头部 + 身体）的角色控制，具备独立动画帧切换。
 * 不包含地图碰撞与互动逻辑，仅实现基于坐标和速度的自由运动。
 */

// 使用说明：
// 在主程序中定义一个包含两个角色（Fireboy 和 Watergirl）的 character_t 数组，
// 每帧调用 update_all_characters() 即可完成状态更新与同步渲染。
// 当前版本不包含任何碰撞检测或地图逻辑，适用于行为部分的原型开发。

#include "../include/game_logic.h"    // 包含游戏逻辑头文件
#include "../include/hw_interface.h"  // 包含硬件接口头文件
#include "../include/input_handler.h" // 包含输入处理头文件
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// 定义动画相关参数
#define NUM_HEAD_FRAMES 8   // 头部动画总帧数（建议包含站立、走路、跳跃、死亡）
#define NUM_BODY_FRAMES 8   // 身体动画总帧数
#define FRAME_INTERVAL 0.1f // 每隔0.1秒切换一帧动画
#define HEAD_OFFSET_Y 0     // 头部相对于角色坐标的垂直偏移量
#define BODY_OFFSET_Y 12    // 身体相对于角色坐标的垂直偏移量

// 角色状态（状态机使用）
typedef enum
{
    STATE_IDLE,         // 静止
    STATE_MOVING_LEFT,  // 向左移动
    STATE_MOVING_RIGHT, // 向右移动
    STATE_JUMPING,      // 跳跃中
    STATE_FALLING,      // 下落中
    STATE_DEAD          // 死亡
} character_state_t;

// 角色类型（可用于区分 Fireboy 和 Watergirl）
typedef enum
{
    TYPE_FIREBOY,
    TYPE_WATERGIRL
} character_type_t;

/**
 * 定义角色数据结构，保存角色运动、状态和动画相关信息。
 */
typedef struct
{
    float x, y;              // 角色左上角的实际像素坐标
    float vx, vy;            // 当前水平和垂直速度（单位：像素/秒）
    int width, height;       // 角色宽高（用于绘制和碰撞）
    int frame_head;          // 当前头部动画帧编号
    int frame_body;          // 当前身体动画帧编号
    float anim_timer_head;   // 头部动画播放计时器（秒）
    float anim_timer_body;   // 身体动画播放计时器（秒）
    bool facing_right;       // 朝向标志（true向右，false向左）
    bool alive;              // 是否存活（false表示角色死亡，不再更新）
    character_state_t state; // 当前角色状态（站立、行走、跳跃、下落、死亡）
    character_type_t type;   // 角色类型（火男孩或水女孩）
    bool on_ground;          // 是否在地面
    float velocity_y;         // 垂直速度
} character_t;

/**
 * 根据角色当前状态选择合适的动画帧区间，并更新帧播放。
 */
void update_character_animation(character_t *ch, float delta_time)
{
    int start_frame_head = 0, end_frame_head = 0; // 用于头部动画的起始和结束帧编号
    int start_frame_body = 0, end_frame_body = 0; // 用于身体动画的起始和结束帧编号

    // 根据当前状态选择对应的动画帧段
    switch (ch->state)
    {
    case STATE_IDLE: // 站立状态
        start_frame_head = 0;
        end_frame_head = 1;
        start_frame_body = 0;
        end_frame_body = 1;
        break;
        case STATE_MOVING_LEFT:
    case STATE_MOVING_RIGHT: // 行走状态
        start_frame_head = 2;
        end_frame_head = 3;
        start_frame_body = 2;
        end_frame_body = 5;
            break;
        case STATE_JUMPING:
    case STATE_FALLING: // 跳跃或下落状态
        start_frame_head = 6;
        end_frame_head = 6;
        start_frame_body = 6;
        end_frame_body = 6;
            break;
    case STATE_DEAD: // 死亡状态，可以根据'alive'变量直接透明化。
        start_frame_head = 7;
        end_frame_head = 7;
        start_frame_body = 7;
        end_frame_body = 7;
            break;
    default: // 其他异常情况，默认站立帧
        start_frame_head = 0;
        end_frame_head = 0;
        start_frame_body = 0;
        end_frame_body = 0;
            break;
    }
    
    // 更新头部动画播放
    ch->anim_timer_head = ch->anim_timer_head + delta_time; // 累加本帧经过的时间
    if (ch->anim_timer_head >= FRAME_INTERVAL)              // 达到切换间隔则切换动画帧
    {
        ch->frame_head = ch->frame_head + 1; // 帧编号加1
        if (ch->frame_head > end_frame_head || ch->frame_head < start_frame_head)
            ch->frame_head = start_frame_head; // 超出范围时回到起始帧
        ch->anim_timer_head = 0.0f;            // 计时器清零，开始计下一帧
    }

    // 更新身体动画播放
    ch->anim_timer_body = ch->anim_timer_body + delta_time;
    if (ch->anim_timer_body >= FRAME_INTERVAL)
    {
        ch->frame_body = ch->frame_body + 1;
        if (ch->frame_body > end_frame_body || ch->frame_body < start_frame_body)
            ch->frame_body = start_frame_body;
        ch->anim_timer_body = 0.0f;
    }
}

/**
 * 根据玩家输入和物理规律更新角色状态。
 */
void update_character_state(character_t *ch, game_action_t input, float dt)
{
    if (!ch->alive)
        return; // 死亡状态不做任何更新

    // --- 水平移动控制 ---
    if (input == ACTION_MOVE_LEFT)
    {
        ch->vx = -MOVE_SPEED;          // 向左移动，速度为负
        ch->facing_right = false;      // 更新面朝方向
        ch->state = STATE_MOVING_LEFT; // 更新状态为向左行走
    }
    else if (input == ACTION_MOVE_RIGHT)
    {
        ch->vx = MOVE_SPEED; // 向右移动，速度为正
        ch->facing_right = true;
        ch->state = STATE_MOVING_RIGHT;
    }
    else
    {
        ch->vx = 0;             // 无输入时水平速度为0
        ch->state = STATE_IDLE; // 状态设为站立
    }

    // --- 垂直运动控制 ---
    if (input == ACTION_JUMP)
    {
        ch->vy = JUMP_VELOCITY;    // 施加初始跳跃速度（向上）
        ch->state = STATE_JUMPING; // 状态设为跳跃
    }
    else
    {
        ch->vy = ch->vy + GRAVITY * dt; // 持续施加重力，使角色下落
        if (ch->vy > 0.1f && ch->state != STATE_JUMPING)
            ch->state = STATE_FALLING; // 若正在下降且非跳跃状态，则切换为下落状态
    }

    update_character_animation(ch, dt); // 每帧同步更新动画播放
}

/**
 * 基于速度简单地更新角色位置。
 */
void update_character_position(character_t *ch, float dt, uint8_t map_id)
{
    if (!ch->alive)
        return; // 死亡角色不动

    // Apply gravity if character is not on ground
    if (!ch->on_ground) {
        ch->velocity_y += GRAVITY * dt;
    }

    // Update position based on velocity
    float new_x = ch->x + ch->vx * dt;
    float new_y = ch->y + ch->vy * dt;

    // Store original position for collision response
    float original_x = ch->x;
    float original_y = ch->y;

    // Update position
    ch->x = new_x;
    ch->y = new_y;

    // Check for collisions and handle them
    if (handle_character_collisions(ch, map_id)) {
        // If character collided with a hazard, reset to original position
        ch->x = original_x;
        ch->y = original_y;
        ch->alive = false;
    }
}

bool handle_character_collisions(character_t* character, uint8_t map_id) {
    // Check map boundary collisions
    if (check_map_boundary_collision(character)) {
        // Reset character to last valid position
        character->vx = 0;
        character->vy = 0;
        return false;
    }

    // Get character's current tile position
    int char_tile_x = (int)(character->x / TILE_SIZE);
    int char_tile_y = (int)(character->y / TILE_SIZE);

    // Check surrounding tiles for collisions
    for (int y = char_tile_y - 1; y <= char_tile_y + 1; y++) {
        for (int x = char_tile_x - 1; x <= char_tile_x + 1; x++) {
            if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
                continue;
            }

            tile_type_t tile_type = get_tile_at(x * TILE_SIZE, y * TILE_SIZE, map_id);
            
            // Skip empty tiles
            if (tile_type == TILE_EMPTY) {
                continue;
            }

            // Check for collision with the tile
            if (check_character_tile_collision(character, x, y)) {
                // Create bounding boxes for collision resolution
                bounding_box_t char_box = {
                    .x = character->x,
                    .y = character->y,
                    .width = character->width,
                    .height = character->height
                };

                bounding_box_t tile_box = {
                    .x = x * TILE_SIZE,
                    .y = y * TILE_SIZE,
                    .width = TILE_SIZE,
                    .height = TILE_SIZE
                };

                // Calculate penetration depth
                float penetration_x, penetration_y;
                calculate_penetration_depth(&char_box, &tile_box, &penetration_x, &penetration_y);

                // Resolve collision
                resolve_collision(character, penetration_x, penetration_y);

                // Check for hazard collision
                if (check_hazard_collision(character, tile_type)) {
                    return true;
                }
            }
        }
    }

    return false;
}

/**
 * 构造一个单独的 sprite 属性（头或身体），用于写入硬件。
 */
sprite_attr_t make_sprite(float x, float y, int frame_id, int width, int height, int priority, bool alive, bool facing_right)
{
    sprite_attr_t s;
    s.x = (uint16_t)x;                                   // Sprite绘制位置X
    s.y = (uint16_t)y;                                   // Sprite绘制位置Y
    s.frame_id = (uint8_t)frame_id;                      // 使用的图块帧编号
    s.priority = (uint8_t)priority;                      // 优先级决定显示顺序
    s.width = (uint8_t)width;                            // Sprite宽度
    s.height = (uint8_t)height;                          // Sprite高度
    s.flags.value = 0;                                   // 清空所有标志位
    s.flags.bits.alive = alive ? 1 : 0;                  // 设置是否激活绘制
    s.flags.bits.flip_horizontal = facing_right ? 0 : 1; // 方向共控制位

    return s;
}

/**
 * 将角色的头部和身体同步到 sprite 属性表，供硬件绘制。
 */
void sync_characters_to_hardware(character_t *characters, int num_characters)
{
    sprite_attr_t sprites[MAX_SPRITES]; // 预分配最大数量的 sprite 槽位
    int count = 0;                      // 当前已写入的Sprite数量

    for (int i = 0; i < num_characters; ++i)
    {
        character_t *ch = &characters[i];
        if (!ch->alive)
            continue; // 跳过死亡角色

        if (count < MAX_SPRITES)
        {
            sprite_attr_t head_sprite = make_sprite(
                ch->x,                 // X坐标
                ch->y + HEAD_OFFSET_Y, // Y坐标偏移表示头部
                ch->frame_head,        // 当前头部动画帧编号
                ch->width,             // 宽度
                ch->height / 2,        // 高度（头部占一半）
                i * 2 + 1,             // 优先级：头部在身体之上
                true,                  // 是否激活绘制
                ch->facing_right       // 是否面朝右，决定是否需要翻转
            );

            sprites[count++] = head_sprite; // 加入 sprite 表
        }

        // 写入身体精灵属性
        if (count < MAX_SPRITES)
        {
            sprite_attr_t body_sprite = make_sprite(
                ch->x,                 // X坐标
                ch->y + BODY_OFFSET_Y, // Y坐标偏移表示身体
                ch->frame_body,        // 当前身体动画帧编号
                ch->width,             // 宽度
                ch->height / 2,        // 高度（身体占一半）
                i * 2,                 // 优先级：身体在下
                true,                  // 是否激活绘制
                ch->facing_right       // 是否面朝右
            );

            sprites[count++] = body_sprite;
        }
    }

    // 清空剩余未使用的 sprite 槽，防止旧数据残留
    for (int i = count; i < MAX_SPRITES; ++i)
    {
        sprites[i].flags.value = 0;
    }

    update_sprite_table(sprites, MAX_SPRITES); // 将整个 sprite 表写入硬件
}

/**
 * 每一帧统一更新所有角色状态与同步渲染。
 */
void update_all_characters(character_t *characters, int num_characters, float dt)
{
    for (int i = 0; i < num_characters; ++i)
    {
        game_action_t input = get_player_action(i);        // 读取玩家输入指令
        update_character_state(&characters[i], input, dt); // 更新角色状态（含动画）
        update_character_position(&characters[i], dt, 0);     // 更新角色物理位置
    }

    sync_characters_to_hardware(characters, num_characters); // 更新所有角色的Sprite显示到硬件
}
