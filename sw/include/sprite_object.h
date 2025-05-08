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
    float x, y;           // λ��
    int width, height;    // ��С
    int frame_id;         // ��ǰ֡���
    int frame_count;      // ����֡����
    float anim_timer;     // ��ǰ֡��ʱ��
    float frame_interval; // ÿ֡ʱ��
    bool active;          // �Ƿ񼤻�
    bool collected;       // �Ƿ��ռ�
    object_type_t type;   // ����
} object_t;

// �����е�ͼ����ͬ����Ⱦ��Ӳ�� Sprite ����
void sync_objects_to_hardware(object_t *objects, int num_objects, int sprite_offset);

#endif // SPRITE_OBJECT_H