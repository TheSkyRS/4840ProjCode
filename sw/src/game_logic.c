/**
 * @file game_logic.c
 * @brief Implements the core game logic for the Forest Fire and Ice game.
 * 
 * This file contains the implementation for managing character states, movement,
 * physics, collision detection, interactions, and overall game state updates.
 * It relies on the hardware interface (hw_interface.h) for rendering and 
 * the input handler (input_handler.h) for player actions.
 */

#include "../include/game_logic.h"
#include "../include/hw_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// --- Static Global Variables --- 

/// Placeholder for the game map data. 
/// In a real implementation, this should be loaded from a level file.
/// Dimensions are defined in game_logic.h (MAP_WIDTH, MAP_HEIGHT).
tile_type_t map_data[MAP_HEIGHT][MAP_WIDTH];

// --- Function Implementations --- 

/**
 * @brief Initializes the game logic module.
 * Currently sets up a simple placeholder map (ground at the bottom).
 * In a real game, this would load level data from files.
 * 
 * @return Always returns 0 (success) in this version.
 */
int game_logic_init() {
    printf("Initializing Game Logic...\n");
    
    // Placeholder: Initialize a simple map with ground
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (y == MAP_HEIGHT - 1 || y == MAP_HEIGHT - 2) { // Add a thicker ground
                map_data[y][x] = TILE_WALL; // Ground
            } else {
                map_data[y][x] = TILE_EMPTY;
            }
        }
    }
    // Add some example platforms/hazards (adjust coordinates as needed)
    map_data[MAP_HEIGHT - 5][5] = TILE_PLATFORM;
    map_data[MAP_HEIGHT - 5][6] = TILE_PLATFORM;
    map_data[MAP_HEIGHT - 5][7] = TILE_PLATFORM;
    map_data[MAP_HEIGHT - 8][15] = TILE_FIRE;
    map_data[MAP_HEIGHT - 8][25] = TILE_WATER;

    return 0;
}

/**
 * @brief Cleans up resources used by the game logic module.
 * Currently just prints a message.
 */
void game_logic_cleanup() {
    printf("Cleaning up Game Logic...\n");
    // Placeholder: Add cleanup if map data or other resources were dynamically allocated.
}

/**
 * @brief Initializes the game state for a specific map/level.
 * Sets starting positions and default states for Fireboy and Watergirl.
 * Resets level-specific variables like collected gems.
 * 
 * @param state Pointer to the game_state_t structure to initialize.
 * @param map_id The ID of the map/level being started (currently unused).
 */
void init_game_state(game_state_t* state, uint8_t map_id) {
    state->active_map = map_id;
    state->collected_gems = 0;
    state->level_complete = false;
    state->game_over = false;

    // Initialize Fireboy (Player 0)
    state->characters[0].x = 50.0f;          // Starting X position
    state->characters[0].y = (MAP_HEIGHT - 4) * TILE_SIZE; // Starting Y position (above ground)
    state->characters[0].velocity_x = 0.0f;
    state->characters[0].velocity_y = 0.0f;
    state->characters[0].width = TILE_SIZE;  // Assuming character is 1 tile wide
    state->characters[0].height = TILE_SIZE; // Assuming character is 1 tile high
    state->characters[0].type = CHARACTER_FIREBOY;
    state->characters[0].state = STATE_IDLE;
    state->characters[0].frame_id = 0;       // Default animation frame
    state->characters[0].on_ground = false;
    state->characters[0].alive = true;

    // Initialize Watergirl (Player 1)
    state->characters[1].x = 100.0f;         // Starting X position
    state->characters[1].y = (MAP_HEIGHT - 4) * TILE_SIZE; // Starting Y position
    state->characters[1].velocity_x = 0.0f;
    state->characters[1].velocity_y = 0.0f;
    state->characters[1].width = TILE_SIZE;
    state->characters[1].height = TILE_SIZE;
    state->characters[1].type = CHARACTER_WATERGIRL;
    state->characters[1].state = STATE_IDLE;
    state->characters[1].frame_id = 10;      // Different starting frame (example)
    state->characters[1].on_ground = false;
    state->characters[1].alive = true;
    
    printf("Game state initialized for map %d\n", map_id);
}

