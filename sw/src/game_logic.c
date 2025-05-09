#define _DEFAULT_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h> // 提供 gettimeofday
#include <unistd.h>   // 提供 usleep

#include "vga_top.h"        // ioctl 指令码结构体
#include "vgasys.h"         // VGA 系统接口封装（write_sprite 等）
#include "character.h"      // character_t, 角色更新逻辑
#include "sprite_object.h"  // object_t, 地图物体动画
#include "collision.h"      // 碰撞检测
#include "keyboard_input.h" // 玩家输入（get_player_action）

// 精灵缓冲区大小（VGA 支持最多 32 个 sprite）
#define MAX_SPRITES 32

// 帧率控制参数（单位：微秒）
#define TARGET_FPS 60
#define FRAME_TIME_US (1000000 / TARGET_FPS)

character_t characters[2];
int num_characters = 2;

object_t objects[4];
int num_objects = 4;

// 精灵写入缓冲数组（每帧构造后在消隐期写入）
uint32_t sprite_words[MAX_SPRITES];
int sprite_count = 0;

// 控制寄存器参数
#define TILEMAP_ID 0       // 当前地图编号（可用于切换关卡）
#define COLLISION_ENABLE 0 // 硬件碰撞开关（可选）

/**
 * 游戏主循环：按帧运行，分为可见期（处理逻辑）与消隐期（写入硬件）
 */
void run_game_loop()
{
    struct timeval frame_start, frame_end;
    float delta_time = 1.0f / TARGET_FPS; // 每帧时间，秒
    bool running = true;

    unsigned col = 0, row = 0; // VGA 当前扫描位置

    while (running)
    {
        // 记录本帧起始时间
        gettimeofday(&frame_start, NULL);

        // 查询 VGA 状态寄存器，获得扫描位置（vcount）
        read_status(&col, &row);

        if (row < 480)
        {
            // ================== 可见区：计算角色和物体逻辑 ==================

            sprite_count = 0; // 清空精灵缓冲区

            // 角色状态更新：输入→速度→碰撞→动作
            for (int i = 0; i < num_characters; ++i)
            {
                character_t *ch = &characters[i];
                game_action_t input = get_player_action(i);

                update_character_state(ch, input, delta_time);
                handle_character_tile_collision(ch, delta_time);
                handle_character_object_collision(ch, objects, num_objects);
            }

            // 角色之间的显示层级调整（谁覆盖谁）
            if (num_characters >= 2)
            {
                handle_character_vs_character(&characters[0], &characters[1]);
            }

            // === 构造每个角色的两个精灵（头部 + 身体） ===
            for (int i = 0; i < num_characters; ++i)
            {
                character_t *ch = &characters[i];
                if (!ch->alive)
                    continue;

                if (sprite_count < MAX_SPRITES)
                    sprite_words[sprite_count++] = make_attr_word(1, !ch->facing_right,
                                                                  (uint16_t)ch->x, (uint16_t)(ch->y), ch->frame_head);

                if (sprite_count < MAX_SPRITES)
                    sprite_words[sprite_count++] = make_attr_word(1, !ch->facing_right,
                                                                  (uint16_t)ch->x, (uint16_t)(ch->y + 12), ch->frame_body);
            }

            // === 更新物体动画，并写入缓冲精灵 ===
            for (int i = 0; i < num_objects; ++i)
            {
                object_t *obj = &objects[i];
                if (obj->collected)
                    continue;

                update_object_animation(obj, delta_time);

                if (sprite_count < MAX_SPRITES)
                    sprite_words[sprite_count++] = make_attr_word(1, 0,
                                                                  (uint16_t)obj->x, (uint16_t)obj->y, obj->frame_id);
            }
        }
        else
        {
            // ================== 消隐区（vcount ≥ 480）：写入硬件寄存器 ==================

            // 写入控制寄存器（地图编号、碰撞开关等）
            uint32_t ctrl_value = (TILEMAP_ID & 0x3) | ((COLLISION_ENABLE & 0x1) << 2);
            write_ctrl(ctrl_value);

            // 写入精灵属性到硬件
            for (int i = 0; i < sprite_count; ++i)
            {
                write_sprite(i, 1,
                             (sprite_words[i] >> 30) & 1,     // flip
                             (sprite_words[i] >> 8) & 0x3FF,  // x
                             (sprite_words[i] >> 18) & 0x1FF, // y
                             sprite_words[i] & 0xFF);         // frame
            }

            // 清除多余精灵槽，避免残影
            for (int i = sprite_count; i < MAX_SPRITES; ++i)
            {
                write_sprite(i, 0, 0, 0, 0, 0);
            }
        }

        // ================== 帧率控制（限速） ==================

        gettimeofday(&frame_end, NULL);

        long duration_us = (frame_end.tv_sec - frame_start.tv_sec) * 1000000 +
                           (frame_end.tv_usec - frame_start.tv_usec);

        delta_time = duration_us / 1000000.0f;
        if (delta_time > 0.1f)
            delta_time = 0.1f; // 防止跳帧时 delta_time 爆炸
        if (delta_time <= 0)
            delta_time = 1.0f / TARGET_FPS;

        // 若帧时间太短则主动 sleep 补齐时间
        if (duration_us < FRAME_TIME_US)
        {
            usleep(FRAME_TIME_US - duration_us);
        }
    }
}
