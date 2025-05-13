#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdbool.h>
#include "player.h"
#include "tilemap.h"
#include "type.h"
extern box_t boxes[NUM_BOXES]; //  告诉编译器 boxes[] 是 main.c 提供的全局定义

// 红钻石
#define RED_GEM_FRAME ((uint8_t)44) // 0x2C00 >> 8 = 44

// 蓝钻石
#define BLUE_GEM_FRAME ((uint8_t)45) // 0x2D00 >> 8 = 45

// 拉杆底盘（2帧）
#define LEVER_BASE_FRAME ((uint8_t)46) // 0x2E00 >> 8 = 46

// 拉杆动画（3帧）
#define LEVER_ANIM_FRAME ((uint8_t)48) // 0x3000 >> 8 = 48

// 黄色升降机（4帧）
#define LIFT_YELLOW_FRAME ((uint8_t)51) // 0x3300 >> 8 = 51

// 紫色按钮（2帧）
#define BUTTON_PURPLE_FRAME ((uint8_t)55) // 0x3700 >> 8 = 55

// 紫色升降机（4帧）
#define LIFT_PURPLE_FRAME ((uint8_t)57) // 0x3900 >> 8 = 57

// 箱子（4帧）
#define BOX_FRAME ((uint8_t)61) // 0x3D00 >> 8 = 61

// 初始化 sprite，设置 index 与帧数
void sprite_set(sprite_t *s, uint8_t index, uint8_t frame_count);

// 帧循环更新（frame_id++)
void sprite_animate(sprite_t *s);

void item_update_sprite(item_t *item);

// 写入 VGA
void sprite_update(sprite_t *s);

// 关闭显示
void sprite_clear(sprite_t *s);

void item_init(item_t *item, float x, float y, uint8_t sprite_index, uint8_t frame_id);

void box_init(box_t *b, int tile_x, int tile_y, int sprite_base_index, uint8_t frame_id);
void box_try_push(box_t *box, const player_t *player);
void box_update_position(box_t *box, player_t *players);
void box_update_sprite(box_t *b);

bool is_box_blocked(float x, float y, float w, float h);
bool check_overlap(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2);

void lever_init(lever_t *lvr, float tile_x, float tile_y, uint8_t sprite_index_base);
void lever_update(lever_t *lvr, const player_t *players);

void elevator_init(elevator_t *elv, float tile_x, float tile_y, float min_tile_y, float max_tile_y, uint8_t sprite_index_base, uint8_t frame_index);
bool is_elevator_blocked(float x, float y, float w, float h, float *vy_out);
void elevator_update(elevator_t *elv, bool go_up, player_t *players);

void button_init(button_t *btn, float tile_x, float tile_y, uint8_t sprite_index_base);

void button_update(button_t *btn, const player_t *players);

#endif