/**
 * @brief Updates the state of a single character based on input and physics (FSM).
 * Handles state transitions (idle, moving, jumping, falling) and applies gravity.
 * 
 * @param character Pointer to the character_t structure to update.
 * @param action The game_action_t derived from player input for this character.
 * @param delta_time Time elapsed since the last frame (seconds).
 */
void update_character_state(character_t* character, game_action_t action, float delta_time) {
    // If character is dead, lock state to STATE_DEAD
    if (!character->alive) {
        character->state = STATE_DEAD;
        character->velocity_x = 0;
        // Optional: Add death animation logic here (e.g., setting a specific frame_id)
        return;
    }

    // Apply gravity if the character is not on the ground
    if (!character->on_ground) {
        character->velocity_y += GRAVITY * delta_time;
        // Optional: Add terminal velocity cap
        // if (character->velocity_y > MAX_FALL_SPEED) character->velocity_y = MAX_FALL_SPEED;
    }

    // Finite State Machine (FSM) for character state transitions
    switch (character->state) {
        case STATE_IDLE:
        case STATE_MOVING_LEFT:
        case STATE_MOVING_RIGHT:
            // Check for jump input (only possible if on ground)
            if (action == ACTION_JUMP && character->on_ground) {
                character->velocity_y = JUMP_VELOCITY; // Apply initial jump velocity
                character->state = STATE_JUMPING;
                character->on_ground = false; // Character is now airborne
            } 
            // Check for horizontal movement input
            else if (action == ACTION_MOVE_LEFT) {
                character->velocity_x = -MOVE_SPEED; // Set horizontal velocity
                character->state = STATE_MOVING_LEFT;
            } else if (action == ACTION_MOVE_RIGHT) {
                character->velocity_x = MOVE_SPEED;
                character->state = STATE_MOVING_RIGHT;
            } 
            // No movement input
            else {
                character->velocity_x = 0; // Stop horizontal movement
                // Only transition to IDLE if not already moving vertically
                if (character->on_ground) {
                    character->state = STATE_IDLE;
                }
            }
            // Transition to falling if airborne and moving downwards or not jumping
            if (!character->on_ground && character->velocity_y > 0.1f) { // Add small threshold
                 character->state = STATE_FALLING;
            }
            break;

        case STATE_JUMPING:
            // Allow horizontal movement control while jumping
            if (action == ACTION_MOVE_LEFT) {
                character->velocity_x = -MOVE_SPEED;
            } else if (action == ACTION_MOVE_RIGHT) {
                character->velocity_x = MOVE_SPEED;
            } else {
                character->velocity_x = 0;
            }
            // Transition to falling state when vertical velocity becomes non-negative (peak of jump or moving down)
            if (character->velocity_y >= 0) {
                character->state = STATE_FALLING;
            }
            break;

        case STATE_FALLING:
            // Allow horizontal movement control while falling
            if (action == ACTION_MOVE_LEFT) {
                character->velocity_x = -MOVE_SPEED;
            } else if (action == ACTION_MOVE_RIGHT) {
                character->velocity_x = MOVE_SPEED;
            } else {
                character->velocity_x = 0;
            }
            // Transition back to idle or moving state if the character lands on the ground
            if (character->on_ground) {
                 // Determine state based on horizontal velocity upon landing
                 character->state = (character->velocity_x == 0) ? STATE_IDLE : 
                                    (character->velocity_x < 0 ? STATE_MOVING_LEFT : STATE_MOVING_RIGHT);
            }
            break;
            
        case STATE_DEAD:
            // Character remains in the dead state. No actions possible.
            // Gravity might still apply depending on desired death effect.
            // character->velocity_y += GRAVITY * delta_time; // Optional: allow dead body to fall
            break;
    }
    
    // Placeholder: Update animation frame based on state and character type
    // This logic would typically involve timers and sprite sheets.
    // Example: character->frame_id = get_animation_frame(character->state, character->type, delta_time);
}

/**
 * @brief Updates the character's position based on its current velocity and delta time.
 * Also performs basic boundary checks to keep the character within screen limits.
 * 
 * @param character Pointer to the character_t structure to update.
 * @param delta_time Time elapsed since the last frame (seconds).
 */
