#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <stdbool.h>

// ��Ϸ�������壨�� input_handler.h һ�£�
typedef enum
{
    ACTION_NONE = 0,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_JUMP
} game_action_t;

/**
 * ��ʼ����������ģ�飨�� /dev/input/eventX��
 * @return 0 �ɹ���-1 ʧ��
 */
int keyboard_input_init(const char *device_path);

/**
 * �������ģ�飨�ر��豸��
 */
void keyboard_input_cleanup();

/**
 * ��ȡָ����ҵ�ǰ�Ķ�����
 * player_index == 0 �� Fireboy (WAD)
 * player_index == 1 �� Watergirl (������)
 */
game_action_t get_keyboard_action(int player_index);

#endif // KEYBOARD_INPUT_H
