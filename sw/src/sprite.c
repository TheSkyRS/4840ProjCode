#include "sprite.h"
#include "hw_interact.h"
#include <math.h>              // 为 sinf 提供声明
extern unsigned frame_counter; // 引入主循环中定义的帧计数器
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

void item_init(item_t *item, float x, float y, uint8_t sprite_index, uint8_t frame_id)
{
    item->x = x;
    item->y = y;
    item->width = 16;
    item->height = 16;
    item->active = true;
    sprite_set(&item->sprite, sprite_index, 0);
    item->sprite.x = (uint16_t)x;
    item->sprite.y = (uint16_t)y;
    item->sprite.frame_id = frame_id;
    item->sprite.enable = true;
    sprite_update(&item->sprite);
}

void item_update_sprite(item_t *item)
{
    if (item->active)
    {
        float offset = 0.0f;
        if (item->float_anim)
        {
            offset = 0.2f * sinf((float)frame_counter * 0.2f + item->sprite.index); // 经验抖动频率和幅度
        }

        item->sprite.x = (uint16_t)item->x;
        item->sprite.y = (uint16_t)(item->y + offset);
        item->sprite.enable = true;
        sprite_update(&item->sprite);
    }
    else
    {
        sprite_clear(&item->sprite);
    }
}

bool check_overlap(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2)
{
    return (x1 < x2 + w2) && (x1 + w1 > x2) &&
           (y1 < y2 + h2) && (y1 + h1 > y2);
}