void update_character_position(character_t* character, float delta_time) {
    if (!character->alive) return; // Don't update position if dead

    // Update position based on velocity: position = position + velocity * time
    character->x += character->velocity_x * delta_time;
    character->y += character->velocity_y * delta_time;

    // Basic screen boundary checks (adjust as needed for camera/scrolling)
    if (character->x < 0) {
        character->x = 0;
        character->velocity_x = 0; // Stop movement at boundary
    }
    if (character->x + character->width > MAP_WIDTH * TILE_SIZE) { // Check against map width
        character->x = (MAP_WIDTH * TILE_SIZE) - character->width;
        character->velocity_x = 0; // Stop movement at boundary
    }
    // Vertical boundaries might be handled differently (e.g., death pit at bottom)
    if (character->y < 0) { // Prevent going above the top
         character->y = 0;
         if (character->velocity_y < 0) character->velocity_y = 0;
    }
    // Note: Ground collision handling (snapping to ground) is done in detect_tile_collision
}

/**
 * @brief Gets the type of tile at a specific pixel coordinate on the map.
 * Converts pixel coordinates to tile indices and returns the tile type from map_data.
 * Includes bounds checking.
 * 
 * @param x The X pixel coordinate.
 * @param y The Y pixel coordinate.
 * @param map_id The ID of the current map (currently unused, assumes one map).
 * @return The tile_type_t at the given coordinates, or TILE_EMPTY if out of bounds.
 */
tile_type_t get_tile_at(int x, int y, uint8_t map_id) {
    // Suppress unused parameter warning for map_id in this simple version
    (void)map_id;

    // Convert pixel coordinates to tile coordinates
    int tile_x = x / TILE_SIZE;
    int tile_y = y / TILE_SIZE;

    // Check if the coordinates are within the map boundaries
    if (tile_x < 0 || tile_x >= MAP_WIDTH || tile_y < 0 || tile_y >= MAP_HEIGHT) {
        return TILE_EMPTY; // Treat out-of-bounds as empty space
    }

    // Return the tile type from the map data array
    return map_data[tile_y][tile_x];
}

/**
 * @brief Detects and resolves collisions between a character and the map tiles.
 * Uses a simplified Axis-Aligned Bounding Box (AABB) approach.
 * Checks tiles around the character's predicted next position.
 * Updates character's position, velocity, and on_ground status based on collisions.
 * 
 * @param character Pointer to the character_t structure.
 * @param map_id The ID of the current map.
 * @return The type of tile the character primarily collided with (e.g., for hazard checks).
 */
