#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "hw_interact.h"
#include "player.h"
#include "joypad_input.h"
#include "sprite.h" // 新增

#define NUM_PLAYERS 2
#define NUM_ITEMS 6
#define VACTIVE 480
unsigned frame_counter = 0; // 全局变量，每帧 ++
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

    item_init(&items[0], 0, 0, 4, BLUE_GEM_FRAME);
    item_place_on_tile(&items[0], 21, 26);
    items[0].sprite.frame_count = 1;
    items[0].sprite.frame_start = BLUE_GEM_FRAME;
    items[0].owner_type = ITEM_WATERGIRL_ONLY;
    items[0].float_anim = true;

    item_init(&items[1], 0, 0, 5, RED_GEM_FRAME);
    item_place_on_tile(&items[1], 29, 26);
    items[1].sprite.frame_count = 1;
    items[1].sprite.frame_start = RED_GEM_FRAME;
    items[1].owner_type = ITEM_FIREBOY_ONLY;
    items[1].float_anim = true;

    item_init(&items[2], 0, 0, 6, RED_GEM_FRAME);
    item_place_on_tile(&items[2], 6, 14);
    items[2].sprite.frame_count = 1;
    items[2].sprite.frame_start = RED_GEM_FRAME;
    items[2].owner_type = ITEM_FIREBOY_ONLY;
    items[2].float_anim = true;

    item_init(&items[3], 0, 0, 7, BLUE_GEM_FRAME);
    item_place_on_tile(&items[3], 23, 14);
    items[3].sprite.frame_count = 1;
    items[3].sprite.frame_start = BLUE_GEM_FRAME;
    items[3].owner_type = ITEM_WATERGIRL_ONLY;
    items[3].float_anim = true;

    item_init(&items[4], 0, 0, 8, BLUE_GEM_FRAME);
    item_place_on_tile(&items[4], 11, 7);
    items[4].sprite.frame_count = 1;
    items[4].sprite.frame_start = BLUE_GEM_FRAME;
    items[4].owner_type = ITEM_WATERGIRL_ONLY;
    items[4].float_anim = true;

    item_init(&items[5], 0, 0, 9, RED_GEM_FRAME);
    item_place_on_tile(&items[5], 1, 4);
    items[5].sprite.frame_count = 1;
    items[5].sprite.frame_start = RED_GEM_FRAME;
    items[5].owner_type = ITEM_FIREBOY_ONLY;
    items[5].float_anim = true;

    unsigned col = 0, row = 0;

    while (1)
    {
        frame_counter++;
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

                // 判断是否允许该角色收集
                if ((items[j].owner_type == ITEM_FIREBOY_ONLY && players[i].type != PLAYER_FIREBOY) ||
                    (items[j].owner_type == ITEM_WATERGIRL_ONLY && players[i].type != PLAYER_WATERGIRL))
                {
                    continue;
                }

                float pw = SPRITE_W_PIXELS;      // 宽度保持 16
                float ph = PLAYER_HITBOX_HEIGHT; // 实际参与碰撞的高度
                float px = players[i].x;
                float py = players[i].y + PLAYER_HITBOX_OFFSET_Y; // 跳过上方透明像素区域

                if (check_overlap(px, py, pw, ph,
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
