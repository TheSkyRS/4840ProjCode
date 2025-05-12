#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "hw_interact.h"
#include "player.h"
#include "joypad_input.h"
#include "sprite.h" // 新增

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

    write_ctrl(0x00000001); // 启动 VGA 控制器
    for (int i = 0; i < 32; i++)
    {
        write_sprite(i, 0, 0, 0, 0, 0); // disable=0，位置0，帧0
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
        // === 帧同步：只在每帧顶部 row==0 时执行一次 ===
        do
        {
            read_status(&col, &row);
        } while (row != 0);

        // === 1. 逻辑更新阶段 ===
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

        // === 2. 等待消隐区 ===
        do
        {
            read_status(&col, &row);
        } while (row < VACTIVE);

        // === 3. 写入 sprite 到 VGA ===
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            player_update_sprite(&players[i]);
        }
        for (int j = 0; j < NUM_ITEMS; j++)
        {
            item_update_sprite(&items[j]);
        }
    }

    // 不会到达，若后续有退出条件，可释放资源：
    input_handler_cleanup();
    close(vga_top_fd);
    return 0;
}
