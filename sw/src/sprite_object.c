// sprite_object.c
// 实现地图物体（如钻石、拉杆、电梯等）的绘制同步逻辑

#include "sprite_object.h" // 物体结构体与接口声明
#include "hw_interface.h"  // 精灵构造与写入硬件接口

/**
 * 将所有地图物体写入 Sprite 表中。
 * - 跳过已收集的物体（例如钻石）
 * - 每个物体根据传入的 sprite_offset 写入不同槽位
 *
 * 参数说明：
 * - objects：物体数组（object_t）
 * - num_objects：物体总数量
 * - sprite_offset：起始写入槽位编号（避免与角色冲突）
 */
void sync_objects_to_hardware(object_t *objects, int num_objects, int sprite_offset)
{
    for (int i = 0; i < num_objects; ++i)
    {
        object_t *obj = &objects[i];

        if (obj->collected) // 如果物体已被收集，则不再显示
            continue;

        // 构造该物体对应的 sprite 属性（默认不翻转）
        sprite_attr_t s = make_sprite(
            obj->x,            // 坐标 X
            obj->y,            // 坐标 Y
            obj->frame_id,     // 使用的图块帧编号
            obj->width,        // 精灵宽度
            obj->height,       // 精灵高度
            sprite_offset + i, // 写入槽位（偏移避免与角色冲突）
            true,              // alive 标志：显示该 sprite
            false              // 默认不翻转（flip_horizontal）
        );

        // 写入该 sprite 到对应槽位
        write_sprite(sprite_offset + i, &s);
    }
}