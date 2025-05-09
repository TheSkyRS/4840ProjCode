// character.h
// ���ƽ�ɫ��Ϊ�붯����ģ��ͷ�ļ���Fireboy & Watergirl��

#ifndef CHARACTER_H
#define CHARACTER_H
#include "keyboard_input.h"
#include <stdbool.h>
#include <stdint.h>

// === ��ɫ��Ϊ���� ===
#define MOVE_SPEED 60.0f      // ˮƽ�ƶ��ٶȣ�����/�룩
#define JUMP_VELOCITY -140.0f // �����ٶȣ���ֵ���ϣ�
#define GRAVITY 400.0f        // �������ٶ�

// === �������Ʋ��� ===
#define NUM_HEAD_FRAMES 8   // ͷ��֡����
#define NUM_BODY_FRAMES 8   // ����֡����
#define FRAME_INTERVAL 0.1f // ÿ֡��������ʱ�䣨�룩

// ö�٣���ɫ��ǰ����״̬�����ڶ���ѡ������Ϊ���ƣ�
typedef enum
{
    STATE_IDLE,         // ��ֹվ��
    STATE_MOVING_LEFT,  // �����ƶ�
    STATE_MOVING_RIGHT, // �����ƶ�
    STATE_JUMPING,      // ��Ծ��
    STATE_FALLING,      // ������
    STATE_DEAD          // ���������ɿ��ƣ�
} character_state_t;

// ö�٣���ɫ���
typedef enum
{
    TYPE_FIREBOY,  // ���к�
    TYPE_WATERGIRL // ˮŮ��
} character_type_t;

// ���ṹ�壺��ɫ����Ϊ�����붯��״̬
typedef struct
{
    float x, y;              // ��ǰ���꣨���Ͻǣ�
    float vx, vy;            // ��ǰ�ٶȣ�����/�룩
    int width, height;       // �ߴ���Ϣ��������Ⱦ/��ײ��
    int frame_head;          // ��ǰͷ������֡���
    int frame_body;          // ��ǰ���嶯��֡���
    float anim_timer_head;   // ͷ������֡��ʱ��
    float anim_timer_body;   // ���嶯��֡��ʱ��
    bool facing_right;       // �Ƿ����Ҳ�
    bool alive;              // �Ƿ�������Ϸ�У�δ������
    int priority_offset;     // ������Ⱦ���ȼ�ƫ�ƣ�������ײʱ��ͼ����ƣ�
    character_state_t state; // ��ǰ����״̬��վ�����ƶ�����Ծ�ȣ�
    character_type_t type;   // Fireboy or Watergirl
} character_t;
// ��ʼ����ɫ����
void init_character(character_t *ch, float x, float y, character_type_t type);

// ÿ֡���ã���ɫ���롢״̬���������������ۺϸ����߼�
void update_character_state(character_t *ch, game_action_t input, float dt);

// ÿ֡���ã������ٶȸ���λ�ã��򵥻��֣�
void update_character_position(character_t *ch, float dt);

// ÿ֡���ã����ݽ�ɫ״̬���¶���֡��ͷ��������ֿ���
void update_character_animation(character_t *ch, float delta_time);

/**
 * ����ɫ����������֡��ͷ�������壩д�� sprite ��������
 *
 * @param ch           ָ���ɫ�ṹ���ָ��
 * @param sprite_words �������
 * @param count        ��ǰд�������ᱻ�޸ģ�
 * @param max_count    �������
 */
void character_push_sprites(character_t *ch, uint32_t *sprite_words, int *count, int max_count);

#endif // CHARACTER_H
