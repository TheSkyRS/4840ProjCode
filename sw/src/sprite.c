#include "sprite.h"
#include "hw_interact.h"

// ����ʯ
#define RED_GEM_FRAME ((uint8_t)44) // 0x2C00 >> 8 = 44

// ����ʯ
#define BLUE_GEM_FRAME ((uint8_t)45) // 0x2D00 >> 8 = 45

// ���˵��̣�2֡��
#define LEVER_BASE_FRAME ((uint8_t)46) // 0x2E00 >> 8 = 46

// ���˶�����3֡��
#define LEVER_ANIM_FRAME ((uint8_t)48) // 0x3000 >> 8 = 48

// ��ɫ��������4֡��
#define LIFT_YELLOW_FRAME ((uint8_t)51) // 0x3300 >> 8 = 51

// ��ɫ��ť��2֡��
#define BUTTON_PURPLE_FRAME ((uint8_t)55) // 0x3700 >> 8 = 55

// ��ɫ��������4֡��
#define LIFT_PURPLE_FRAME ((uint8_t)57) // 0x3900 >> 8 = 57

// ���ӣ�4֡��
#define BOX_FRAME ((uint8_t)61) // 0x3D00 >> 8 = 61

// ��ʼ�� sprite ������֡��
void sprite_set(sprite_t *s, uint8_t index, uint8_t frame_count)
{
    s->index = index;
    s->x = s->y = 0;
    s->frame_id = 0;
    s->flip = 0;
    s->enable = false;
    s->frame_count = frame_count;
}

// ����֡������ѭ����
void sprite_animate(sprite_t *s)
{
    if (s->frame_count > 0)
    {
        uint8_t rel = (s->frame_id - s->frame_start + 1) % s->frame_count;
        s->frame_id = s->frame_start + rel;
    }
}

// ���� sprite ��Ӳ��
void sprite_update(sprite_t *s)
{
    write_sprite(s->index, s->enable, s->flip, s->x, s->y, s->frame_id);
}

// �ر� sprite ��ʾ
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
        item->sprite.x = (uint16_t)item->x;
        item->sprite.y = (uint16_t)item->y;
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