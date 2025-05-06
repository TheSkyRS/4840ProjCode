// character.c
// ��ɫ�����߼�ģ��ʵ�֣��˶����ơ��������š�����ͬ��

#include "character.h"     // ������ɫ�ṹ���뺯������
#include "hw_interface.h"  // ����������ƺ�����Ӳ���ӿڶ���
#include "input_handler.h" // ����������ƵĶ����뺯��

// === �������Ʋ��� ===
#define NUM_HEAD_FRAMES 8   // ͷ������֡����
#define NUM_BODY_FRAMES 8   // ���嶯��֡����
#define FRAME_INTERVAL 0.1f // ÿ֡��������ʱ�䣨�룩
#define HEAD_OFFSET_Y 0     // ͷ����Խ�ɫλ�õ� Y ƫ��
#define BODY_OFFSET_Y 12    // ������Խ�ɫλ�õ� Y ƫ��

// ���½�ɫ����֡�����ݵ�ǰ״̬����֡���䣩
void update_character_animation(character_t *ch, float delta_time)
{
    int start_frame_head = 0, end_frame_head = 0; // ͷ������֡��Χ
    int start_frame_body = 0, end_frame_body = 0; // ���嶯��֡��Χ

    // ���ݽ�ɫ״̬ѡ���Ӧ�Ķ���֡��Χ
    switch (ch->state)
    {
    case STATE_IDLE: // վ��״̬ʹ��ǰ��֡
        start_frame_head = 0;
        end_frame_head = 1;
        start_frame_body = 0;
        end_frame_body = 1;
        break;
    case STATE_MOVING_LEFT:
    case STATE_MOVING_RIGHT: // ����״̬ʹ��֡2~5
        start_frame_head = 2;
        end_frame_head = 3;
        start_frame_body = 2;
        end_frame_body = 5;
        break;
    case STATE_JUMPING:
    case STATE_FALLING: // ��Ծ/����״̬�̶�֡6
        start_frame_head = 6;
        end_frame_head = 6;
        start_frame_body = 6;
        end_frame_body = 6;
        break;
    case STATE_DEAD: // ����״̬ʹ��֡7
        start_frame_head = 7;
        end_frame_head = 7;
        start_frame_body = 7;
        end_frame_body = 7;
        break;
    default: // Ĭ��������ֵ�һ֡
        start_frame_head = 0;
        end_frame_head = 0;
        start_frame_body = 0;
        end_frame_body = 0;
        break;
    }

    // === ͷ���������� ===
    ch->anim_timer_head += delta_time;         // �ۼ�ͷ������ʱ��
    if (ch->anim_timer_head >= FRAME_INTERVAL) // �����л�ʱ��
    {
        ch->frame_head++; // �л�����һ֡
        if (ch->frame_head > end_frame_head || ch->frame_head < start_frame_head)
            ch->frame_head = start_frame_head; // ������Χ��ص���ʼ֡
        ch->anim_timer_head = 0.0f;            // ���ü�ʱ��
    }

    // === ���嶯������ ===
    ch->anim_timer_body += delta_time;
    if (ch->anim_timer_body >= FRAME_INTERVAL)
    {
        ch->frame_body++;
        if (ch->frame_body > end_frame_body || ch->frame_body < start_frame_body)
            ch->frame_body = start_frame_body;
        ch->anim_timer_body = 0.0f;
    }
}

// ����������½�ɫ״̬���ٶ�
void update_character_state(character_t *ch, game_action_t input, float dt)
{
    if (!ch->alive) // ������ɫ������
        return;

    // === ˮƽ�˶����� ===
    if (input == ACTION_MOVE_LEFT)
    {
        ch->vx = -MOVE_SPEED;     // �����ƶ�
        ch->facing_right = false; // ����
        ch->state = STATE_MOVING_LEFT;
    }
    else if (input == ACTION_MOVE_RIGHT)
    {
        ch->vx = MOVE_SPEED;     // �����ƶ�
        ch->facing_right = true; // ����
        ch->state = STATE_MOVING_RIGHT;
    }
    else // �޷�������
    {
        ch->vx = 0;
        ch->state = STATE_IDLE;
    }

    // === ��ֱ�˶����� ===
    if (input == ACTION_JUMP)
    {
        ch->vy = JUMP_VELOCITY; // �����ٶ�
        ch->state = STATE_JUMPING;
    }
    else
    {
        ch->vy += GRAVITY * dt; // ʼ��ʩ������
        if (ch->vy > 0.1f && ch->state != STATE_JUMPING)
            ch->state = STATE_FALLING; // ����״̬ʶ��
    }

    update_character_animation(ch, dt); // �����¶���֡
}

// �����ٶȸ��½�ɫλ�ã��򵥻��֣�
void update_character_position(character_t *ch, float dt)
{
    if (!ch->alive)
        return;
    ch->x += ch->vx * dt; // ����ˮƽλ��
    ch->y += ch->vy * dt; // ���´�ֱλ��
}

// �����н�ɫ��ͷ�������� sprite ����д��Ӳ�� BRAM ��
void sync_characters_to_hardware(character_t *characters, int num_characters)
{
    sprite_attr_t sprites[MAX_SPRITES]; // �������ʱ����
    int count = 0;                      // ��д�� sprite ����

    for (int i = 0; i < num_characters; ++i)
    {
        character_t *ch = &characters[i];
        if (!ch->alive)
            continue; // ����������ɫ

        // === д��ͷ�� sprite ===
        if (count < MAX_SPRITES)
        {
            sprite_attr_t head = make_sprite(
                ch->x, ch->y + HEAD_OFFSET_Y,
                ch->frame_head, ch->width, ch->height / 2,
                i * 2 + 1, true, ch->facing_right);
            sprites[count++] = head;
        }

        // === д������ sprite ===
        if (count < MAX_SPRITES)
        {
            sprite_attr_t body = make_sprite(
                ch->x, ch->y + BODY_OFFSET_Y,
                ch->frame_body, ch->width, ch->height / 2,
                i * 2, true, ch->facing_right);
            sprites[count++] = body;
        }
    }

    // ���δʹ�õ� sprite ��λ����ֹ��Ӱ
    for (int i = count; i < MAX_SPRITES; ++i)
        sprites[i].flags.value = 0;

    update_sprite_table(sprites, MAX_SPRITES); // д�� sprite ��Ӳ��
}

// ��ʼ��һ����ɫʵ����λ�á����͡�Ĭ��״̬��
void init_character(character_t *ch, float x, float y, character_type_t type)
{
    ch->x = x;
    ch->y = y;
    ch->vx = 0;
    ch->vy = 0;
    ch->width = 16;  // Ĭ�Ͽ�ȣ��ɸ���ʵ�ʾ��������
    ch->height = 32; // Ĭ�ϸ߶�
    ch->frame_head = 0;
    ch->frame_body = 0;
    ch->anim_timer_head = 0.0f;
    ch->anim_timer_body = 0.0f;
    ch->facing_right = true;
    ch->alive = true;
    ch->state = STATE_IDLE;
    ch->type = type;

    // ���û������ȼ�����ˮŮ���ڻ��к��ϲ㣩
    ch->priority_offset = (type == TYPE_WATERGIRL) ? 1 : 0;
}
