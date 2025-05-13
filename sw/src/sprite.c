#include "sprite.h"
#include "hw_interact.h"
#include <math.h> //
#include "type.h"
#include <stdio.h>

extern box_t boxes[NUM_BOXES];

void sprite_set(sprite_t *s, uint8_t index, uint8_t frame_count)
{
    s->index = index;
    s->x = s->y = 0;
    s->frame_id = 0;
    s->flip = 0;
    s->enable = false;
    s->frame_count = frame_count;
}

void sprite_animate(sprite_t *s)
{
    if (s->frame_count > 0)
    {
        uint8_t rel = (s->frame_id - s->frame_start + 1) % s->frame_count;
        s->frame_id = s->frame_start + rel;
    }
}

void sprite_update(sprite_t *s)
{
    write_sprite(s->index, s->enable, s->flip, s->x, s->y, s->frame_id);
}

void sprite_clear(sprite_t *s)
{
    s->enable = false;
    sprite_update(s);
}

void item_init(item_t *item, float x, float y, uint8_t sprite_index, uint8_t frame_id)
{
    item->x = x;
    item->y = y;
    item->width = 16;
    item->height = 16;
    item->active = true;
    sprite_set(&item->sprite, sprite_index, 0);
    item->sprite.x = (uint16_t)x;
    item->sprite.y = (uint16_t)y;
    item->sprite.frame_id = frame_id;
    item->sprite.enable = true;
    sprite_update(&item->sprite);
}

void item_update_sprite(item_t *item)
{
    if (item->active)
    {
        float offset = 0.0f;
        if (item->float_anim)
        {
            offset = 0.01f * sinf((float)frame_counter * 0.1f + item->sprite.index); // ���鶶�����Ⱥ�Ƶ��
        }

        item->sprite.x = (uint16_t)item->x;
        item->sprite.y = (uint16_t)(item->y + offset);
        item->sprite.enable = true;
        sprite_update(&item->sprite);
    }
    else
    {
        sprite_clear(&item->sprite);
    }
}

void box_init(box_t *b, int tile_x, int tile_y, int sprite_base_index, uint8_t frame_id)

{
    b->x = tile_x * 16;
    b->y = tile_y * 16;
    b->vx = 0;
    b->active = true;

    for (int i = 0; i < 4; i++)
    {
        sprite_set(&b->sprites[i], sprite_base_index + i, 1);
        b->sprites[i].frame_id = frame_id + i;
        b->sprites[i].enable = true;
    }
}

void box_update_sprite(box_t *b)
{
    float x = b->x;
    float y = b->y;

    b->sprites[0].x = x;
    b->sprites[0].y = y + 1;

    b->sprites[1].x = x + 15;
    b->sprites[1].y = y + 1;

    b->sprites[2].x = x;
    b->sprites[2].y = y + 16;

    b->sprites[3].x = x + 15;
    b->sprites[3].y = y + 16;

    for (int i = 0; i < 4; i++)
    {
        sprite_update(&b->sprites[i]);
    }
}

void box_try_push(box_t *box, const player_t *p)
{
    float pw = SPRITE_W_PIXELS;
    float ph = PLAYER_HITBOX_HEIGHT;
    float px = p->x;
    float py = p->y + PLAYER_HITBOX_OFFSET_Y;

    float bw = 32.0f;
    float bh = 32.0f;
    float bx = box->x;
    float by = box->y;

    bool vertical_overlap = (py + ph > by) && (py < by + bh);
    if (!vertical_overlap)
    {
        return;
    }

    float p_center_x = px + pw / 2.0f;
    float b_left = bx;
    float b_right = bx + bw;
    const float PUSH_TOLERANCE = 5.0f; // 放宽至 5 像素范围

    if ((fabsf(p_center_x - b_left) <= PUSH_TOLERANCE) && p->vx > 0)
    {
        box->vx = BOX_PUSH_SPEED;
    }
    else if ((fabsf(p_center_x - b_right) <= PUSH_TOLERANCE) && p->vx < 0)
    {
        box->vx = -BOX_PUSH_SPEED;
    }
}
void box_update_position(box_t *box, player_t *players)
{
    float next_x = box->x + box->vx;

    bool blocked = false;
    if (box->vx > 0)
        blocked |= is_tile_blocked(next_x + 31, box->y + 2, 1, 28);
    else if (box->vx < 0)
        blocked |= is_tile_blocked(next_x + 1, box->y + 2, 1, 28);

    bool will_overlap_non_pusher = false;

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        float px = players[i].x;
        float py = players[i].y + PLAYER_HITBOX_OFFSET_Y;
        float pw = SPRITE_W_PIXELS;
        float ph = PLAYER_HITBOX_HEIGHT;

        float p_center_x = px + pw / 2.0f;
        bool vertical_overlap = (py + ph > box->y) && (py < box->y + 32);
        if (!vertical_overlap)
            continue;

        // 判断是否为贴边推箱者（允许重叠）
        bool is_pusher = false;
        if (box->vx > 0 && fabsf(p_center_x - box->x) <= 10.0f && players[i].vx > 0)
            is_pusher = true;
        else if (box->vx < 0 && fabsf(p_center_x - (box->x + 32)) <= 10.0f && players[i].vx < 0)
            is_pusher = true;

        // 非推箱者且会被压上，阻止
        if (!is_pusher && check_overlap(next_x + 2, box->y + 2, 28, 28, px + SPRITE_W_PIXELS / 2.0f, py + PLAYER_HITBOX_OFFSET_Y, 1.0f, PLAYER_HITBOX_HEIGHT))
        {
            will_overlap_non_pusher = true;
            break;
        }
    }
    if (!blocked && !will_overlap_non_pusher)
    {
        box->x = next_x;
    }
    if (box->vx > 0)
        box->vx -= BOX_FRICTION;
    else if (box->vx < 0)
        box->vx += BOX_FRICTION;
    if (fabsf(box->vx) < BOX_FRICTION)
        box->vx = 0;
}

