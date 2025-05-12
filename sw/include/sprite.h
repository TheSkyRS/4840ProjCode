#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdbool.h>

#define SPRITE_H_PIXELS 16
#define SPRITE_W_PIXELS 16
// 红钻石
#define RED_GEM_FRAME ((uint8_t)44) // 0x2C00 >> 8 = 44

// 蓝钻石
#define BLUE_GEM_FRAME ((uint8_t)45) // 0x2D00 >> 8 = 45

// 拉杆底盘（2帧）
#define LEVER_BASE_FRAME ((uint8_t)46) // 0x2E00 >> 8 = 46

// 拉杆动画（3帧）
#define LEVER_ANIM_FRAME ((uint8_t)48) // 0x3000 >> 8 = 48

// 黄色升降机（4帧）
#define LIFT_YELLOW_FRAME ((uint8_t)51) // 0x3300 >> 8 = 51

// 紫色按钮（2帧）
#define BUTTON_PURPLE_FRAME ((uint8_t)55) // 0x3700 >> 8 = 55

// 紫色升降机（4帧）
#define LIFT_PURPLE_FRAME ((uint8_t)57) // 0x3900 >> 8 = 57

// 箱子（4帧）
#define BOX_FRAME ((uint8_t)61) // 0x3D00 >> 8 = 61

typedef struct
{
    uint8_t index;
    uint8_t frame_id;
    uint8_t flip;
    uint16_t x, y;
    bool enable;
    uint8_t frame_count; // 总帧数
    uint8_t frame_start; // 起始帧 ID（默认 0）
} sprite_t;

typedef enum
{
    ITEM_ANY = 0,
    ITEM_FIREBOY_ONLY,
    ITEM_WATERGIRL_ONLY
} item_owner_t;

typedef struct
{
    float x, y;
    float width, height;
    bool active;
    sprite_t sprite;
    item_owner_t owner_type; // 🔺新增
} item_t;

bool check_overlap(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2);

// 初始化 sprite，设置 index 与帧数
void sprite_set(sprite_t *s, uint8_t index, uint8_t frame_count);

// 帧循环更新（frame_id++)
void sprite_animate(sprite_t *s);

// 写入 VGA
void sprite_update(sprite_t *s);

// 关闭显示
void sprite_clear(sprite_t *s);

void item_init(item_t *item, float x, float y, uint8_t sprite_index, uint8_t frame_id);
#endif
