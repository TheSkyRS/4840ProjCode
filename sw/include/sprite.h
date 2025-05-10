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

// 初始化 sprite，设置 index 与帧数
void sprite_set(sprite_t *s, uint8_t index, uint8_t frame_count);

// 帧循环更新（frame_id++)
void sprite_animate(sprite_t *s);

// 写入 VGA
void sprite_update(sprite_t *s);

// 关闭显示
void sprite_clear(sprite_t *s);

#endif
