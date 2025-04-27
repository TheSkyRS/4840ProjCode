#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdint.h>
#include <stdbool.h>
#include "../include/hw_interface.h"
#include "../include/input_handler.h"

/**
 * @file game_logic.h
 * @brief Defines the core game logic, state, and entities for the Forest Fire and Ice game.
 * 
 * This file contains data structures for characters, game state, map elements,
 * and function prototypes for initializing, updating, and managing the game world.
 */

// --- Constants and Configuration --- 

// Define game physics constants (can be tuned for gameplay feel)
#define GRAVITY 9.8f * 50.0f ///< Scaled gravity constant (pixels/s^2).
#define MOVE_SPEED 100.0f    ///< Horizontal movement speed (pixels/s).
#define JUMP_VELOCITY -200.0f ///< Initial vertical velocity on jump (pixels/s, negative is up).

// Define map dimensions (assuming fixed size for now)
#define MAP_WIDTH 40 ///< Width of the tilemap in tiles.
#define MAP_HEIGHT 30 ///< Height of the tilemap in tiles.
#define TILE_SIZE 16 ///< Size of one square tile in pixels.

// --- Enumerations --- 

/**
 * @brief Identifies the type of character.
 */
typedef enum {
    CHARACTER_FIREBOY = 0, ///< Represents the Fireboy character.
    CHARACTER_WATERGIRL = 1  ///< Represents the Watergirl character.
} character_type_t;

/**
 * @brief Represents the possible states of a character (Finite State Machine - FSM).
 */
typedef enum {
    STATE_IDLE = 0,     ///< Character is standing still.
    STATE_MOVING_LEFT,  ///< Character is moving left.
    STATE_MOVING_RIGHT, ///< Character is moving right.
    STATE_JUMPING,      ///< Character is moving upwards due to a jump.
    STATE_FALLING,      ///< Character is falling due to gravity.
    STATE_DEAD          ///< Character is dead (e.g., touched hazard).
} character_state_t;

/**
 * @brief Represents the different types of tiles that make up the game map.
 */
typedef enum {
    TILE_EMPTY = 0,     ///< Empty space, character can pass through.
    TILE_WALL,          ///< Solid wall, blocks movement.
    TILE_PLATFORM,      ///< Solid platform, blocks movement from above, may allow passing from below.
    TILE_FIRE,          ///< Fire hazard, kills Watergirl.
    TILE_WATER,         ///< Water hazard, kills Fireboy.
    TILE_POISON,        ///< Poison hazard, kills both characters.
    TILE_BUTTON,        ///< Button that can be pressed by characters.
    TILE_LEVER,         ///< Lever that can be activated by characters.
    TILE_DOOR,          ///< Door that can be opened/closed (e.g., by buttons/levers).
    TILE_GEM_RED,       ///< Red gem, collected by Fireboy.
    TILE_GEM_BLUE,      ///< Blue gem, collected by Watergirl.
    TILE_EXIT           ///< Exit point for completing the level.
    // Add more tile types as needed (e.g., moving platforms, ice)
} tile_type_t;

// --- Data Structures --- 

/**
 * @brief Structure representing a game character (Fireboy or Watergirl).
 * Contains position, velocity, state, and other relevant attributes.
 */
typedef struct {
    float x;                  ///< Current X coordinate (in pixels, top-left corner).
    float y;                  ///< Current Y coordinate (in pixels, top-left corner).
    float velocity_x;         ///< Current horizontal velocity (pixels/s).
    float velocity_y;         ///< Current vertical velocity (pixels/s).
    uint8_t width;            ///< Width of the character's bounding box (pixels).
    uint8_t height;           ///< Height of the character's bounding box (pixels).
    character_type_t type;    ///< Type of the character (Fireboy or Watergirl).
    character_state_t state;  ///< Current state in the character's FSM.
    uint8_t frame_id;         ///< Current animation frame ID (to be sent to hardware).
    bool on_ground;           ///< Flag indicating if the character is currently standing on solid ground.
    bool alive;               ///< Flag indicating if the character is currently alive.
} character_t;

/**
 * @brief Structure representing the overall state of the game.
 * Contains character information, level status, and other global game variables.
 */
