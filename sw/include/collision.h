// collision.h
// 碰撞检测模块头文件

#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include "character.h"
#include "sprite_object.h"

/**
 * 通用 AABB 碰撞检测：用于任意两个矩形的交叉判断
 */
bool check_aabb_collision(float x1, float y1, float w1, float h1,
                          float x2, float y2, float w2, float h2);

/**
 * 地形碰撞处理：角色在尝试移动前检测是否撞墙/地板
 * 若检测到地形阻挡，则取消本帧速度；否则更新坐标
 */
void handle_character_tile_collision(character_t *ch, float dt);

/**
 * 角色与地图物体（钻石、拉杆、电梯等）碰撞处理
 * 检测所有 object，若与角色碰撞则设置相应状态标志位
 */
void handle_character_object_collision(character_t *ch, object_t *objects, int num_objects);

/**
 * 角色之间的碰撞检测与响应（例如推挤、重叠限制等）
 * 默认以编号靠后的角色优先生效（例如水女孩覆盖火男孩）
 */
void handle_character_vs_character(character_t *a, character_t *b);

#endif