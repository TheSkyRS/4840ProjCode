#include "sprite.h"
#include "hw_interact.h"

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
