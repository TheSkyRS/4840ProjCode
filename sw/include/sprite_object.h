// sprite_object.h
// ��Ϸ�����壨���ء����ߵȣ�ͳһ�Ľṹ������ӿ�

#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "hw_interface.h" // ���� sprite_attr_t �� make_sprite()
#include <stdbool.h>

// ��Ϸ��������ö��
typedef enum
{
    OBJ_DIAMOND, // �ռ����ʯ
    OBJ_LEVER,   // ���أ�����
    OBJ_ELEVATOR // ���أ�����ƽ̨
} object_type_t;

// ��Ϸ�����ڻ��Ƶ�����ṹ�壨�ǽ�ɫ��
typedef struct
{
    float x, y;         // ����λ��
    int width, height;  // ��ʾ�ߴ�
    int frame_id;       // ʹ�õĶ���֡���
    bool active;        // �Ƿ񼤻�������Ѵ�����
    bool collected;     // �Ƿ��ѱ��ռ�����������ʯ��
    object_type_t type; // ���ͱ�ʶ
} object_t;

// �����е�ͼ����ͬ����Ⱦ��Ӳ�� Sprite ����
void sync_objects_to_hardware(object_t *objects, int num_objects, int sprite_offset);

#endif // SPRITE_OBJECT_H