typedef struct {
    character_t characters[2]; ///< Array containing the state of both characters ([0]=Fireboy, [1]=Watergirl).
    uint8_t active_map;        ///< ID of the currently loaded/active map.
    uint8_t collected_gems;    ///< Count of gems collected in the current level.
    bool level_complete;       ///< Flag indicating if the level has been successfully completed.
    bool game_over;            ///< Flag indicating if the game is over (e.g., a character died).
    // Add other state variables as needed (e.g., timer, score, door states)
} game_state_t;

// --- Function Prototypes --- 

/**
 * @brief Initializes the game logic module.
 * This could involve loading map data, setting up initial game parameters, etc.
 * 
 * @return 0 on success, -1 on failure.
 */
int game_logic_init();

/**
 * @brief Cleans up resources used by the game logic module.
 */
void game_logic_cleanup();

/**
 * @brief Initializes the game state for a new level.
 * Sets character starting positions, resets scores/gems, etc.
 * 
 * @param state Pointer to the game_state_t structure to initialize.
 * @param map_id The ID of the map/level to load.
 */
void init_game_state(game_state_t* state, uint8_t map_id);

/**
 * @brief Updates the state of a single character based on input and physics.
 * Implements the character's Finite State Machine (FSM).
 * 
 * @param character Pointer to the character_t structure to update.
 * @param action The game_action_t derived from player input for this character.
 * @param delta_time The time elapsed since the last frame (in seconds), used for physics calculations.
 */
void update_character_state(character_t* character, game_action_t action, float delta_time);

/**
 * @brief Updates the position of a single character based on its velocity.
 * 
 * @param character Pointer to the character_t structure to update.
 * @param delta_time The time elapsed since the last frame (in seconds).
 */
void update_character_position(character_t* character, float delta_time);

/**
 * @brief Detects collisions between a character and the map tiles.
 * Uses the character's bounding box and checks against the map data.
 * This function might also update the character's `on_ground` status.
 * 
 * @param character Pointer to the character_t structure.
 * @param map_id The ID of the current map.
 * @return The type of tile the character collided with (tile_type_t), or TILE_EMPTY if no collision.
 *         Note: This simplified version might only return the first collision detected.
 */
tile_type_t detect_tile_collision(character_t* character, uint8_t map_id);

/**
 * @brief Detects collisions between the two player characters (AABB check).
 * 
 * @param char1 Pointer to the first character_t structure.
 * @param char2 Pointer to the second character_t structure.
 * @return true if the characters' bounding boxes overlap, false otherwise.
 */
bool detect_character_collision(character_t* char1, character_t* char2);

/**
 * @brief Handles the response to a collision between a character and a map tile.
 * This can involve stopping movement, applying damage, collecting items, etc.
 * 
 * @param character Pointer to the character_t structure that collided.
 * @param tile_type The type of tile (tile_type_t) the character collided with.
 */
void handle_collision_response(character_t* character, tile_type_t tile_type);

/**
 * @brief Handles game interactions like buttons, levers, doors.
 * Checks character positions relative to interactive elements and updates game state accordingly.
 * 
 * @param state Pointer to the current game_state_t.
 */
void handle_interactions(game_state_t* state);

/**
 * @brief Updates the overall game state for one frame.
 * This function orchestrates calls to update character states, positions, 
 * handle collisions, interactions, and check win/lose conditions.
 * 
 * @param state Pointer to the game_state_t structure to update.
 * @param delta_time The time elapsed since the last frame (in seconds).
 */
void update_game_state(game_state_t* state, float delta_time);

/**
 * @brief Synchronizes the current game state to the hardware.
 * Prepares the sprite attribute data based on the character states and calls
 * the hardware interface function (update_sprite_table) during VBlank.
 * May also update control registers if needed.
 * 
 * @param state Pointer to the current game_state_t.
 */
void sync_game_state_to_hardware(game_state_t* state);

/**
 * @brief Gets the type of tile at a specific pixel coordinate on the map.
 * Converts pixel coordinates to tile coordinates and returns the tile type.
 * (Helper function, potentially internal to game_logic.c)
 * 
 * @param x The X pixel coordinate.
 * @param y The Y pixel coordinate.
 * @param map_id The ID of the current map.
 * @return The tile_type_t at the given coordinates.
 */
tile_type_t get_tile_at(int x, int y, uint8_t map_id);


#endif // GAME_LOGIC_H

