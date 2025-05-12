#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "sprite.h"
#include "type.h"
// #define MOVE_SPEED 1.5f
#define GRAVITY 0.2f
#define JUMP_VELOCITY -10.0f
#define MOVE_SPEED 1.0f
#define MAX_FRAME_TIMER 6 // 控制动画切换速度
#define PLAYER_HEIGHT_PIXELS 28
#define PLAYER_HITBOX_HEIGHT 24
#define PLAYER_HITBOX_OFFSET_Y 4
#define NUM_PLAYERS 2

// 初始化角色（初始坐标，sprite index，角色类型）
void player_init(player_t *p, int x, int y,
                 uint8_t upper_index, uint8_t lower_index,
                 player_type_t type);

int get_frame_count(player_t *p, bool is_upper);

// 每帧调用流程
void player_handle_input(player_t *p, int player_index);
void player_update_physics(player_t *p);
void player_check_collision(player_t *p);
void player_update_sprite(player_t *p);
void adjust_to_slope_y(player_t *p);
void debug_print_player_state(player_t *p, const char *tag);
void adjust_to_platform_y(player_t *p);
#endif // PLAYER_H