tile_type_t detect_tile_collision(character_t* character, uint8_t map_id) {
    if (!character->alive) return TILE_EMPTY;

    character->on_ground = false; // Assume not on ground unless collision proves otherwise
    tile_type_t significant_collision = TILE_EMPTY;

    // --- Collision Detection Logic --- 
    // A more robust implementation would check the predicted next position.
    // This simplified version checks around the current position.

    // Define character bounding box edges
    int char_left = (int)floor(character->x);
    int char_right = (int)ceil(character->x + character->width);
    int char_top = (int)floor(character->y);
    int char_bottom = (int)ceil(character->y + character->height);

    // Convert pixel edges to tile indices
    int tile_left = char_left / TILE_SIZE;
    int tile_right = (char_right - 1) / TILE_SIZE; // -1 to handle edges correctly
    int tile_top = char_top / TILE_SIZE;
    int tile_bottom = (char_bottom - 1) / TILE_SIZE;

    // Iterate through the tiles the character potentially overlaps with
    for (int ty = tile_top; ty <= tile_bottom; ++ty) {
        for (int tx = tile_left; tx <= tile_right; ++tx) {
            tile_type_t tile = get_tile_at(tx * TILE_SIZE, ty * TILE_SIZE, map_id);

            // Check if the tile is solid (Wall or Platform)
            if (tile == TILE_WALL || tile == TILE_PLATFORM) {
                // Calculate tile boundaries
                int tile_rect_left = tx * TILE_SIZE;
                int tile_rect_right = tile_rect_left + TILE_SIZE;
                int tile_rect_top = ty * TILE_SIZE;
                int tile_rect_bottom = tile_rect_top + TILE_SIZE;

                // --- Collision Resolution (Simplified) --- 
                // This needs refinement for better physics feel.
                
                // Check vertical collision (landing on top or hitting head)
                if (character->velocity_y > 0 && char_bottom > tile_rect_top && char_top < tile_rect_top) { // Landing
                    character->y = (float)tile_rect_top - character->height;
                    character->velocity_y = 0;
                    character->on_ground = true;
                    significant_collision = tile; 
                } else if (character->velocity_y < 0 && char_top < tile_rect_bottom && char_bottom > tile_rect_bottom) { // Hitting head
                    // Allow passing through platforms from below?
                    if (tile == TILE_WALL) { 
                        character->y = (float)tile_rect_bottom;
                        character->velocity_y = 0;
                        significant_collision = tile; 
                    }
                }
                
                // Check horizontal collision (hitting side)
                // Re-check bounds after potential vertical correction
                char_top = (int)floor(character->y);
                char_bottom = (int)ceil(character->y + character->height);
                
                if (character->velocity_x > 0 && char_right > tile_rect_left && char_left < tile_rect_left && char_bottom > tile_rect_top && char_top < tile_rect_bottom) { // Moving right into tile
                    character->x = (float)tile_rect_left - character->width;
                    character->velocity_x = 0;
                    significant_collision = tile; 
                } else if (character->velocity_x < 0 && char_left < tile_rect_right && char_right > tile_rect_right && char_bottom > tile_rect_top && char_top < tile_rect_bottom) { // Moving left into tile
                    character->x = (float)tile_rect_right;
                    character->velocity_x = 0;
                    significant_collision = tile; 
                }
            }
            // Check for non-solid tile interactions (hazards, items)
            else if (tile == TILE_FIRE || tile == TILE_WATER || tile == TILE_POISON || 
                     tile == TILE_GEM_RED || tile == TILE_GEM_BLUE || tile == TILE_EXIT ||
                     tile == TILE_BUTTON || tile == TILE_LEVER) {
                 // Simple overlap check for interaction tiles
                 int tile_rect_left = tx * TILE_SIZE;
                 int tile_rect_right = tile_rect_left + TILE_SIZE;
                 int tile_rect_top = ty * TILE_SIZE;
                 int tile_rect_bottom = tile_rect_top + TILE_SIZE;
                 
                 if (char_right > tile_rect_left && char_left < tile_rect_right && char_bottom > tile_rect_top && char_top < tile_rect_bottom) {
                     // Handle the specific interaction/hazard in handle_collision_response
                     handle_collision_response(character, tile);
                     // Potentially remove the tile (e.g., collected gem)
                     if (tile == TILE_GEM_RED || tile == TILE_GEM_BLUE) {
                         map_data[ty][tx] = TILE_EMPTY; // Remove collected gem
                         // Need access to game_state to increment gem count
                     }
                     if (significant_collision == TILE_EMPTY) significant_collision = tile;
                 }
            }
        }
    }

    return significant_collision;
}

/**
 * @brief Detects collisions between the two player characters using AABB.
 * 
 * @param char1 Pointer to the first character_t structure.
 * @param char2 Pointer to the second character_t structure.
 * @return true if the characters' bounding boxes overlap, false otherwise.
 */
bool detect_character_collision(character_t* char1, character_t* char2) {
    // Don't check collision if either character is dead
    if (!char1->alive || !char2->alive) return false;

    // Calculate bounding box edges for both characters
    float left1 = char1->x;
    float right1 = char1->x + char1->width;
    float top1 = char1->y;
    float bottom1 = char1->y + char1->height;

    float left2 = char2->x;
    float right2 = char2->x + char2->width;
    float top2 = char2->y;
    float bottom2 = char2->y + char2->height;

    // Check for non-overlap using the Separating Axis Theorem (SAT) for AABBs
    if (right1 < left2 || left1 > right2 || bottom1 < top2 || top1 > bottom2) {
        return false; // No overlap
    }

    // If none of the non-overlap conditions are met, the boxes must be colliding.
    printf("Character collision detected!\n"); // Debug message
    return true;
}

/**
 * @brief Handles the response to a collision between a character and a specific map tile type.
 * Applies effects like death for hazards.
 * Placeholder for item collection, button presses etc.
 * 
 * @param character Pointer to the character_t structure that collided.
 * @param tile_type The type of tile (tile_type_t) the character collided with.
 */
