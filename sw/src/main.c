/**
 * @file main.c
 * @brief Main entry point and game loop for the Forest Fire and Ice game.
 * 
 * This file contains the main function which initializes all necessary modules,
 * runs the main game loop, and handles module cleanup upon exit.
 * The game loop aims for a fixed frame rate (TARGET_FPS) and orchestrates
 * game state updates and hardware synchronization.
 */

#include "../include/hw_interface.h"
#include "../include/input_handler.h"
#include "../include/game_logic.h"
#include "../include/audio_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> // For usleep()
#include <time.h>   // For clock_gettime()

// --- Constants --- 

/// Target frame rate for the game loop (Frames Per Second).
#define TARGET_FPS 60
/// Target time duration for a single frame in microseconds.
#define FRAME_TIME_US (1000000 / TARGET_FPS)

/**
 * @brief Main function - the entry point of the software application.
 * 
 * - Initializes hardware interface, input handler, game logic, and audio control modules.
 * - Initializes the game state.
 * - Enters the main game loop:
 *   - Calculates delta time.
 *   - Updates the game state (logic, physics, collisions).
 *   - Synchronizes the game state with the hardware (sprites, etc.) via VBlank.
 *   - Checks for game over or level complete conditions.
 *   - Sleeps to maintain the target frame rate.
 * - Cleans up all initialized modules upon exiting the loop.
 * 
 * @return 0 on successful execution, -1 if initialization fails.
 */
int main() {
    printf("--- Forest Fire and Ice Game --- \n");
    
    // --- Module Initialization --- 
    printf("Initializing modules...\n");
    if (hw_interface_init() != 0) {
        fprintf(stderr, "Fatal Error: Hardware interface initialization failed!\n");
        return -1;
    }
    
    if (input_handler_init() != 0) {
        fprintf(stderr, "Fatal Error: Input handler initialization failed!\n");
        hw_interface_cleanup(); // Clean up already initialized modules
        return -1;
    }
    
    if (game_logic_init() != 0) {
        fprintf(stderr, "Fatal Error: Game logic initialization failed!\n");
        input_handler_cleanup();
        hw_interface_cleanup();
        return -1;
    }
    
    if (audio_control_init() != 0) {
        fprintf(stderr, "Fatal Error: Audio control initialization failed!\n");
        game_logic_cleanup();
        input_handler_cleanup();
        hw_interface_cleanup();
        return -1;
    }
    printf("All modules initialized successfully.\n");
    
    // --- Game State Initialization --- 
    game_state_t game_state;
    // Start level 0 (or load the first level based on game design)
    init_game_state(&game_state, 0); 
    
    // --- Timing Variables --- 
    struct timespec frame_start, frame_end;
    long frame_duration_us; // Duration of the current frame in microseconds
    float delta_time = 1.0f / TARGET_FPS; // Initialize delta_time to target frame time
    
    // --- Main Game Loop --- 
    printf("Starting main game loop...\n");
    bool running = true;
    while (running) {
        // 1. Record Frame Start Time
        clock_gettime(CLOCK_MONOTONIC, &frame_start);
        
        // 2. Update Game State
        // This function handles input processing (via get_player_action), 
        // character FSM, physics, collision detection, and interactions.
        update_game_state(&game_state, delta_time);
        
        // 3. Synchronize Game State with Hardware
        // This updates the sprite attribute table in BRAM during VBlank.
        sync_game_state_to_hardware(&game_state);
        
        // 4. Check for Exit Conditions
        if (game_state.game_over) {
            printf("Game Over condition met. Exiting loop.\n");
            // Optional: Add delay or game over screen logic here
            running = false;
        }
        else if (game_state.level_complete) {
            printf("Level Complete condition met. Exiting loop.\n");
            // Optional: Add delay, score screen, or load next level logic here
            running = false;
        }
        
        // --- Frame Rate Control --- 
        // 5. Record Frame End Time and Calculate Duration
        clock_gettime(CLOCK_MONOTONIC, &frame_end);
        frame_duration_us = (frame_end.tv_sec - frame_start.tv_sec) * 1000000 + 
                            (frame_end.tv_nsec - frame_start.tv_nsec) / 1000;
        
        // 6. Calculate Delta Time for the *next* frame
        // Use the actual duration of the last frame for more accurate physics.
        delta_time = frame_duration_us / 1000000.0f;
        // Clamp delta_time to prevent physics issues if frame rate drops significantly
        if (delta_time > 0.1f) { // e.g., clamp at 10 FPS equivalent
            delta_time = 0.1f;
        }
        if (delta_time <= 0) { // Ensure delta_time is positive
            delta_time = 1.0f / TARGET_FPS; 
        }

        // 7. Sleep if Frame Completed Early
        if (frame_duration_us < FRAME_TIME_US) {
            usleep(FRAME_TIME_US - frame_duration_us);
        } else {
            // Optional: Log a warning if the frame took too long
            // printf("Warning: Frame duration exceeded target: %ld us\n", frame_duration_us);
        }
    } // End of main game loop
    
    // --- Module Cleanup --- 
    printf("Exiting game loop. Cleaning up modules...\n");
    audio_control_cleanup();
    game_logic_cleanup();
    input_handler_cleanup();
    hw_interface_cleanup();
    
    printf("--- Game Exited --- \n");
    return 0;
}

