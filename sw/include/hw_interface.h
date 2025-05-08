// hw_interface.h
// 硬件绘制接口：控制 BRAM sprite 表与通用精灵构造

#ifndef HW_INTERFACE_H
#define HW_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_SPRITES 64

// 精灵结构体 flags（位域）
typedef union
{
    struct
    {
        uint8_t alive : 1;           // 是否显示该 sprite
        uint8_t flip_horizontal : 1; // 是否水平翻转（用于左右朝向）
        uint8_t reserved : 6;        // 保留位
    } bits;
    uint8_t value;
} sprite_flags_t;

// 精灵属性结构体（用于 BRAM 表）
typedef struct
{
    uint16_t x, y;         // 坐标位置
    uint8_t frame_id;      // 使用图块帧编号
    uint8_t priority;      // 显示优先级（小值在上）
    uint8_t width, height; // 精灵宽高
    sprite_flags_t flags;  // 标志位（是否显示、翻转等）
} sprite_attr_t;

// 向硬件写入完整的 sprite 表（MAX_SPRITES 个）
void update_sprite_table(sprite_attr_t *sprites, int count);

// 仅写入单个 sprite（用于快速测试）
void write_sprite(int index, const sprite_attr_t *s);

// 构造一个 sprite 属性结构（通用函数，可用于角色或物体）
sprite_attr_t make_sprite(float x, float y, int frame_id,
                          int width, int height,
                          int priority, bool alive, bool facing_right);

#endif // HW_INTERFACE_H
