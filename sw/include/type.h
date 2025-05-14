#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////����/////////////////////////////////////////////////////

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

#define MAX_FRAME_TIMER 6 // ���ƶ����л��ٶ�
#define PLAYER_HEIGHT_PIXELS 28
#define PLAYER_HITBOX_HEIGHT 24
#define PLAYER_HITBOX_OFFSET_Y 4
#define NUM_PLAYERS 2

// === ��ͼ���� ===
#define MAP_WIDTH 40  // tile ��������
#define MAP_HEIGHT 30 // tile ��������
#define TILE_SIZE 16  // ÿ�� tile ���ش�С����λ��px��

#define NUM_BUTTONS 2

#define PLAYER_NONE -1

////////////////////////////////////�ṹ��///////////////////////////////////////////////////////
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

// === Tile types ===
typedef enum
{
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_FIRE = 2,
    TILE_WATER = 3,
    TILE_POISON = 4,

    TILE_SLOPE_L_UP = 5, // Slope: left high, right low (\)
    TILE_SLOPE_R_UP = 6, // Slope: right high, left low (/)

    TILE_CEIL_R = 7, // Ceiling: left high, right low (\)
    TILE_CEIL_L = 8, // Ceiling: left low, right high (/)
    TILE_GOAL1 = 9,  // Water girl's goal
    TILE_GOAL2 = 10
} tile_type_t;

typedef struct
{
    float x, y;               // ���Ͻ�λ�ã���λ����
    bool activated;           // ��ǰ״̬��true=���£�false=δ����
    uint8_t base_frame[2];    // ����֡��������
    uint8_t handle_frames[3]; // ���˶���֡�����С��ң�
    sprite_t base_sprites[4]; // ������3 tile����Ӧ3������
    // sprite_t handle_sprite;   // ��������
    sprite_t handle_sprite_left;
    sprite_t handle_sprite_right;
    int sprite_base_index;           // ���� index ��㣨���ʼ�����䣩
    bool left_entered[NUM_PLAYERS];  // ��¼ÿ����ɫ�Ƿ���������
    bool right_entered[NUM_PLAYERS]; // ��¼ÿ����ɫ�Ƿ����Ҳ����
} lever_t;

typedef struct
{
    float x, y;            // ���Ͻ�λ��
    float vy;              // ��ֱ�ٶȣ���Ϊ0��
    float min_y, max_y;    // �����˶���Χ
    bool moving_up;        // ��ǰ����
    bool active;           // �Ƿ����ڼ���/�˶�
    sprite_t sprites[4];   // ������ݵ�4�� tile
    int sprite_base_index; // ���� index ���
} elevator_t;

typedef struct
{
    float x, y; // ��ť���Ͻ��������꣨��ť�ϲ� sprite��
    sprite_t top_sprite;
    sprite_t base_left_sprite;
    sprite_t base_right_sprite;

    uint8_t sprite_index_base;
    uint8_t frame_top;
    uint8_t frame_base_left;
    uint8_t frame_base_right;
    float press_offset; // ��ǰѹ����ȣ���λ���أ�0~8��
    bool pressed;       // ���������ڹ����ж�
} button_t;

///////////////////////////////////////����////////////////////////////////////////////////////

extern player_t players[NUM_PLAYERS];
extern item_t items[NUM_ITEMS];
extern box_t boxes[NUM_BOXES];
extern lever_t levers[NUM_LEVERS];
extern elevator_t elevators[NUM_ELEVATORS];
extern button_t buttons[NUM_BUTTONS];
extern unsigned frame_counter; // ������ѭ���ж����֡������
extern const int tilemap[MAP_HEIGHT][MAP_WIDTH];

///////////////////////////////////////////////////////////////////////////////////////////
#endif // TYPEDEFS_H

// ��ʯ4 5 6 7 8 9
// ����10 11 12 13
// ���ݻ� 14 15 16 17
// ������ 18 19 20 21
// ���˻� 22 23 24 25
// ��ť   26 17 28
//        29 30 21
// ����0 1
// ˮŮ2 3