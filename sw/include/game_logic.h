// game_logic.h
// ��Ϸ�߼����ӿڣ�������ѭ��������ȫ��״̬����

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "character.h"     // �����ɫģ�鶨��
#include "sprite_object.h" // ������Ϸ����ģ�鶨��
#include <stdbool.h>

// ��Ϸ����״̬������չ������ͣ��ʤ����ʧ�ܵȣ�
typedef enum
{
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_WIN
} game_state_t;

// ����һ֡��������Ϸ״̬����ɫ�����塢������
void update_game_state(character_t *characters, int num_characters,
                       object_t *objects, int num_objects,
                       float delta_time);

// ��ѡ�����������л��ؿ�״̬��ͬ��ȫ�ֲ�����Ӳ������ƹ⡢�Ʒֵȣ�
void sync_game_state_to_hardware(game_state_t state);

#endif // GAME_LOGIC_H
