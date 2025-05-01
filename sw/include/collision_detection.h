#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include <stdbool.h>
#include "game_logic.h"

/**
 * @file collision_detection.h
 * @brief Defines collision detection functions and structures for the game.
 */

/**
 * @brief Structure representing a bounding box for collision detection.
 */
typedef struct {
    float x;      ///< X coordinate of the top-left corner
    float y;      ///< Y coordinate of the top-left corner
    float width;  ///< Width of the bounding box
    float height; ///< Height of the bounding box
} bounding_box_t;

/**
 * @brief Checks for collision between two bounding boxes using AABB (Axis-Aligned Bounding Box) algorithm.
 * 
 * @param box1 First bounding box
 * @param box2 Second bounding box
 * @return true if boxes collide, false otherwise
 */
bool check_aabb_collision(const bounding_box_t* box1, const bounding_box_t* box2);

/**
 * @brief Checks for collision between a character and a tile.
 * 
 * @param character The character to check
 * @param tile_x X coordinate of the tile
 * @param tile_y Y coordinate of the tile
 * @return true if collision occurs, false otherwise
 */
bool check_character_tile_collision(const character_t* character, int tile_x, int tile_y);

/**
 * @brief Checks for collision between a character and the map boundaries.
 * 
 * @param character The character to check
 * @return true if character is out of bounds, false otherwise
 */
bool check_map_boundary_collision(const character_t* character);

/**
 * @brief Checks for collision between a character and a hazard tile.
 * 
 * @param character The character to check
 * @param tile_type The type of tile to check against
 * @return true if character collides with a hazard, false otherwise
 */
bool check_hazard_collision(const character_t* character, tile_type_t tile_type);

/**
 * @brief Calculates the penetration depth between two colliding objects.
 * 
 * @param box1 First bounding box
 * @param box2 Second bounding box
 * @param penetration_x Pointer to store X-axis penetration depth
 * @param penetration_y Pointer to store Y-axis penetration depth
 */
void calculate_penetration_depth(const bounding_box_t* box1, const bounding_box_t* box2, 
                               float* penetration_x, float* penetration_y);

/**
 * @brief Resolves collision between two objects by adjusting their positions.
 * 
 * @param character The character to adjust
 * @param penetration_x X-axis penetration depth
 * @param penetration_y Y-axis penetration depth
 */
void resolve_collision(character_t* character, float penetration_x, float penetration_y);

#endif // COLLISION_DETECTION_H 