// sprite_object.c
// 实现地图物体（如钻石、拉杆、电梯等）的绘制与动画更新逻辑

#include "sprite_object.h" // 物体结构体与接口声明
#include "vgasys.h"

/**
 * 更新单个物体的动画帧（如果有多帧）
 */
void update_object_animation(object_t *obj, float dt)
{
    if (obj->frame_count <= 1)
        return; // 静态物体无需动画更新

    obj->anim_timer += dt;
    if (obj->anim_timer >= obj->frame_interval)
    {
        obj->frame_id = (obj->frame_id + 1) % obj->frame_count;
        obj->anim_timer = 0.0f;
    }
}

/**
 * @brief 将物体的精灵信息写入 sprite 缓存数组
 *
 * @param obj           指向物体结构体的指针
 * @param sprite_words  精灵缓存数组
 * @param count         当前写入数量指针
 * @param max_count     缓冲区最大容量
 */
void object_push_sprite(object_t *obj, uint32_t *sprite_words, int *count, int max_count)
{
    if (!obj || obj->collected || *count >= max_count)
        return;

    uint32_t word = make_attr_word(
        1, // enable
        0, // flip_horizontal = false
        (uint16_t)obj->x,
        (uint16_t)obj->y,
        obj->frame_id);

    sprite_words[(*count)++] = word;
}