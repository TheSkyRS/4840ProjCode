#define _DEFAULT_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h> // 提供 gettimeofday
#include <unistd.h>   // 提供 usleep
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "vga_top.h" // 必须包含：定义 ioctl 结构体与宏
#include "character.h"
#include "sprite_object.h"
#include "collision.h"
#include "joypad_input.h"

// ===== 外部 VGA 文件描述符，由 main.c 中 open 后提供 =====
extern int vga_fd;

// ===== VGA 通信函数：完全替代 vgasys =====
void write_ctrl(uint32_t val)
{
    vga_top_ctrl_arg_t arg = {.value = val};
    ioctl(vga_fd, VGA_TOP_WRITE_CTRL, &arg);
}

void write_sprite(int index, int enable, int flip, int x, int y, int frame)
{
    vga_top_sprite_arg_t arg = {
        .index = index,
        .attr_word = (enable << 31) | (flip << 30) | (frame << 24) |
                     ((x & 0x3FF) << 10) | (y & 0x3FF)};
    ioctl(vga_fd, VGA_TOP_WRITE_SPRITE, &arg);
}

void read_status(unsigned *col, unsigned *row)
{
    vga_top_status_arg_t arg;
    ioctl(vga_fd, VGA_TOP_READ_STATUS, &arg);
    *col = (arg.value >> 10) & 0x3FF;
    *row = arg.value & 0x3FF;
}

// ===== 全局实体 =====
character_t characters[2];
int num_characters = 2;

object_t objects[4];
int num_objects = 4;

#define MAX_SPRITES 32
uint32_t sprite_words[MAX_SPRITES];
int sprite_count = 0;

#define TARGET_FPS 60
#define FRAME_TIME_US (1000000 / TARGET_FPS)
#define TILEMAP_ID 0
#define COLLISION_ENABLE 0
void run_game_loop()
{
    struct timeval frame_start, frame_end;
    float delta_time = 1.0f / TARGET_FPS;
    bool running = true;

    unsigned col = 0, row = 0;

    while (running)
    {
        gettimeofday(&frame_start, NULL);
        read_status(&col, &row);

        if (row < 480)
        {
            sprite_count = 0;

            for (int i = 0; i < num_characters; ++i)
            {
                character_t *ch = &characters[i];
                game_action_t input = get_player_action(i);
                printf("[DEBUG] player %d input: %x\n", i, input);

                update_character_state(ch, input, delta_time);
                update_character_position(ch, delta_time);
                handle_character_tile_collision(ch, delta_time);
                handle_character_object_collision(ch, objects, num_objects);

                printf("[DEBUG] ch[%d] x=%.2f y=%.2f vx=%.2f vy=%.2f\n", i, ch->x, ch->y, ch->vx, ch->vy);
                character_push_sprites(ch, sprite_words, &sprite_count, MAX_SPRITES);
            }

            if (num_characters >= 2)
                handle_character_vs_character(&characters[0], &characters[1]);

            for (int i = 0; i < num_characters; ++i)
            {
                character_t *ch = &characters[i];
                if (!ch->alive)
                    continue;

                if (sprite_count < MAX_SPRITES)
                    sprite_words[sprite_count++] = make_attr_word(1, !ch->facing_right,
                                                                  (uint16_t)ch->x, (uint16_t)ch->y, ch->frame_head);
                if (sprite_count < MAX_SPRITES)
                    sprite_words[sprite_count++] = make_attr_word(1, !ch->facing_right,
                                                                  (uint16_t)ch->x, (uint16_t)(ch->y + 12), ch->frame_body);
            }

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
            write_ctrl(0x00000002); // 控制寄存器

            for (int i = 0; i < sprite_count; ++i)
            {
                write_sprite(i, 1,
                             (sprite_words[i] >> 30) & 1,
                             (sprite_words[i] >> 8) & 0x3FF,
                             (sprite_words[i] >> 18) & 0x1FF,
                             sprite_words[i] & 0xFF);
            }

            for (int i = sprite_count; i < MAX_SPRITES; ++i)
                write_sprite(i, 0, 0, 0, 0, 0);
        }

        gettimeofday(&frame_end, NULL);
        long duration_us = (frame_end.tv_sec - frame_start.tv_sec) * 1000000 +
                           (frame_end.tv_usec - frame_start.tv_usec);

        delta_time = duration_us / 1000000.0f;
        if (delta_time > 0.1f)
            delta_time = 0.1f;
        if (delta_time <= 0)
            delta_time = 1.0f / TARGET_FPS;
        if (duration_us < FRAME_TIME_US)
            usleep(FRAME_TIME_US - duration_us);
    }
}
