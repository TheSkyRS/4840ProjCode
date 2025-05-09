// sprite_object.h
// 游戏内物体（机关、道具等）统一的结构定义与接口

#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H
#include <stdbool.h>
#include <stdint.h> // ✅ 提供 uint32_t 类型定义

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
void update_object_animation(object_t *obj, float dt);
/**
 * @brief 打包 sprite 属性为 32-bit 控制字
 *
 * @param enable 是否启用该 sprite
 * @param flip 是否水平翻转
 * @param x sprite 左上角 x 坐标（0~1023）
 * @param y sprite 左上角 y 坐标（0~511）
 * @param frame 使用的帧编号（0~255）
 * @return uint32_t 精灵属性控制字（写入 VGA）
 */
uint32_t make_attr_word(int enable, int flip, uint16_t x, uint16_t y, uint8_t frame);

#endif // SPRITE_OBJECT_H