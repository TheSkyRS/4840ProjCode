#include "keyboard_input.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static int keyboard_fd = -1;

#define KEY_W 17
#define KEY_A 30
#define KEY_D 32
#define KEY_UP 103
#define KEY_LEFT 105
#define KEY_RIGHT 106

// 当前按键状态缓存
static bool key_states[256] = {false};

// 初始化：打开输入设备
int keyboard_input_init(const char *device_path)
{
    keyboard_fd = open(device_path, O_RDONLY | O_NONBLOCK);
    if (keyboard_fd < 0)
    {
        perror("Failed to open keyboard device");
        return -1;
    }
    return 0;
}

// 清理：关闭设备
void keyboard_input_cleanup()
{
    if (keyboard_fd >= 0)
    {
        close(keyboard_fd);
        keyboard_fd = -1;
    }
}

// 内部更新键盘状态
static void update_key_states()
{
    struct input_event ev;
    while (read(keyboard_fd, &ev, sizeof(ev)) == sizeof(ev))
    {
        if (ev.type == EV_KEY && ev.code < 256)
        {
            key_states[ev.code] = (ev.value != 0);
        }
    }
}

// 主接口：获取玩家动作
game_action_t get_keyboard_action(int player_index)
{
    update_key_states();

    if (player_index == 0)
    { // Fireboy
        if (key_states[KEY_A])
            return ACTION_MOVE_LEFT;
        if (key_states[KEY_D])
            return ACTION_MOVE_RIGHT;
        if (key_states[KEY_W])
            return ACTION_JUMP;
    }
    else if (player_index == 1)
    { // Watergirl
        if (key_states[KEY_LEFT])
            return ACTION_MOVE_LEFT;
        if (key_states[KEY_RIGHT])
            return ACTION_MOVE_RIGHT;
        if (key_states[KEY_UP])
            return ACTION_JUMP;
    }

    return ACTION_NONE;
}
