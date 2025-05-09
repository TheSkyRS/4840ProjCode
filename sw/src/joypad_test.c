/**
 * @file joypad_test.c
 * @brief Joypad Input Test Program
 * 
 * This program demonstrates how to use the joypad_input module, including
 * initializing controllers, dynamically inserting controller devices,
 * and reading controller input to control game characters.
 * Supports the classic 8-button joypad layout with continuous press tracking.
 */

#include "../include/input_handler.h"
#include "../include/joypad_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

extern int insert_joypad(const char *device_path, int player_index);

/* Global variables */
static volatile int running = 1;

/**
 * @brief Signal handler function, for capturing Ctrl+C
 */
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nCaught Ctrl+C, exiting...\n");
        running = 0;
    }
}

/**
 * @brief Print player action information
 */
void print_action(int player_index, game_action_t action) {
    printf("Player %d: ", player_index + 1);
    
    switch (action) {
        case ACTION_NONE:
            printf("No action");
            break;
        case ACTION_MOVE_LEFT:
            printf("Moving left");
            break;
        case ACTION_MOVE_RIGHT:
            printf("Moving right");
            break;
        case ACTION_JUMP:
            printf("Jumping");
            break;
        default:
            printf("Unknown action");
            break;
    }
    printf("\n");
}

/**
 * @brief Display button state with duration
 */
void print_button_state(int player_index, int button_id, const char* button_name) {
    int state = get_joypad_button_state(player_index, button_id);
    unsigned long duration = get_button_press_duration(player_index, button_id);
    
    if (state) {
        printf("    %s: PRESSED (%.1f sec)", button_name, duration / 1000.0f);
    } else {
        printf("    %s: released", button_name);
    }
}

/**
 * @brief Format button state for compact display
 */
const char* format_button_state(int player_index, int button_id) {
    static char buffer[20];
    int state = get_joypad_button_state(player_index, button_id);
    unsigned long duration = get_button_press_duration(player_index, button_id);
    
    if (state) {
        if (duration < 500) {
            sprintf(buffer, "PRESSED");
        } else {
            sprintf(buffer, "HELD %.1fs", duration / 1000.0f);
        }
    } else {
        sprintf(buffer, "released");
    }
    
    return buffer;
}

/**
 * @brief Display joypad connection status and all button states
 */
void show_joypad_status() {
    printf("\n--- Joypad Connection Status ---\n");
    
    for (int i = 0; i < 2; i++) {
        printf("Player %d Joypad: ", i + 1);
        
        if (is_joypad_connected(i)) {
            printf("CONNECTED\n");
            
            // Display button states
            printf("  Direction buttons (left side):\n");
            print_button_state(i, JOYPAD_BTN_UP, "UP");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_DOWN, "DOWN");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_LEFT, "LEFT");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_RIGHT, "RIGHT");
            printf("\n");
            
            printf("  Function buttons (right side):\n");
            print_button_state(i, JOYPAD_BTN_A, "A");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_B, "B");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_X, "X");
            printf("\n");
            print_button_state(i, JOYPAD_BTN_Y, "Y");
            printf("\n");
        } else {
            printf("NOT CONNECTED\n");
        }
        printf("\n");
    }
}

/**
 * @brief Display menu and handle user input
 */
void show_menu() {
    printf("\n--- Joypad Test Menu ---\n");
    printf("1. Connect Player 1 Joypad\n");
    printf("2. Connect Player 2 Joypad\n");
    printf("3. Test Joypad Input\n");
    printf("4. Test Continuous Button Press\n");
    printf("5. Show Joypad Status\n");
    printf("0. Exit Program\n");
    printf("Select: ");
}

/**
 * @brief Test continuous button press functionality
 */
void test_continuous_press() {
    printf("Starting continuous press test, press Ctrl+C to exit test mode\n");
    printf("Press and hold buttons to see duration tracking\n\n");
    
    while (running) {
        // Clear screen
        printf("\033[2J\033[H");
        printf("=== Continuous Press Test ===\n");
        printf("Press and hold buttons to see duration tracking\n");
        printf("Press Ctrl+C to exit test mode\n\n");
        
        // Display joypad connection status
        for (int i = 0; i < 2; i++) {
            printf("Player %d Joypad: %s\n", 
                   i + 1, 
                   is_joypad_connected(i) ? "CONNECTED" : "NOT CONNECTED");
            
            if (is_joypad_connected(i)) {
                printf("  Button Press Durations:\n");
                
                for (int btn = 0; btn < JOYPAD_BUTTON_COUNT; btn++) {
                    const char* btn_name;
                    switch (btn) {
                        case JOYPAD_BTN_UP: btn_name = "UP"; break;
                        case JOYPAD_BTN_DOWN: btn_name = "DOWN"; break;
                        case JOYPAD_BTN_LEFT: btn_name = "LEFT"; break;
                        case JOYPAD_BTN_RIGHT: btn_name = "RIGHT"; break;
                        case JOYPAD_BTN_A: btn_name = "A"; break;
                        case JOYPAD_BTN_B: btn_name = "B"; break;
                        case JOYPAD_BTN_X: btn_name = "X"; break;
                        case JOYPAD_BTN_Y: btn_name = "Y"; break;
                        default: btn_name = "?"; break;
                    }
                    
                    unsigned long duration = get_button_press_duration(i, btn);
                    if (duration > 0) {
                        printf("    %s: Pressed for %.2f seconds\n", 
                               btn_name, duration / 1000.0f);
                    }
                }
                
                printf("\n  Direction Pad:     Function Buttons:\n");
                printf("       [%s]           [%s] [%s]\n", 
                       format_button_state(i, JOYPAD_BTN_UP),
                       format_button_state(i, JOYPAD_BTN_Y),
                       format_button_state(i, JOYPAD_BTN_X));
                       
                printf("  [%s] [%s]      [%s] [%s]\n", 
                       format_button_state(i, JOYPAD_BTN_LEFT),
                       format_button_state(i, JOYPAD_BTN_RIGHT),
                       format_button_state(i, JOYPAD_BTN_B),
                       format_button_state(i, JOYPAD_BTN_A));
                       
                printf("       [%s]\n", 
                       format_button_state(i, JOYPAD_BTN_DOWN));
            }
            printf("\n");
        }
        
        // Small delay to avoid too fast output
        usleep(50000); // 50ms
    }
}