bool is_box_blocked(float x, float y, float w, float h)
{
    for (int i = 0; i < NUM_BOXES; i++)
    {
        if (!boxes[i].active)
            continue;

        float bx = boxes[i].x;
        float by = boxes[i].y;

        if (check_overlap(x, y, w, h, bx + 2, by + 2, 28, 28))
        {
            return true;
        }
    }

    return false;
}

bool check_overlap(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2)
{
    return (x1 < x2 + w2) && (x1 + w1 > x2) &&
           (y1 < y2 + h2) && (y1 + h1 > y2);
}

void lever_init(lever_t *lvr, float tile_x, float tile_y, uint8_t sprite_index_base)
{
    lvr->x = tile_x * 16;
    lvr->y = tile_y * 16;
    lvr->activated = false;
    lvr->sprite_base_index = sprite_index_base;

    // 帧资源定义
    lvr->base_frame[0] = LEVER_BASE_FRAME + 0;
    lvr->base_frame[1] = LEVER_BASE_FRAME + 1;
    lvr->handle_frames[0] = LEVER_ANIM_FRAME + 0; // ←
    lvr->handle_frames[1] = LEVER_ANIM_FRAME + 1; // 中
    lvr->handle_frames[2] = LEVER_ANIM_FRAME + 2; // →

    // 设置底座精灵（2 tile）
    for (int i = 0; i < 2; ++i)
    {
        sprite_set(&lvr->base_sprites[i], sprite_index_base + i, 0);
        lvr->base_sprites[i].x = (uint16_t)(lvr->x + i * 16);
        lvr->base_sprites[i].y = (uint16_t)(lvr->y - 4);
        lvr->base_sprites[i].frame_id = lvr->base_frame[i];
        lvr->base_sprites[i].enable = true;
        sprite_update(&lvr->base_sprites[i]);
    }

    // 设置拉杆柄
    sprite_set(&lvr->handle_sprite_left, sprite_index_base + 2, 0);
    lvr->handle_sprite_left.x = (uint16_t)(lvr->x + 5);
    lvr->handle_sprite_left.y = (uint16_t)(lvr->y - 16);
    lvr->handle_sprite_left.frame_id = lvr->handle_frames[1]; // 中间帧
    lvr->handle_sprite_left.enable = false;
    sprite_update(&lvr->handle_sprite_left);
    // 设置拉杆柄
    sprite_set(&lvr->handle_sprite_right, sprite_index_base + 3, 0);
    lvr->handle_sprite_right.x = (uint16_t)(lvr->x + 13);
    lvr->handle_sprite_right.y = (uint16_t)(lvr->y - 16);
    lvr->handle_sprite_right.frame_id = lvr->handle_frames[2]; // 中间帧
    lvr->handle_sprite_right.enable = true;
    sprite_update(&lvr->handle_sprite_right);
}
void lever_update(lever_t *lvr, const player_t *players)
{
    for (int i = 0; i < NUM_PLAYERS; ++i)
    {
        const player_t *p = &players[i];
        float px = p->x + SPRITE_W_PIXELS / 2.0f;
        float py = p->y + 32;

        if (fabsf(py - lvr->y) > 12.0f)
            continue;

        // 切换为右置（玩家从左进）
        if (!lvr->activated && px >= lvr->x + 24 && px <= lvr->x + 32)
        {
            lvr->activated = true;
            lvr->handle_sprite_left.enable = false;
            lvr->handle_sprite_right.enable = true;
            sprite_update(&lvr->handle_sprite_left);
            sprite_update(&lvr->handle_sprite_right);
            break;
        }

        // 切换为左置（玩家从右进）
        if (lvr->activated && px >= lvr->x && px <= lvr->x + 8)
        {
            lvr->activated = false;
            lvr->handle_sprite_left.enable = true;
            lvr->handle_sprite_right.enable = false;
            sprite_update(&lvr->handle_sprite_left);
            sprite_update(&lvr->handle_sprite_right);
            break;
        }
    }
}

