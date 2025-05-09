// sprite_object.h
// 游戏内物体（机关、道具等）统一的结构定义与接口

#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H
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
    float x, y;           // 位置
    int width, height;    // 大小
    int frame_id;         // 当前帧编号
    int frame_count;      // 动画帧总数
    float anim_timer;     // 当前帧计时器
    float frame_interval; // 每帧时间
    bool active;          // 是否激活
    bool collected;       // 是否收集
    object_type_t type;   // 类型
} object_t;

// 将所有地图物体同步渲染到硬件 Sprite 表中
void object_push_sprite(object_t *obj, uint32_t *sprite_words, int *count, int max_count);

#endif // SPRITE_OBJECT_H