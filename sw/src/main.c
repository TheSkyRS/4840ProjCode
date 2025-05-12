#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "hw_interact.h"
#include "player.h"
#include "joypad_input.h"
#include "sprite.h" // ����

#define NUM_PLAYERS 2
#define NUM_ITEMS 2
#define VACTIVE 480
extern const uint8_t RED_GEM_FRAME;       // 44
extern const uint8_t BLUE_GEM_FRAME;      // 45
extern const uint8_t LEVER_BASE_FRAME;    // 46
extern const uint8_t LEVER_ANIM_FRAME;    // 48
extern const uint8_t LIFT_YELLOW_FRAME;   // 51
extern const uint8_t BUTTON_PURPLE_FRAME; // 55
extern const uint8_t LIFT_PURPLE_FRAME;   // 57
extern const uint8_t BOX_FRAME;           // 61

int main()
{
    if ((vga_top_fd = open("/dev/vga_top", O_RDWR)) == -1)
    {
        fprintf(stderr, "Error: cannot open /dev/vga_top\n");
        return -1;
    }

    write_ctrl(0x00000001); // ���� VGA ������
    for (int i = 0; i < 32; i++)
    {
        write_sprite(i, 0, 0, 0, 0, 0); // disable=0��λ��0��֡0
    }
    input_handler_init();

    player_t players[NUM_PLAYERS];
    player_init(&players[0], 64, 360, 0, 1, PLAYER_FIREBOY);
    player_init(&players[1], 64, 420, 2, 3, PLAYER_WATERGIRL);

    item_t items[NUM_ITEMS];
    item_init(&items[0], 416, 464, 10, RED_GEM_FRAME);
    items[0].sprite.frame_count = 1;
    items[0].sprite.frame_start = RED_GEM_FRAME;

    item_init(&items[1], 112, 176, 11, BLUE_GEM_FRAME);
    items[1].sprite.frame_count = 1;
    items[1].sprite.frame_start = BLUE_GEM_FRAME;

    unsigned col = 0, row = 0;

    while (1)
    {
        // === ֡ͬ����ֻ��ÿ֡���� row==0 ʱִ��һ�� ===
        do
        {
            read_status(&col, &row);
        } while (row != 0);

        // === 1. �߼����½׶� ===
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            player_handle_input(&players[i], i);
            player_update_physics(&players[i]);

            for (int j = 0; j < NUM_ITEMS; j++)
            {
                if (!items[j].active)
                    continue;

                float pw = SPRITE_W_PIXELS;
                float ph = SPRITE_H_PIXELS;

                if (check_overlap(players[i].x, players[i].y, pw, ph,
                                  items[j].x, items[j].y, items[j].width, items[j].height))
                {
                    items[j].active = false;
                }
            }
        }

        // === 2. �ȴ������� ===
        do
        {
            read_status(&col, &row);
        } while (row < VACTIVE);

        // === 3. д�� sprite �� VGA ===
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            player_update_sprite(&players[i]);
        }
        for (int j = 0; j < NUM_ITEMS; j++)
        {
            item_update_sprite(&items[j]);
        }
    }

    // ���ᵽ����������˳����������ͷ���Դ��
    input_handler_cleanup();
    close(vga_top_fd);
    return 0;
}
