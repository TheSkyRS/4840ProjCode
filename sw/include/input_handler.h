#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <stdbool.h>

/**
 * @file input_handler.h
 * @brief Defines the interface for handling player input.
 * 
 * This file declares the functions and data types necessary for reading 
 * input (e.g., from a USB keyboard) and translating it into game actions.
 * Note: The current implementation uses placeholders and needs to be adapted
 * for the specific hardware/input library used (e.g., libusb, evdev).
 */

/**
 * @brief Enumeration of possible game actions derived from player input.
 */
typedef enum {
    ACTION_NONE = 0, ///< No action performed.
    ACTION_MOVE_LEFT,  ///< Move the character left.
    ACTION_MOVE_RIGHT, ///< Move the character right.
    ACTION_JUMP,       ///< Make the character jump.
    // Add other actions if needed (e.g., ACTION_INTERACT)
} game_action_t;

/**
 * @brief Initializes the input handling module.
 * This function should set up the connection to the input device (e.g., open keyboard device).
 * 
 * @return 0 on success, -1 on failure.
 */
int input_handler_init();

/**
 * @brief Cleans up the input handling module.
 * This function should release any resources used by the input handler (e.g., close device file).
 */
void input_handler_cleanup();

/**
 * @brief Gets the current game action for a specific player.
 * Reads the state of the input device (e.g., keyboard) and maps the pressed keys
 * to the corresponding game action for the specified player.
 * 
 * @param player_index The index of the player (0 for Fireboy, 1 for Watergirl).
 * @return The game_action_t corresponding to the current input for that player.
 *         Returns ACTION_NONE if no relevant key is pressed or input cannot be read.
 */
game_action_t get_player_action(int player_index);

#endif // INPUT_HANDLER_H

