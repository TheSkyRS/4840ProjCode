// sprite_object.h
// 游戏内物体（机关、道具等）统一的结构定义与接口

#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "hw_interface.h" // 依赖 sprite_attr_t 与 make_sprite()
#include <stdbool.h>

// 游戏对象类型枚举
typedef enum
{
    OBJ_DIAMOND, // 收集物：钻石
    OBJ_LEVER,   // 机关：拉杆
    OBJ_ELEVATOR // 机关：升降平台
} object_type_t;

// 游戏中用于绘制的物体结构体（非角色）
typedef struct
{
    float x, y;         // 物体位置
    int width, height;  // 显示尺寸
    int frame_id;       // 使用的动画帧编号
    bool active;        // 是否激活（如拉杆已触发）
    bool collected;     // 是否已被收集（仅用于钻石）
    object_type_t type; // 类型标识
} object_t;

// 将所有地图物体同步渲染到硬件 Sprite 表中
void sync_objects_to_hardware(object_t *objects, int num_objects, int sprite_offset);

#endif // SPRITE_OBJECT_H