#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "hw_interact.h"
#include "player.h"
#include "joypad_input.h"

#define NUM_PLAYERS 2
#define VACTIVE 480

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
    player_init(&players[0], 64, 352, 0, 1, PLAYER_FIREBOY);
    player_init(&players[1], 64, 416, 2, 3, PLAYER_WATERGIRL);

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
    }

    // ���ᵽ����������˳����������ͷ���Դ��
    input_handler_cleanup();
    close(vga_top_fd);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// #include <stdio.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include "hw_interact.h"
// #include "player.h"
// #include "joypad_input.h"

// #define NUM_PLAYERS 2

// int main()
// {
//     if ((vga_top_fd = open("/dev/vga_top", O_RDWR)) == -1)
//     {
//         fprintf(stderr, "Error: cannot open /dev/vga_top\n");
//         return -1;
//     }

//     write_ctrl(0x00000002); // ���� VGA ������
//     input_handler_init();

//     player_t players[NUM_PLAYERS];
//     player_init(&players[0], 100, 400, 0, 1, PLAYER_FIREBOY);
//     player_init(&players[1], 200, 400, 2, 3, PLAYER_WATERGIRL);

//     unsigned col = 0, row = 0;

//     while (1)
//     {
//         read_status(&col, &row);

//         if (row < VACTIVE)
//         {
//             // �ڿɼ����������У����� + ������
//             for (int i = 0; i < NUM_PLAYERS; i++)
//             {
//                 player_handle_input(&players[i], i);
//                 player_update_physics(&players[i]);
//             }
//         }
//         else
//         {
//             // ���������������� sprite д��
//             for (int i = 0; i < NUM_PLAYERS; i++)
//             {
//                 player_update_sprite(&players[i]);
//             }
//         }
//     }

//     input_handler_cleanup();
//     close(vga_top_fd);
//     return 0;
// }
