#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "hw_interact.h"
#include "player.h"
#include "joypad_input.h"
#include "sprite.h" // 新增
#include "type.h"
#include <time.h>

player_t players[NUM_PLAYERS];
item_t items[NUM_ITEMS];
box_t boxes[NUM_BOXES];
lever_t levers[NUM_LEVERS];
elevator_t elevators[NUM_ELEVATORS];
button_t buttons[NUM_BUTTONS];
unsigned frame_counter = 0;

int main()
{
    if ((vga_top_fd = open("/dev/vga_top", O_RDWR)) == -1)
    {
        fprintf(stderr, "Error: cannot open /dev/vga_top\n");
        return -1;
    }
Logo:
    input_handler_init();
    for (int i = 0; i < 32; i++)
    {
        write_sprite(i, 0, 0, 0, 0, 0); // disable=0，位置0，帧0
    }
    while (1)
    {
        set_map_and_audio(0, 0, 0); // 启动 VGA 控制器
        for (int i = 0; i < NUM_PLAYERS; i++)
        {
            game_action_t action = get_player_action(i);
            if (action != ACTION_NONE)
            {
            }
        }
    }
    // debug_draw_test_sprites();
Game:
    set_map_and_audio(1, 1, 0);
    input_handler_init();
    // player_init(&players[0], 64, 360, 0, 1, PLAYER_FIREBOY);
    player_init(&players[1], 64, 420, 2, 3, PLAYER_WATERGIRL);

    player_init(&players[0], 368, 224, 0, 1, PLAYER_FIREBOY);
    // player_init(&players[1], 320, 152, 2, 3, PLAYER_WATERGIRL);

    item_init(&items[0], 0, 0, 4, BLUE_GEM_FRAME);
    item_place_on_tile(&items[0], 21, 26);
    items[0].sprite.frame_count = 1;
    items[0].sprite.frame_start = BLUE_GEM_FRAME;
    items[0].owner_type = ITEM_WATERGIRL_ONLY;
    items[0].float_anim = true;
    items[0].width = 12;  // 碰撞箱宽度
    items[0].height = 12; // 碰撞箱高度

    item_init(&items[1], 0, 0, 5, RED_GEM_FRAME);
    item_place_on_tile(&items[1], 29, 26);
    items[1].sprite.frame_count = 1;
    items[1].sprite.frame_start = RED_GEM_FRAME;
    items[1].owner_type = ITEM_FIREBOY_ONLY;
    items[1].float_anim = true;
    items[1].width = 12;  // 碰撞箱宽度
    items[1].height = 12; // 碰撞箱高度

    item_init(&items[2], 0, 0, 6, RED_GEM_FRAME);
    item_place_on_tile(&items[2], 6, 14);
    items[2].sprite.frame_count = 1;
    items[2].sprite.frame_start = RED_GEM_FRAME;
    items[2].owner_type = ITEM_FIREBOY_ONLY;
    items[2].float_anim = true;
    items[2].width = 12;  // 碰撞箱宽度
    items[2].height = 12; // 碰撞箱高度

    item_init(&items[3], 0, 0, 7, BLUE_GEM_FRAME);
    item_place_on_tile(&items[3], 23, 14);
    items[3].sprite.frame_count = 1;
    items[3].sprite.frame_start = BLUE_GEM_FRAME;
    items[3].owner_type = ITEM_WATERGIRL_ONLY;
    items[3].float_anim = true;
    items[3].width = 12;  // 碰撞箱宽度
    items[3].height = 12; // 碰撞箱高度

    item_init(&items[4], 0, 0, 8, BLUE_GEM_FRAME);
    item_place_on_tile(&items[4], 11, 7);
    items[4].sprite.frame_count = 1;
    items[4].sprite.frame_start = BLUE_GEM_FRAME;
    items[4].owner_type = ITEM_WATERGIRL_ONLY;
    items[4].float_anim = true;
    items[4].width = 12;  // 碰撞箱宽度
    items[4].height = 12; // 碰撞箱高度

    item_init(&items[5], 0, 0, 9, RED_GEM_FRAME);
    item_place_on_tile(&items[5], 1, 4);
    items[5].sprite.frame_count = 1;
    items[5].sprite.frame_start = RED_GEM_FRAME;
    items[5].owner_type = ITEM_FIREBOY_ONLY;
    items[5].float_anim = true;
    items[5].width = 12;  // 碰撞箱宽度
    items[5].height = 12; // 碰撞箱高度

    box_init(&boxes[0], 17, 10, 10, BOX_FRAME);

    lever_init(&levers[0], 9, 21, 22);

    elevator_init(&elevators[0], 1, 16, 16, 19, 14, 51);
    elevator_init(&elevators[1], 35, 12, 12, 16, 18, 57);

    button_init(&buttons[0], 32, 12, 26);
    button_init(&buttons[1], 32, 17, 29);

    unsigned col = 0, row = 0;
    while (1)
    {

        // clock_t start = clock();
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
            for (int i = 0; i < NUM_BOXES; i++)
            {
                for (int j = 0; j < NUM_PLAYERS; j++)
                {
                    box_try_push(&boxes[i], &players[j]);
                }
                box_update_position(&boxes[i], players);
            }
            for (int i = 0; i < NUM_LEVERS; i++)
            {
                lever_update(&levers[0], players);
            }
            for (int i = 0; i < NUM_ELEVATORS; i++)
            {
                if (i == 0)
                    elevator_update(&elevators[i], levers[0].activated, players);
                if (i == 1)
                {
                    elevator_update(&elevators[i], buttons[0].pressed || buttons[1].pressed, players);
                }
            }
            for (int i = 0; i < NUM_BUTTONS; i++)
            {
                button_update(&buttons[i], players);
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
        for (int i = 0; i < NUM_BOXES; i++)
        {
            box_update_sprite(&boxes[i]);
        }
        // clock_t end = clock();
        // float duration = (float)(end - start) / CLOCKS_PER_SEC * 1000;
        // printf("[FRAME] duration = %.2f ms\n", duration);
    }

    // 不会到达，若后续有退出条件，可释放资源：
    input_handler_cleanup();
    close(vga_top_fd);
    return 0;
}
// void debug_draw_test_sprites()
// {
// int index = 14;
// int y = 200;

// // 拉杆底盘（2帧）
// for (int i = 0; i < 2; ++i)
//     write_sprite(index++, 1, 0, 16 * i + 200, y, LEVER_BASE_FRAME + i);

// y += 20;
// // 拉杆动画（3帧）
// for (int i = 0; i < 3; ++i)
//     write_sprite(index++, 1, 0, 16 * i + 200, y, LEVER_ANIM_FRAME + i);

// y += 20;
// // 黄色升降机（4帧）
// for (int i = 0; i < 4; ++i)
//     write_sprite(index++, 1, 0, 16 * i + 200, y, LIFT_YELLOW_FRAME + i);

// y += 20;
// // 紫色按钮（2帧）
// for (int i = 0; i < 2; ++i)
//     write_sprite(index++, 1, 0, 16 * i + 200, y, BUTTON_PURPLE_FRAME + i);

// y += 20;
// // 紫色升降机（4帧）
// for (int i = 0; i < 4; ++i)
//     write_sprite(index++, 1, 0, 16 * i + 200, y, LIFT_PURPLE_FRAME + i);
// }
