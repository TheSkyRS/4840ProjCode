#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdbool.h>

#define SPRITE_H_PIXELS 16
#define SPRITE_W_PIXELS 16

typedef struct
{
    uint8_t index;
    uint8_t frame_id;
    uint8_t flip;
    uint16_t x, y;
    bool enable;
    uint8_t frame_count; // 总帧数
    uint8_t frame_start; // 起始帧 ID（默认 0）
} sprite_t;

typedef struct
{
    float x, y;
    float width, height;
    bool active;
    sprite_t sprite; // 封装已有 sprite 信息
} item_t;

bool check_overlap(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2);

// 初始化 sprite，设置 index 与帧数
void sprite_set(sprite_t *s, uint8_t index, uint8_t frame_count);

// 帧循环更新（frame_id++)
void sprite_animate(sprite_t *s);

// 写入 VGA
void sprite_update(sprite_t *s);

// 关闭显示
void sprite_clear(sprite_t *s);

void item_init(item_t *item, float x, float y, uint8_t sprite_index, uint8_t frame_id);
#endif
