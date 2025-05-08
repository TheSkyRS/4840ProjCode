// hw_interface.h
// Ӳ�����ƽӿڣ����� BRAM sprite ����ͨ�þ��鹹��

#ifndef HW_INTERFACE_H
#define HW_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_SPRITES 64

// ����ṹ�� flags��λ��
typedef union
{
    struct
    {
        uint8_t alive : 1;           // �Ƿ���ʾ�� sprite
        uint8_t flip_horizontal : 1; // �Ƿ�ˮƽ��ת���������ҳ���
        uint8_t reserved : 6;        // ����λ
    } bits;
    uint8_t value;
} sprite_flags_t;

// �������Խṹ�壨���� BRAM ��
typedef struct
{
    uint16_t x, y;         // ����λ��
    uint8_t frame_id;      // ʹ��ͼ��֡���
    uint8_t priority;      // ��ʾ���ȼ���Сֵ���ϣ�
    uint8_t width, height; // ������
    sprite_flags_t flags;  // ��־λ���Ƿ���ʾ����ת�ȣ�
} sprite_attr_t;

// ��Ӳ��д�������� sprite ��MAX_SPRITES ����
void update_sprite_table(sprite_attr_t *sprites, int count);

// ��д�뵥�� sprite�����ڿ��ٲ��ԣ�
void write_sprite(int index, const sprite_attr_t *s);

// ����һ�� sprite ���Խṹ��ͨ�ú����������ڽ�ɫ�����壩
sprite_attr_t make_sprite(float x, float y, int frame_id,
                          int width, int height,
                          int priority, bool alive, bool facing_right);

#endif // HW_INTERFACE_H
