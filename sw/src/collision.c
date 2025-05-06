// collision.c
// 三类碰撞检测与响应函数实现

#include "collision.h"
#include <stdio.h>

// AABB（轴对齐包围盒）通用碰撞检测函数
bool check_aabb_collision(float x1, float y1, float w1, float h1,
                          float x2, float y2, float w2, float h2)
{
    return !(x1 + w1 < x2 || x1 > x2 + w2 ||
             y1 + h1 < y2 || y1 > y2 + h2);
}

// 地形碰撞处理：基于尝试位置判断是否撞墙/地板
void handle_character_tile_collision(character_t *ch, float dt)
{
    float new_x = ch->x + ch->vx * dt;
    if (is_tile_blocked(new_x, ch->y, ch->width, ch->height))
        ch->vx = 0;
    else
        ch->x = new_x;

    float new_y = ch->y + ch->vy * dt;
    if (is_tile_blocked(ch->x, new_y, ch->width, ch->height))
        ch->vy = 0;
    else
        ch->y = new_y;
}

// 角色与地图物体的碰撞（如：钻石、拉杆等）
void handle_character_object_collision(character_t *ch, object_t *objects, int num_objects)
{
    for (int i = 0; i < num_objects; ++i)
    {
        object_t *obj = &objects[i];
        if (!obj->active || obj->collected)
            continue; // 忽略已处理的对象

        bool collided = check_aabb_collision(
            ch->x, ch->y, ch->width, ch->height,
            obj->x, obj->y, obj->width, obj->height);

        if (collided)
        {
            if (obj->type == OBJ_DIAMOND)
            {
                obj->collected = true;
                printf("[Info] Object collected!\n");
            }
            else if (obj->type == OBJ_LEVER || obj->type == OBJ_ELEVATOR)
            {
                obj->active = true;
                printf("[Info] Object triggered!\n");
            }
        }
    }
}

// 角色与角色碰撞（仅用于控制显示层级，不影响位置）
void handle_character_vs_character(character_t *a, character_t *b)
{
    if (!a->alive || !b->alive)
        return;

    bool collided = check_aabb_collision(
        a->x, a->y, a->width, a->height,
        b->x, b->y, b->width, b->height);

    if (collided)
    {
        // 水女孩显示在火男孩之上
        if (a->type == TYPE_WATERGIRL && b->type == TYPE_FIREBOY)
        {
            a->priority_offset = 1;
            b->priority_offset = 0;
        }
        else if (a->type == TYPE_FIREBOY && b->type == TYPE_WATERGIRL)
        {
            b->priority_offset = 1;
            a->priority_offset = 0;
        }
    }
}
