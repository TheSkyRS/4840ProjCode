// character.h
// 控制角色行为与动画的模块头文件（Fireboy & Watergirl）

#ifndef CHARACTER_H
#define CHARACTER_H
#include "keyboard_input.h"
#include <stdbool.h>
#include <stdint.h>

// === 角色行为参数 ===
#define MOVE_SPEED 60.0f      // 水平移动速度（像素/秒）
#define JUMP_VELOCITY -140.0f // 起跳速度（负值向上）
#define GRAVITY 400.0f        // 重力加速度

// === 动画控制参数 ===
#define NUM_HEAD_FRAMES 8   // 头部帧总数
#define NUM_BODY_FRAMES 8   // 身体帧总数
#define FRAME_INTERVAL 0.1f // 每帧动画持续时间（秒）

// 枚举：角色当前所处状态（用于动画选择与行为控制）
typedef enum
{
    STATE_IDLE,         // 静止站立
    STATE_MOVING_LEFT,  // 向左移动
    STATE_MOVING_RIGHT, // 向右移动
    STATE_JUMPING,      // 跳跃中
    STATE_FALLING,      // 下落中
    STATE_DEAD          // 死亡（不可控制）
} character_state_t;

// 枚举：角色身份
typedef enum
{
    TYPE_FIREBOY,  // 火男孩
    TYPE_WATERGIRL // 水女孩
} character_type_t;

// 主结构体：角色的行为控制与动画状态
typedef struct
{
    float x, y;              // 当前坐标（左上角）
    float vx, vy;            // 当前速度（像素/秒）
    int width, height;       // 尺寸信息（用于渲染/碰撞）
    int frame_head;          // 当前头部动画帧编号
    int frame_body;          // 当前身体动画帧编号
    float anim_timer_head;   // 头部动画帧计时器
    float anim_timer_body;   // 身体动画帧计时器
    bool facing_right;       // 是否朝向右侧
    bool alive;              // 是否仍在游戏中（未死亡）
    int priority_offset;     // 精灵渲染优先级偏移（用于碰撞时的图层控制）
    character_state_t state; // 当前动作状态（站立、移动、跳跃等）
    character_type_t type;   // Fireboy or Watergirl
} character_t;
// 初始化角色属性
void init_character(character_t *ch, float x, float y, character_type_t type);

// 每帧调用：角色输入、状态、动画、重力的综合更新逻辑
void update_character_state(character_t *ch, game_action_t input, float dt);

// 每帧调用：基于速度更新位置（简单积分）
void update_character_position(character_t *ch, float dt);

// 每帧调用：根据角色状态更新动画帧（头部和身体分开）
void update_character_animation(character_t *ch, float delta_time);

/**
 * 将角色的两个精灵帧（头部和身体）写入 sprite 缓存数组
 *
 * @param ch           指向角色结构体的指针
 * @param sprite_words 输出数组
 * @param count        当前写入数（会被修改）
 * @param max_count    最大容量
 */
void character_push_sprites(character_t *ch, uint32_t *sprite_words, int *count, int max_count);

#endif // CHARACTER_H
