#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <stdbool.h>

// 游戏动作定义（与 input_handler.h 一致）
typedef enum
{
    ACTION_NONE = 0,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_JUMP
} game_action_t;

/**
 * 初始化键盘输入模块（打开 /dev/input/eventX）
 * @return 0 成功，-1 失败
 */
int keyboard_input_init(const char *device_path);

/**
 * 清理键盘模块（关闭设备）
 */
void keyboard_input_cleanup();

/**
 * 获取指定玩家当前的动作：
 * player_index == 0 → Fireboy (WAD)
 * player_index == 1 → Watergirl (↑←→)
 */
game_action_t get_keyboard_action(int player_index);

#endif // KEYBOARD_INPUT_H
