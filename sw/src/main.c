#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vgasys.h"
#include "game_logic.h"
#include "character.h"
#include "sprite_object.h"
#include "joypad_input.h" // ✅ 新增：引入 Joypad 模块

// 全局角色和物体
character_t characters[2];
int num_characters = 2;

object_t objects[4];
int num_objects = 4;

int main()
{
    // === 初始化 Joypad 控制器 ===
    if (insert_joypad(get_default_joypad_path(0), 0) < 0)
    {
        fprintf(stderr, "[main] 玩家1的 Joypad 初始化失败。\n");
        return 1;
    }
    if (insert_joypad(get_default_joypad_path(1), 1) < 0)
    {
        fprintf(stderr, "[main] 玩家2的 Joypad 初始化失败。\n");
        return 1;
    }

    // === 初始化 VGA 硬件 ===
    if (vgasys_init("/dev/vga_top") < 0)
    {
        fprintf(stderr, "[main] 无法打开 VGA 设备，退出程序。\n");
        return 1;
    }

    // === 初始化角色 ===
    init_character(&characters[0], 100, 300, TYPE_FIREBOY);
    init_character(&characters[1], 200, 300, TYPE_WATERGIRL);

    // === 初始化地图物体 ===
    memset(objects, 0, sizeof(objects));
    objects[0].x = 240;
    objects[0].y = 280;
    objects[0].frame_id = 2;
    objects[1].x = 360;
    objects[1].y = 280;
    objects[1].frame_id = 3;
    objects[2].x = 100;
    objects[2].y = 200;
    objects[2].frame_id = 4;
    objects[3].x = 300;
    objects[3].y = 180;
    objects[3].frame_id = 5;

    // === 启动游戏主循环 ===
    run_game_loop();

    // === 退出前清理资源 ===
    vgasys_cleanup();
    // 可选：调用 hander 清理
    input_handler_cleanup(); // 若你有此函数实现

    return 0;
}
