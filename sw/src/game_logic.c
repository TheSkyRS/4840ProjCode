// game_logic.c
// ��Ϸ������ģ�飬ʵ��ÿ֡ͳһ���ƽ�ɫ�����塢�����߼�

#include "game_logic.h"    // ������ģ��ͷ�ļ�
#include "character.h"     // ��ɫ���º���
#include "sprite_object.h" // ��ͼ����ͬ������
#include "collision.h"     // ������ײ�߼�
#include "input_handler.h" // ��������ȡ

/**
 * ÿ֡���ã��������н�ɫ���ͼ�����״̬����ʾ
 *
 * ������
 * - characters�����н�ɫ����
 * - num_characters����ɫ����
 * - objects����ͼ�������飨��ʯ�����أ�
 * - num_objects����ͼ��������
 * - delta_time����ǰ֡ʱ�䣨�룩
 */
void update_game_state(character_t *characters, int num_characters,
                       object_t *objects, int num_objects,
                       float delta_time)
{
    // === ���� 1������ÿ����ɫ ===
    for (int i = 0; i < num_characters; ++i)
    {
        character_t *ch = &characters[i];

        game_action_t input = get_player_action(i);                  // ��ȡ�������
        update_character_state(ch, input, delta_time);               // ���¶���״̬
        handle_character_tile_collision(ch, delta_time);             // ������ײ����
        handle_character_object_collision(ch, objects, num_objects); // ��������ײ
    }

    // === ���� 2����ɫ֮�����Դ���ֻ֧��2����ɫ�� ===
    if (num_characters >= 2)
    {
        handle_character_vs_character(&characters[0], &characters[1]);
    }

    // === ���� 3���������嶯�� ===
    for (int i = 0; i < num_objects; ++i)
    {
        if (!objects[i].collected)
        {
            update_object_animation(&objects[i], delta_time);
        }
    }

    // === ���� 4����Ⱦͬ�� ===
    sync_characters_to_hardware(characters, num_characters);
    sync_objects_to_hardware(objects, num_objects, 20); // �Ӳ�λ20��ʼ��������
}

/**
 * ��ѡ�ӿڣ�����ȫ��״̬��Ӳ��ͬ����������ܡ�LEDָʾ�Ƶ�
 */
void sync_game_state_to_hardware(game_state_t state)
{
    // ��ǰδʵ�֣�����չ���ƼƷְ塢����ֵ������״̬��
}