/**
 * @brief Main function
 */
int main() {
    char input[256];
    int choice;
    char device_path[128];
    
    // Set up signal handling
    signal(SIGINT, handle_signal);
    
    // Initialize input handling module
    if (input_handler_init() != 0) {
        fprintf(stderr, "Failed to initialize input handling module\n");
        return -1;
    }
    
    printf("=== Joypad Input Test Program ===\n");
    printf("This program demonstrates how to use joypads to control game characters\n");
    printf("Supports continuous button press tracking\n");
    
    // Display initial joypad status
    show_joypad_status();

    while (running) {
        show_menu();
        
        // Get user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        choice = atoi(input);
        
        switch (choice) {
            case 0:
                running = 0;
                break;
                
            case 1:
                printf("Enter Player 1 joypad device path (default: %s): ", get_default_joypad_path(0));
                if (fgets(input, sizeof(input), stdin) != NULL) {
                    input[strcspn(input, "\n")] = 0; // Remove newline
                    
                    // Use default value if input is empty
                    if (strlen(input) == 0) {
                        strcpy(device_path, get_default_joypad_path(0));
                    } else {
                        strcpy(device_path, input);
                    }
                    
                    if (insert_joypad(device_path, 0) == 0) {
                        printf("Successfully connected Player 1 joypad\n");
                    }
                }
                break;
                
            case 2:
                printf("Enter Player 2 joypad device path (default: %s): ", get_default_joypad_path(1));
                if (fgets(input, sizeof(input), stdin) != NULL) {
                    input[strcspn(input, "\n")] = 0; // Remove newline
                    
                    // Use default value if input is empty
                    if (strlen(input) == 0) {
                        strcpy(device_path, get_default_joypad_path(1));
                    } else {
                        strcpy(device_path, input);
                    }
                    
                    if (insert_joypad(device_path, 1) == 0) {
                        printf("Successfully connected Player 2 joypad\n");
                    }
                }
                break;
                
            case 3:
                printf("Starting joypad input test, press Ctrl+C to exit test mode\n");
                
                // Loop to read and display joypad input
                while (running) {
                    game_action_t action1 = get_player_action(0);
                    game_action_t action2 = get_player_action(1);
                    
                    // Clear screen
                    printf("\033[2J\033[H");
                    printf("=== Joypad Input Test ===\n");
                    printf("Press Ctrl+C to exit test mode\n\n");
                    
                    // Display joypad connection status
                    for (int i = 0; i < 2; i++) {
                        printf("Player %d Joypad: %s\n", 
                               i + 1, 
                               is_joypad_connected(i) ? "CONNECTED" : "NOT CONNECTED");
                        
                        if (is_joypad_connected(i)) {
                            // Display all button states
                            printf("  Direction buttons:\n");
                            printf("    UP: %s  DOWN: %s  LEFT: %s  RIGHT: %s\n", 
                                get_joypad_button_state(i, JOYPAD_BTN_UP) ? "PRESSED" : "released",
                                get_joypad_button_state(i, JOYPAD_BTN_DOWN) ? "PRESSED" : "released",
                                get_joypad_button_state(i, JOYPAD_BTN_LEFT) ? "PRESSED" : "released",
                                get_joypad_button_state(i, JOYPAD_BTN_RIGHT) ? "PRESSED" : "released");
                                
                            printf("  Function buttons:\n");
                            printf("    A: %s  B: %s  X: %s  Y: %s\n", 
                                get_joypad_button_state(i, JOYPAD_BTN_A) ? "PRESSED" : "released",
                                get_joypad_button_state(i, JOYPAD_BTN_B) ? "PRESSED" : "released",
                                get_joypad_button_state(i, JOYPAD_BTN_X) ? "PRESSED" : "released",
                                get_joypad_button_state(i, JOYPAD_BTN_Y) ? "PRESSED" : "released");
                        }
                    }
                    printf("\n");
                    
                    // Display player actions
                    printf("Game Actions:\n");
                    print_action(0, action1);
                    print_action(1, action2);
                    
                    // Small delay to avoid too fast output
                    usleep(100000); // 100ms
                }
                break;
            
            case 4:
                test_continuous_press();
                break;
                
            case 5:
                show_joypad_status();
                break;
                
            default:
                printf("Invalid selection, please try again\n");
                break;
        }
    }
    
    // Clean up resources
    input_handler_cleanup();
    
    printf("Program has exited\n");
    return 0;
} 