void handle_collision_response(character_t* character, tile_type_t tile_type) {
    if (!character->alive) return; // No response if already dead

    switch (tile_type) {
        case TILE_WALL:
        case TILE_PLATFORM:
            // Solid collision resolution is handled in detect_tile_collision
            break;
            
        case TILE_FIRE:
            if (character->type == CHARACTER_WATERGIRL) {
                character->alive = false;
                printf("Watergirl died in fire!\n");
                // play_sfx(AUDIO_DEATH); // Trigger death sound
            }
            break;
            
        case TILE_WATER:
            if (character->type == CHARACTER_FIREBOY) {
                character->alive = false;
                printf("Fireboy died in water!\n");
                // play_sfx(AUDIO_DEATH);
            }
            break;
            
        case TILE_POISON:
            character->alive = false;
            printf("Character died in poison!\n");
            // play_sfx(AUDIO_DEATH);
            break;
            
        case TILE_GEM_RED:
            if (character->type == CHARACTER_FIREBOY) {
                printf("Fireboy collected red gem!\n");
                // Increment gem count (needs game_state access)
                // play_sfx(AUDIO_GEM_COLLECT);
                // Tile removal is handled in detect_tile_collision
            }
            break;
            
        case TILE_GEM_BLUE:
             if (character->type == CHARACTER_WATERGIRL) {
                printf("Watergirl collected blue gem!\n");
                // Increment gem count (needs game_state access)
                // play_sfx(AUDIO_GEM_COLLECT);
                // Tile removal is handled in detect_tile_collision
            }
            break;
            
        case TILE_BUTTON:
            printf("Character on button!\n");
            // Trigger button action (needs game_state access to modify door states etc.)
            break;
            
        case TILE_LEVER:
            printf("Character activated lever!\n");
            // Trigger lever action (needs game_state access)
            break;
            
        case TILE_DOOR:
            // Check if door is open/closed and block movement if necessary
            // This logic might be better placed in collision detection/resolution
            break;
            
        case TILE_EXIT:
            printf("Character reached exit!\n");
            // Check if conditions met for level complete (needs game_state access)
            break;
            
        default:
            // No specific response for TILE_EMPTY or other unhandled types
            break;
    }
}

/**
 * @brief Handles game interactions like buttons affecting doors (Placeholder).
 * This function would typically check which buttons/levers are pressed
 * and update the state of corresponding mechanisms (e.g., open doors).
 * 
 * @param state Pointer to the current game_state_t.
 */
void handle_interactions(game_state_t* state) {
    // Suppress unused parameter warning for the placeholder
    (void)state;
    // Placeholder: 
    // 1. Check if Fireboy is on a red button.
    // 2. Check if Watergirl is on a blue button.
    // 3. Check if either character activated a lever.
    // 4. Update the state of doors or platforms based on button/lever states.
    //    (This might involve modifying the map_data array or using separate state variables).
}

/**
 * @brief Updates the overall game state for a single frame.
 * Orchestrates input reading, character state/position updates, collision detection,
 * interaction handling, and checking for game over or level complete conditions.
 * 
 * @param state Pointer to the game_state_t structure to update.
 * @param delta_time The time elapsed since the last frame (in seconds).
 */
void update_game_state(game_state_t* state, float delta_time) {
    // Don't update if game is over or level is complete
    if (state->game_over || state->level_complete) return;

    // 1. Get Player Input
    // Note: Input reading should ideally happen once per frame in main loop,
    // but called here for simplicity in this structure.
    game_action_t action0 = get_player_action(0); // Fireboy
    game_action_t action1 = get_player_action(1); // Watergirl

    // 2. Update Character States (FSM) based on input and physics
    update_character_state(&state->characters[0], action0, delta_time);
    update_character_state(&state->characters[1], action1, delta_time);

    // 3. Update Character Positions based on velocity
    update_character_position(&state->characters[0], delta_time);
    update_character_position(&state->characters[1], delta_time);

    // 4. Detect and Handle Collisions with Map Tiles
    // This updates position/velocity based on solid tiles and handles hazards/items.
    // It also updates the on_ground status.
    detect_tile_collision(&state->characters[0], state->active_map);
    detect_tile_collision(&state->characters[1], state->active_map);
    // Note: Hardware collision detection (reading STATUS_REG) is an alternative 
    //       or supplementary approach not fully implemented here.

    // 5. Detect and Handle Character-Character Collisions (Optional)
    // if (detect_character_collision(&state->characters[0], &state->characters[1])) {
    //     // Implement response if needed (e.g., pushing, specific interactions)
    //     // handle_character_collision_response(&state->characters[0], &state->characters[1]);
    // }

    // 6. Handle Game Interactions (Buttons, Levers, Doors)
    handle_interactions(state);

    // 7. Check Game Over / Level Complete Conditions
    // Game over if either character is not alive
    if (!state->characters[0].alive || !state->characters[1].alive) {
        if (!state->game_over) { // Check to print only once
             state->game_over = true;
             printf("Game Over!\n");
             // play_sfx(AUDIO_DEATH); // Or a specific game over sound
        }
    }
    
    // Placeholder: Check for level complete condition
    // Example: Both characters are alive and overlapping with their respective exit tiles.
    // bool fireboy_at_exit = check_overlap(state->characters[0], EXIT_TILE_FIREBOY_COORDS);
    // bool watergirl_at_exit = check_overlap(state->characters[1], EXIT_TILE_WATERGIRL_COORDS);
    // if (state->characters[0].alive && state->characters[1].alive && fireboy_at_exit && watergirl_at_exit) {
    //     if (!state->level_complete) { // Check to print only once
    //         state->level_complete = true;
    //         printf("Level Complete!\n");
    //         // play_sfx(AUDIO_LEVEL_COMPLETE);
    //     }
    // }
}