void elevator_init(elevator_t *elv, float tile_x, float tile_y, float min_tile_y, float max_tile_y, uint8_t sprite_index_base)
{
    float x = tile_x * 16;
    float y = tile_y * 16;

    elv->x = x;
    elv->y = y;
    elv->min_y = min_tile_y * 16;
    elv->max_y = max_tile_y * 16;
    elv->vy = 0.0f;
    elv->moving_up = true;
    elv->active = false;
    elv->sprite_base_index = sprite_index_base;

    // 左一块
    sprite_set(&elv->sprites[0], sprite_index_base + 0, 0);
    elv->sprites[0].x = (uint16_t)(x + 1);
    elv->sprites[0].y = (uint16_t)(y);
    elv->sprites[0].frame_id = LIFT_YELLOW_FRAME + 0;
    elv->sprites[0].enable = true;
    sprite_update(&elv->sprites[0]);

    // 左中块
    sprite_set(&elv->sprites[1], sprite_index_base + 1, 0);
    elv->sprites[1].x = (uint16_t)(x + 16);
    elv->sprites[1].y = (uint16_t)(y);
    elv->sprites[1].frame_id = LIFT_YELLOW_FRAME + 1;
    elv->sprites[1].enable = true;
    sprite_update(&elv->sprites[1]);

    // 右中块
    sprite_set(&elv->sprites[2], sprite_index_base + 2, 0);
    elv->sprites[2].x = (uint16_t)(x + 32);
    elv->sprites[2].y = (uint16_t)(y);
    elv->sprites[2].frame_id = LIFT_YELLOW_FRAME + 2;
    elv->sprites[2].enable = true;
    sprite_update(&elv->sprites[2]);

    // 右一块
    sprite_set(&elv->sprites[3], sprite_index_base + 3, 0);
    elv->sprites[3].x = (uint16_t)(x + 47);
    elv->sprites[3].y = (uint16_t)(y);
    elv->sprites[3].frame_id = LIFT_YELLOW_FRAME + 3;
    elv->sprites[3].enable = true;
    sprite_update(&elv->sprites[3]);
}

bool is_elevator_blocked(float x, float y, float w, float h, float *vy_out)
{
    for (int i = 0; i < NUM_ELEVATORS; i++)
    {
        elevator_t *elv = &elevators[i];

        for (int j = 0; j < 4; j++)
        {
            sprite_t *s = &elv->sprites[j];

            float ex = s->x;
            float ey = s->y;

            if (check_overlap(x, y, w, h, ex, ey, 16, 16))
            {
                if (vy_out)
                    *vy_out = elv->vy; // 返回电梯垂直速度（用于随动）
                return true;
            }
        }
    }
    return false;
}

void elevator_update(elevator_t *elv, bool go_up, player_t *players)
{
    // 决定目标方向
    if (go_up)
    {
        if (elv->y > elv->min_y)
        {
            elv->vy = -0.5f;
        }
        else
        {
            elv->y = elv->min_y;
            elv->vy = 0;
        }
    }
    else
    {
        if (elv->y < elv->max_y)
        {
            elv->vy = 0.5f;
        }
        else
        {
            elv->y = elv->max_y;
            elv->vy = 0;
        }
    }

    elv->y += elv->vy;

    for (int i = 0; i < 4; ++i)
    {
        elv->sprites[i].y = (uint16_t)(elv->y);
        sprite_update(&elv->sprites[i]);
    }

    // 玩家随动
    for (int i = 0; i < NUM_PLAYERS; ++i)
    {
        player_t *p = &players[i];
        float px = p->x + SPRITE_W_PIXELS / 2.0f;
        float foot_y = p->y + PLAYER_HEIGHT_PIXELS;

        if (px >= elv->x && px <= elv->x + 64 &&
            fabsf(foot_y - elv->y) < 4.0f)
        {
            p->y += elv->vy;
        }
    }
}
