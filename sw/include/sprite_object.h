// sprite_object.h
// ��Ϸ�����壨���ء����ߵȣ�ͳһ�Ľṹ������ӿ�

#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H
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
void object_push_sprite(object_t *obj, uint32_t *sprite_words, int *count, int max_count);

#endif // SPRITE_OBJECT_H