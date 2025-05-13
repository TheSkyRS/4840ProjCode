#include "sprite.h"
#include "hw_interact.h"
#include <math.h> // Ϊ sinf �ṩ����
#include "type.h"
#include <stdio.h>

extern box_t boxes[NUM_BOXES];
// ��ʼ�� sprite ������֡��
void sprite_set(sprite_t *s, uint8_t index, uint8_t frame_count)
{
    s->index = index;
    s->x = s->y = 0;
    s->frame_id = 0;
    s->flip = 0;
    s->enable = false;
    s->frame_count = frame_count;
}

// ����֡������ѭ����
void sprite_animate(sprite_t *s)
{
    if (s->frame_count > 0)
    {
        uint8_t rel = (s->frame_id - s->frame_start + 1) % s->frame_count;
        s->frame_id = s->frame_start + rel;
    }
}

// ���� sprite ��Ӳ��
void sprite_update(sprite_t *s)
{
    write_sprite(s->index, s->enable, s->flip, s->x, s->y, s->frame_id);
}

// �ر� sprite ��ʾ
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
    if (p->type != PLAYER_WATERGIRL)
        return; // ֻ��ӡˮŮ���ĵ�����Ϣ

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
        printf("[PUSH] No vertical overlap: py=%.1f~%.1f  box_y=%.1f~%.1f\n",
               py, py + ph, by, by + bh);
        return;
    }

    float p_center_x = px + pw / 2.0f;
    float b_left = bx;
    float b_right = bx + bw;

    printf("[PUSH] Watergirl center x=%.1f, vx=%.1f | Box left=%.1f, right=%.1f\n",
           p_center_x, p->vx, b_left, b_right);

    if ((fabsf(p_center_x - b_left) <= 2.0f) && p->vx > 0)
    {
        printf("  [PUSH RIGHT] Triggered push: center %.1f close to box_left %.1f\n",
               p_center_x, b_left);
        box->vx = BOX_PUSH_SPEED;
    }
    else if ((fabsf(p_center_x - b_right) <= 2.0f) && p->vx < 0)
    {
        printf("  [PUSH LEFT] Triggered push: center %.1f close to box_right %.1f\n",
               p_center_x, b_right);
        box->vx = -BOX_PUSH_SPEED;
    }
    else
    {
        printf("  [NO PUSH] No proximity or wrong direction\n");
    }
}

void box_update_position(box_t *box, player_t *players)
{
    float next_x = box->x + box->vx;

    bool blocked = false;
    if (box->vx > 0)
        blocked |= is_tile_blocked(next_x + 31, box->y + 2, 1, 28);
    else if (box->vx < 0)
        blocked |= is_tile_blocked(next_x, box->y + 2, 1, 28);
    printf("[BOX] box->vx=%.2f, next_x=%.1f\n", box->vx, next_x);

    bool collides_with_player = false;
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        float px = players[i].x;
        float py = players[i].y + PLAYER_HITBOX_OFFSET_Y;
        float pw = SPRITE_W_PIXELS;
        float ph = PLAYER_HITBOX_HEIGHT;

        float player_center_x = px + pw / 2.0f;
        float box_center_x = box->x + 16;

        if ((box->vx > 0 && player_center_x < box_center_x) ||
            (box->vx < 0 && player_center_x > box_center_x))
        {
            continue;
        }

        if (check_overlap(next_x, box->y, 32.0f, 32.0f, px, py, pw, ph))
        {
            collides_with_player = true;
            break;
        }
    }
    printf("       blocked=%d, collides_with_player=%d\n", blocked, collides_with_player);

    if (!blocked && !collides_with_player)
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

        if (check_overlap(x, y, w, h, bx, by, 32, 32))
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