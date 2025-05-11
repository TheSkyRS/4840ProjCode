#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "sprite.h"

typedef enum
{
    PLAYER_FIREBOY,
    PLAYER_WATERGIRL
} player_type_t;

typedef enum
{
    STATE_IDLE,
    STATE_RUNNING,
    STATE_JUMPING,
    STATE_FALLING,
    STATE_DEAD
} player_state_t;

typedef struct
{
    float x, y;
    float vx, vy;
    bool on_ground;

    player_state_t state;
    player_type_t type;

    sprite_t upper_sprite;
    sprite_t lower_sprite;

    int frame_timer; // ����֡�л�����
    int frame_index; // ��ǰ֡��ţ��������߶�����
    bool was_on_slope_last_frame;
} player_t;

// ��ʼ����ɫ����ʼ���꣬sprite index����ɫ���ͣ�
void player_init(player_t *p, int x, int y,
                 uint8_t upper_index, uint8_t lower_index,
                 player_type_t type);

int get_frame_count(player_t *p, bool is_upper);

// ÿ֡��������
void player_handle_input(player_t *p, int player_index);
void player_update_physics(player_t *p);
void player_check_collision(player_t *p);
void player_update_sprite(player_t *p);
void adjust_to_slope_y(player_t *p);

#endif // PLAYER_H
