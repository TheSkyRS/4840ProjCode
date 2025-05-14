#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////定量/////////////////////////////////////////////////////

#define NUM_ITEMS 6
#define VACTIVE 480
#define NUM_BOXES 1
#define NUM_LEVERS 1
#define NUM_ELEVATORS 2

#define SPRITE_H_PIXELS 16
#define SPRITE_W_PIXELS 16

// #define GRAVITY 0.2f
// #define JUMP_VELOCITY -10.0f
// #define MOVE_SPEED 5.0f

#define MAX_FRAME_TIMER 6 // 控制动画切换速度
#define PLAYER_HEIGHT_PIXELS 28
#define PLAYER_HITBOX_HEIGHT 24
#define PLAYER_HITBOX_OFFSET_Y 4
#define NUM_PLAYERS 2

// === 地图常量 ===
#define MAP_WIDTH 40  // tile 横向数量
#define MAP_HEIGHT 30 // tile 纵向数量
#define TILE_SIZE 16  // 每个 tile 像素大小（单位：px）

#define NUM_BUTTONS 2

#define PLAYER_NONE -1

////////////////////////////////////结构体///////////////////////////////////////////////////////
// === sprite_t ===
typedef struct
{
    uint8_t index;
    uint8_t frame_id;
    uint8_t flip;
    uint16_t x, y;
    bool enable;
    uint8_t frame_count;
    uint8_t frame_start;
} sprite_t;

// === item_owner_t ===
typedef enum
{
    ITEM_ANY = 0,
    ITEM_FIREBOY_ONLY,
    ITEM_WATERGIRL_ONLY
} item_owner_t;

// === item_t ===
typedef struct item_t
{
    float x, y;
    float width, height;
    bool active;
    bool float_anim;
    sprite_t sprite;
    item_owner_t owner_type;
} item_t;

// === player_type_t ===
typedef enum
{
    PLAYER_FIREBOY,
    PLAYER_WATERGIRL
} player_type_t;

// === box_t ===
typedef struct
{
    float x, y;
    float vx;
    bool active;
    sprite_t sprites[4];
    player_type_t pushing_player_type;
} box_t;

// === player_state_t ===
typedef enum
{
    STATE_IDLE,
    STATE_RUNNING,
    STATE_JUMPING,
    STATE_FALLING,
    STATE_DEAD
} player_state_t;

// === player_t ===
typedef struct player_t
{
    float x, y;
    float vx, vy;
    bool on_ground;

    player_state_t state;
    player_type_t type;

    sprite_t upper_sprite;
    sprite_t lower_sprite;

    int frame_timer;
    int frame_index;
    bool was_on_slope_last_frame;
} player_t;

// === Tile 类型 ===
typedef enum
{
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_FIRE = 2,
    TILE_WATER = 3,
    TILE_POISON = 4,

    TILE_SLOPE_L_UP = 5, // 地面：左高右低（\）
    TILE_SLOPE_R_UP = 6, // 地面：右高左低（/）

    TILE_CEIL_R = 7, // 天花板：左高右低（\）
    TILE_CEIL_L = 8, // 天花板：左低右高（/）
    TILE_GOAL1 = 9,  // 水人终点
    TILE_GOAL2 = 10
} tile_type_t;

typedef struct
{
    float x, y;               // 左上角位置，单位像素
    bool activated;           // 当前状态：true=拉下，false=未激活
    uint8_t base_frame[2];    // 底座帧（两个）
    uint8_t handle_frames[3]; // 拉杆动画帧（左、中、右）
    sprite_t base_sprites[4]; // 底座：3 tile，对应3个精灵
    // sprite_t handle_sprite;   // 拉杆主体
    sprite_t handle_sprite_left;
    sprite_t handle_sprite_right;
    int sprite_base_index;           // 精灵 index 起点（需初始化分配）
    bool left_entered[NUM_PLAYERS];  // 记录每个角色是否在左侧进入
    bool right_entered[NUM_PLAYERS]; // 记录每个角色是否在右侧进入
} lever_t;

typedef struct
{
    float x, y;            // 左上角位置
    float vy;              // 垂直速度（可为0）
    float min_y, max_y;    // 电梯运动范围
    bool moving_up;        // 当前方向
    bool active;           // 是否正在激活/运动
    sprite_t sprites[4];   // 横向电梯的4个 tile
    int sprite_base_index; // 精灵 index 起点
} elevator_t;

typedef struct
{
    float x, y; // 按钮左上角像素坐标（按钮上部 sprite）
    sprite_t top_sprite;
    sprite_t base_left_sprite;
    sprite_t base_right_sprite;

    uint8_t sprite_index_base;
    uint8_t frame_top;
    uint8_t frame_base_left;
    uint8_t frame_base_right;
    float press_offset; // 当前压下深度，单位像素（0~8）
    bool pressed;       // 后续可用于功能判断
} button_t;

///////////////////////////////////////数组////////////////////////////////////////////////////

extern player_t players[NUM_PLAYERS];
extern item_t items[NUM_ITEMS];
extern box_t boxes[NUM_BOXES];
extern lever_t levers[NUM_LEVERS];
extern elevator_t elevators[NUM_ELEVATORS];
extern button_t buttons[NUM_BUTTONS];
extern unsigned frame_counter; // 引入主循环中定义的帧计数器
extern const int tilemap[MAP_HEIGHT][MAP_WIDTH];

///////////////////////////////////////////////////////////////////////////////////////////
#endif // TYPEDEFS_H

// 钻石4 5 6 7 8 9
// 箱子10 11 12 13
// 电梯黄 14 15 16 17
// 电梯紫 18 19 20 21
// 拉杆黄 22 23 24 25
// 按钮   26 17 28
//        29 30 21
// 火男0 1
// 水女2 3