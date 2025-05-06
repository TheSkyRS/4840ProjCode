// collision.h
// ��ײ���ģ��ͷ�ļ�

#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include "character.h"
#include "sprite_object.h"

/**
 * ͨ�� AABB ��ײ��⣺���������������εĽ����ж�
 */
bool check_aabb_collision(float x1, float y1, float w1, float h1,
                          float x2, float y2, float w2, float h2);

/**
 * ������ײ������ɫ�ڳ����ƶ�ǰ����Ƿ�ײǽ/�ذ�
 * ����⵽�����赲����ȡ����֡�ٶȣ������������
 */
void handle_character_tile_collision(character_t *ch, float dt);

/**
 * ��ɫ���ͼ���壨��ʯ�����ˡ����ݵȣ���ײ����
 * ������� object�������ɫ��ײ��������Ӧ״̬��־λ
 */
void handle_character_object_collision(character_t *ch, object_t *objects, int num_objects);

/**
 * ��ɫ֮�����ײ�������Ӧ�������Ƽ����ص����Ƶȣ�
 * Ĭ���Ա�ſ���Ľ�ɫ������Ч������ˮŮ�����ǻ��к���
 */
void handle_character_vs_character(character_t *a, character_t *b);

#endif