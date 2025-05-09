#define _DEFAULT_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h> // �ṩ gettimeofday
#include <unistd.h>   // �ṩ usleep

#include "vga_top.h"        // ioctl ָ����ṹ��
#include "vgasys.h"         // VGA ϵͳ�ӿڷ�װ��write_sprite �ȣ�
#include "character.h"      // character_t, ��ɫ�����߼�
#include "sprite_object.h"  // object_t, ��ͼ���嶯��
#include "collision.h"      // ��ײ���
#include "keyboard_input.h" // ������루get_player_action��

// ���黺������С��VGA ֧����� 32 �� sprite��
#define MAX_SPRITES 32

// ֡�ʿ��Ʋ�������λ��΢�룩
#define TARGET_FPS 60
#define FRAME_TIME_US (1000000 / TARGET_FPS)

character_t characters[2];
int num_characters = 2;

object_t objects[4];
int num_objects = 4;

// ����д�뻺�����飨ÿ֡�������������д�룩
uint32_t sprite_words[MAX_SPRITES];
int sprite_count = 0;

// ���ƼĴ�������
#define TILEMAP_ID 0       // ��ǰ��ͼ��ţ��������л��ؿ���
#define COLLISION_ENABLE 0 // Ӳ����ײ���أ���ѡ��

/**
 * ��Ϸ��ѭ������֡���У���Ϊ�ɼ��ڣ������߼����������ڣ�д��Ӳ����
 */
void run_game_loop()
{
    struct timeval frame_start, frame_end;
    float delta_time = 1.0f / TARGET_FPS; // ÿ֡ʱ�䣬��
    bool running = true;

    unsigned col = 0, row = 0; // VGA ��ǰɨ��λ��

    while (running)
    {
        // ��¼��֡��ʼʱ��
        gettimeofday(&frame_start, NULL);

        // ��ѯ VGA ״̬�Ĵ��������ɨ��λ�ã�vcount��
        read_status(&col, &row);

        if (row < 480)
        {
            // ================== �ɼ����������ɫ�������߼� ==================

            sprite_count = 0; // ��վ��黺����

            // ��ɫ״̬���£�������ٶȡ���ײ������
            for (int i = 0; i < num_characters; ++i)
            {
                character_t *ch = &characters[i];
                game_action_t input = get_player_action(i);

                update_character_state(ch, input, delta_time);
                handle_character_tile_collision(ch, delta_time);
                handle_character_object_collision(ch, objects, num_objects);
            }

            // ��ɫ֮�����ʾ�㼶������˭����˭��
            if (num_characters >= 2)
            {
                handle_character_vs_character(&characters[0], &characters[1]);
            }

            // === ����ÿ����ɫ���������飨ͷ�� + ���壩 ===
            for (int i = 0; i < num_characters; ++i)
            {
                character_t *ch = &characters[i];
                if (!ch->alive)
                    continue;

                if (sprite_count < MAX_SPRITES)
                    sprite_words[sprite_count++] = make_attr_word(1, !ch->facing_right,
                                                                  (uint16_t)ch->x, (uint16_t)(ch->y), ch->frame_head);

                if (sprite_count < MAX_SPRITES)
                    sprite_words[sprite_count++] = make_attr_word(1, !ch->facing_right,
                                                                  (uint16_t)ch->x, (uint16_t)(ch->y + 12), ch->frame_body);
            }

            // === �������嶯������д�뻺�徫�� ===
            for (int i = 0; i < num_objects; ++i)
            {
                object_t *obj = &objects[i];
                if (obj->collected)
                    continue;

                update_object_animation(obj, delta_time);

                if (sprite_count < MAX_SPRITES)
                    sprite_words[sprite_count++] = make_attr_word(1, 0,
                                                                  (uint16_t)obj->x, (uint16_t)obj->y, obj->frame_id);
            }
        }
        else
        {
            // ================== ��������vcount �� 480����д��Ӳ���Ĵ��� ==================

            // д����ƼĴ�������ͼ��š���ײ���صȣ�
            uint32_t ctrl_value = (TILEMAP_ID & 0x3) | ((COLLISION_ENABLE & 0x1) << 2);
            write_ctrl(ctrl_value);

            // д�뾫�����Ե�Ӳ��
            for (int i = 0; i < sprite_count; ++i)
            {
                write_sprite(i, 1,
                             (sprite_words[i] >> 30) & 1,     // flip
                             (sprite_words[i] >> 8) & 0x3FF,  // x
                             (sprite_words[i] >> 18) & 0x1FF, // y
                             sprite_words[i] & 0xFF);         // frame
            }

            // ������ྫ��ۣ������Ӱ
            for (int i = sprite_count; i < MAX_SPRITES; ++i)
            {
                write_sprite(i, 0, 0, 0, 0, 0);
            }
        }

        // ================== ֡�ʿ��ƣ����٣� ==================

        gettimeofday(&frame_end, NULL);

        long duration_us = (frame_end.tv_sec - frame_start.tv_sec) * 1000000 +
                           (frame_end.tv_usec - frame_start.tv_usec);

        delta_time = duration_us / 1000000.0f;
        if (delta_time > 0.1f)
            delta_time = 0.1f; // ��ֹ��֡ʱ delta_time ��ը
        if (delta_time <= 0)
            delta_time = 1.0f / TARGET_FPS;

        // ��֡ʱ��̫�������� sleep ����ʱ��
        if (duration_us < FRAME_TIME_US)
        {
            usleep(FRAME_TIME_US - duration_us);
        }
    }
}
