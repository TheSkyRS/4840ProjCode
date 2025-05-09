// sprite_object.c
// ʵ�ֵ�ͼ���壨����ʯ�����ˡ����ݵȣ��Ļ����붯�������߼�

#include "sprite_object.h" // ����ṹ����ӿ�����
#include "vgasys.h"

/**
 * ���µ�������Ķ���֡������ж�֡��
 */
void update_object_animation(object_t *obj, float dt)
{
    if (obj->frame_count <= 1)
        return; // ��̬�������趯������

    obj->anim_timer += dt;
    if (obj->anim_timer >= obj->frame_interval)
    {
        obj->frame_id = (obj->frame_id + 1) % obj->frame_count;
        obj->anim_timer = 0.0f;
    }
}

/**
 * @brief ������ľ�����Ϣд�� sprite ��������
 *
 * @param obj           ָ������ṹ���ָ��
 * @param sprite_words  ���黺������
 * @param count         ��ǰд������ָ��
 * @param max_count     �������������
 */
void object_push_sprite(object_t *obj, uint32_t *sprite_words, int *count, int max_count)
{
    if (!obj || obj->collected || *count >= max_count)
        return;

    uint32_t word = make_attr_word(
        1, // enable
        0, // flip_horizontal = false
        (uint16_t)obj->x,
        (uint16_t)obj->y,
        obj->frame_id);

    sprite_words[(*count)++] = word;
}