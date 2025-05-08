// sprite_object.c
// ʵ�ֵ�ͼ���壨����ʯ�����ˡ����ݵȣ��Ļ����붯�������߼�

#include "sprite_object.h" // ����ṹ����ӿ�����
#include "hw_interface.h"  // ���鹹����д��Ӳ���ӿ�

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
 * �����е�ͼ����д�� Sprite ���С�
 * - �������ռ������壨������ʯ��
 * - ÿ��������ݴ���� sprite_offset д�벻ͬ��λ
 *
 * ����˵����
 * - objects���������飨object_t��
 * - num_objects������������
 * - sprite_offset����ʼд���λ��ţ��������ɫ��ͻ��
 */
void sync_objects_to_hardware(object_t *objects, int num_objects, int sprite_offset)
{
    for (int i = 0; i < num_objects; ++i)
    {
        object_t *obj = &objects[i];

        if (obj->collected) // ��������ѱ��ռ���������ʾ
            continue;

        // ����������Ӧ�� sprite ���ԣ�Ĭ�ϲ���ת��
        sprite_attr_t s = make_sprite(
            obj->x,            // ���� X
            obj->y,            // ���� Y
            obj->frame_id,     // ʹ�õ�ͼ��֡���
            obj->width,        // ������
            obj->height,       // ����߶�
            sprite_offset + i, // д���λ��ƫ�Ʊ������ɫ��ͻ��
            true,              // alive ��־����ʾ�� sprite
            false              // Ĭ�ϲ���ת��flip_horizontal��
        );

        // д��� sprite ����Ӧ��λ
        write_sprite(sprite_offset + i, &s);
    }
}
