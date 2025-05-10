#include "sprite.h"
#include "hw_interact.h"

// 初始化 sprite 索引和帧数
void sprite_set(sprite_t *s, uint8_t index, uint8_t frame_count)
{
    s->index = index;
    s->x = s->y = 0;
    s->frame_id = 0;
    s->flip = 0;
    s->enable = false;
    s->frame_count = frame_count;
}

// 动画帧递增（循环）
void sprite_animate(sprite_t *s)
{
    if (s->frame_count > 0)
    {
        uint8_t rel = (s->frame_id - s->frame_start + 1) % s->frame_count;
        s->frame_id = s->frame_start + rel;
    }
}

// 更新 sprite 到硬件
void sprite_update(sprite_t *s)
{
    write_sprite(s->index, s->enable, s->flip, s->x, s->y, s->frame_id);
}

// 关闭 sprite 显示
void sprite_clear(sprite_t *s)
{
    s->enable = false;
    sprite_update(s);
}
