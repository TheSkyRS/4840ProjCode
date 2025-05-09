#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "vga_top.h" // ✅ 必须：用于结构体与 ioctl 宏
#include "game_logic.h"
#include "character.h"
#include "sprite_object.h"
#include "joypad_input.h" // ✅ Joypad 输入控制

// 全局角色和物体
extern character_t characters[];
extern int num_characters;

extern object_t objects[];
extern int num_objects;

// VGA 设备文件描述符
int vga_fd = -1;

// 初始化 VGA 设备（替代 vgasys_init）
int init_vga(const char *dev_path)
{
    vga_fd = open(dev_path, O_RDWR);
    if (vga_fd < 0)
    {
        perror("[main] 无法打开 VGA 设备");
        return -1;
    }
    return 0;
}

// 清理 VGA（替代 vgasys_cleanup）
void close_vga()
{
    if (vga_fd >= 0)
        close(vga_fd);
}

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
    if (init_vga("/dev/vga_top") < 0)
    {
        fprintf(stderr, "[main] 无法打开 VGA 设备，退出程序。\n");
        return 1;
    }

    // === 初始化角色 ===
    init_character(&characters[0], 100, 300, TYPE_FIREBOY);
    init_character(&characters[1], 200, 300, TYPE_WATERGIRL);

    // === 初始化地图物体 ===
    memset(objects, 0, sizeof(object_t) * num_objects);
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
    run_game_loop(); // 你必须确保 run_game_loop() 内部使用了 vga_fd

    // === 退出前清理资源 ===
    close_vga();

    return 0;
}