/**
 * @brief Task function to be called during VBlank to update hardware state.
 * Prepares the sprite attribute data from the game state and calls the 
 * hardware interface function to update the sprite table in BRAM.
 * 
 * @param arg A void pointer expected to point to the current game_state_t.
 */
static void sync_game_state_to_hardware_task(void* arg) {
    if (arg == NULL) return;
    game_state_t* state = (game_state_t*)arg;
    
    sprite_attr_t sprites[MAX_SPRITES];
    int sprite_count = 0;

    // Prepare sprite data for Fireboy
    if (sprite_count < MAX_SPRITES) {
        sprites[sprite_count].x = (uint16_t)state->characters[0].x;
        sprites[sprite_count].y = (uint16_t)state->characters[0].y;
        sprites[sprite_count].frame_id = state->characters[0].frame_id; // Use frame ID from game logic
        sprites[sprite_count].priority = 0; // Example priority
        sprites[sprite_count].width = state->characters[0].width;
        sprites[sprite_count].height = state->characters[0].height;
        sprites[sprite_count].flags.value = 0; // Reset flags
        sprites[sprite_count].flags.bits.alive = state->characters[0].alive ? 1 : 0;
        // Set other flags based on character state if needed by hardware
        // sprites[sprite_count].flags.bits.jumping = (state->characters[0].state == STATE_JUMPING); 
        sprite_count++;
    }

    // Prepare sprite data for Watergirl
    if (sprite_count < MAX_SPRITES) {
        sprites[sprite_count].x = (uint16_t)state->characters[1].x;
        sprites[sprite_count].y = (uint16_t)state->characters[1].y;
        sprites[sprite_count].frame_id = state->characters[1].frame_id;
        sprites[sprite_count].priority = 1; // Example priority (higher than Fireboy?)
        sprites[sprite_count].width = state->characters[1].width;
        sprites[sprite_count].height = state->characters[1].height;
        sprites[sprite_count].flags.value = 0; // Reset flags
        sprites[sprite_count].flags.bits.alive = state->characters[1].alive ? 1 : 0;
        sprite_count++;
    }

    // Placeholder: Add other sprites (gems, enemies, UI elements) to the array here
    // ...
    
    // Ensure remaining sprite slots are marked as not alive
    for (int i = sprite_count; i < MAX_SPRITES; ++i) {
        sprites[i].flags.bits.alive = 0;
    }

    // Update the entire sprite attribute table in hardware BRAM
    update_sprite_table(sprites, MAX_SPRITES); // Send all slots

    // Optionally update control register if needed (e.g., trigger audio, change map)
    // Note: Specific triggers like audio might be better handled immediately when the event occurs,
    // but using VBlank sync ensures atomicity if multiple control bits change.
    // control_reg_t control = { .value = read_control_reg_value_somehow() }; // Need a way to get current value
    // control.bits.some_flag = ...; 
    // write_control_reg(control); 
}

/**
 * @brief Synchronizes the current game state to the hardware via VBlank.
 * Calls wait_for_vblank, passing the sync_game_state_to_hardware_task function
 * and the current game state as arguments.
 * 
 * @param state Pointer to the current game_state_t.
 */
void sync_game_state_to_hardware(game_state_t* state) {
    // Pass the task function and the game state to wait_for_vblank
    wait_for_vblank(sync_game_state_to_hardware_task, state);
}

