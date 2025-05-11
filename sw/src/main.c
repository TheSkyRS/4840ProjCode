#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "hw_interact.h"
#include "player.h"
#include "joypad_input.h"
#include "tilemap.h" // 一定要包含

#define NUM_PLAYERS 2
#define VACTIVE 480

int main()
{
    collision_map_init(); // ✅ 初始化像素级碰撞图
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
    player_init(&players[0], 64, 352, 0, 1, PLAYER_FIREBOY);
    player_init(&players[1], 64, 416, 2, 3, PLAYER_WATERGIRL);

    unsigned col = 0, row = 0;

    // 动画帧测试
    for (uint8_t i = 0; i < 32; i++)
    {
        write_sprite(i, 1, 0, (i % 8) * 32, (i / 8) * 32, i + 17); // frame_id = i
    }
    //

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
    }

    // 不会到达，若后续有退出条件，可释放资源：
    input_handler_cleanup();
    close(vga_top_fd);
    return 0;
}
