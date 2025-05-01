#include "../include/collision_detection.h"
#include "../include/game_logic.h"
#include <math.h>

bool check_aabb_collision(const bounding_box_t* box1, const bounding_box_t* box2) {
    return (box1->x <= box2->x + box2->width &&
            box1->x + box1->width >= box2->x &&
            box1->y <= box2->y + box2->height &&
            box1->y + box1->height >= box2->y);
}

bool check_character_tile_collision(const character_t* character, int tile_x, int tile_y) {
    bounding_box_t char_box = {
        .x = character->x,
        .y = character->y,
        .width = character->width,
        .height = character->height
    };

    bounding_box_t tile_box = {
        .x = tile_x * TILE_SIZE,
        .y = tile_y * TILE_SIZE,
        .width = TILE_SIZE,
        .height = TILE_SIZE
    };

    return check_aabb_collision(&char_box, &tile_box);
}

bool check_map_boundary_collision(const character_t* character) {
    return (character->x < 0 ||
            character->x + character->width > MAP_WIDTH * TILE_SIZE ||
            character->y < 0 ||
            character->y + character->height > MAP_HEIGHT * TILE_SIZE);
}

bool check_hazard_collision(const character_t* character, tile_type_t tile_type) {
    // Get the tile coordinates the character is currently on
    int tile_x = (int)(character->x / TILE_SIZE);
    int tile_y = (int)(character->y / TILE_SIZE);

    // Check if the character is colliding with the tile
    if (check_character_tile_collision(character, tile_x, tile_y)) {
        // Check if the tile is a hazard for this character type
        if (character->type == CHARACTER_FIREBOY) {
            return (tile_type == TILE_WATER || tile_type == TILE_POISON);
        } else if (character->type == CHARACTER_WATERGIRL) {
            return (tile_type == TILE_FIRE || tile_type == TILE_POISON);
        }
    }
    return false;
}

void calculate_penetration_depth(const bounding_box_t* box1, const bounding_box_t* box2,
                               float* penetration_x, float* penetration_y) {
    // Calculate overlap on X axis
    float overlap_x = fmin(box1->x + box1->width - box2->x,
                          box2->x + box2->width - box1->x);

    // Calculate overlap on Y axis
    float overlap_y = fmin(box1->y + box1->height - box2->y,
                          box2->y + box2->height - box1->y);

    // Determine the direction of penetration
    if (overlap_x < overlap_y) {
        *penetration_x = (box1->x < box2->x) ? -overlap_x : overlap_x;
        *penetration_y = 0;
    } else {
        *penetration_x = 0;
        *penetration_y = (box1->y < box2->y) ? -overlap_y : overlap_y;
    }
}

void resolve_collision(character_t* character, float penetration_x, float penetration_y) {
    // Adjust character position based on penetration depth
    character->x += penetration_x;
    character->y += penetration_y;

    // Update character state based on collision
    if (penetration_y < 0) {
        // Collision from above (landing on ground)
        character->velocity_y = 0;
        character->on_ground = true;
        character->state = STATE_IDLE;
    } else if (penetration_y > 0) {
        // Collision from below (hitting ceiling)
        character->velocity_y = 0;
    }

    if (penetration_x != 0) {
        // Collision from sides
        character->velocity_x = 0;
    }
} 