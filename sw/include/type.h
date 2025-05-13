#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdint.h>
#include <stdbool.h>
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

// === box_t ===
typedef struct
{
    float x, y;
    float vx;
    bool active;
    sprite_t sprites[4];
    player_type_t pushing_player_type;
} box_t;

// === player_type_t ===
typedef enum
{
    PLAYER_FIREBOY,
    PLAYER_WATERGIRL
} player_type_t;

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

// === Tile ���� ===
typedef enum
{
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_FIRE = 2,
    TILE_WATER = 3,
    TILE_GOAL = 4,

    TILE_SLOPE_L_UP = 5, // ���棺����ҵͣ�\��
    TILE_SLOPE_R_UP = 6, // ���棺�Ҹ���ͣ�/��

    TILE_CEIL_R = 7, // �컨�壺����ҵͣ�\��
    TILE_CEIL_L = 8  // �컨�壺����Ҹߣ�/��
} tile_type_t;

//////////////////////////////////////����/////////////////////////////////////////////////////

#define NUM_ITEMS 6
#define VACTIVE 480
#define NUM_BOXES 1

#define SPRITE_H_PIXELS 16
#define SPRITE_W_PIXELS 16
#define BOX_PUSH_SPEED 1.0f
#define BOX_FRICTION 0.2f

// #define GRAVITY 0.4f
// #define JUMP_VELOCITY -5.0f
// #define MOVE_SPEED 1.5f
#define GRAVITY 0.2f
#define JUMP_VELOCITY -10.0f
#define MOVE_SPEED 2.0f

#define MAX_FRAME_TIMER 6 // ���ƶ����л��ٶ�
#define PLAYER_HEIGHT_PIXELS 28
#define PLAYER_HITBOX_HEIGHT 24
#define PLAYER_HITBOX_OFFSET_Y 4
#define NUM_PLAYERS 2

// === ��ͼ���� ===
#define MAP_WIDTH 40  // tile ��������
#define MAP_HEIGHT 30 // tile ��������
#define TILE_SIZE 16  // ÿ�� tile ���ش�С����λ��px��

#define PLAYER_NONE -1

///////////////////////////////////////����////////////////////////////////////////////////////

extern player_t players[NUM_PLAYERS];
extern item_t items[NUM_ITEMS];
extern box_t boxes[NUM_BOXES];
extern unsigned frame_counter; // ������ѭ���ж����֡������
extern const int tilemap[MAP_HEIGHT][MAP_WIDTH];

///////////////////////////////////////////////////////////////////////////////////////////
#endif // TYPEDEFS_H
