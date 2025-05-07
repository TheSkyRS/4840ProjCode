/**
 * @file fpga_sprite_test.c
 * @brief User space test program for the FPGA Sprite Engine kernel driver
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>
#include "fpga_sprite.h"

#define DEVICE_PATH "/dev/fpga_sprite"

// Global variable for signal handling
volatile int running = 1;

/**
 * Signal handler for gracefully handling Ctrl+C
 */
void handle_signal(int sig) {
    printf("\nReceived signal %d, preparing to exit...\n", sig);
    running = 0;
}

/**
 * Create a sprite attribute structure
 */
fpga_sprite_attr_t make_sprite(int x, int y, int frame_id, int flip, int enable) {
    fpga_sprite_attr_t sprite;
    
    // Safety checks and value limits
    if (x < 0) x = 0;
    if (x > 639) x = 639;
    if (y < 0) y = 0;
    if (y > 479) y = 479;
    if (frame_id < 0) frame_id = 0;
    if (frame_id > 255) frame_id = 255;
    
    // Fill the struct bit fields
    sprite.bits.frame_id = frame_id;
    sprite.bits.pos_col = x;
    sprite.bits.pos_row = y;
    sprite.bits.reserved = 0;
    sprite.bits.flip = flip ? 1 : 0;
    sprite.bits.enable = enable ? 1 : 0;
    
    return sprite;
}

/**
 * Main function
 */
int main() {
    int fd;
    fpga_sprite_arg_t arg;
    int i;
    
    // Set up signal handling
    signal(SIGINT, handle_signal);
    
    printf("=== FPGA Sprite Engine Test Program ===\n");
    
    // Open the device
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Cannot open device");
        return -1;
    }
    printf("Device opened: %s\n", DEVICE_PATH);
    
    // Read initial status register value
    if (ioctl(fd, FPGA_SPRITE_READ_STATUS, &arg) < 0) {
        perror("Failed to read status register");
        close(fd);
        return -1;
    }
    
    printf("Initial status register value: 0x%08X\n", arg.status.value);
    printf("  - VCount: %d\n", arg.status.bits.vcount);
    printf("  - HCount: %d\n", arg.status.bits.hcount);
    printf("  - VBlank: %s\n", arg.status.bits.vblank ? "Yes" : "No");
    printf("  - Collision Detected: %s\n", arg.status.bits.collision_detected ? "Yes" : "No");
    
    // Configure control register
    arg.control.value = 0;
    arg.control.bits.enable_display = 1;
    arg.control.bits.enable_sprites = 1;
    arg.control.bits.enable_collision = 1;
    arg.control.bits.active_tilemap = 0;
    
    printf("Writing control register: 0x%08X\n", arg.control.value);
    if (ioctl(fd, FPGA_SPRITE_WRITE_CONTROL, &arg) < 0) {
        perror("Failed to write control register");
        close(fd);
        return -1;
    }
    
    // Create 5 test sprites
    printf("Creating test sprites...\n");
    
    // Red square (center)
    arg.sprites[0] = make_sprite(320, 240, 1, 0, 1);
    
    // Green square (top left)
    arg.sprites[1] = make_sprite(100, 100, 2, 0, 1);
    
    // Blue square (top right) - horizontally flipped
    arg.sprites[2] = make_sprite(540, 100, 3, 1, 1);
    
    // Yellow square (bottom left)
    arg.sprites[3] = make_sprite(100, 380, 4, 0, 1);
    
    // Purple square (bottom right) - horizontally flipped
    arg.sprites[4] = make_sprite(540, 380, 5, 1, 1);
    
    // Update sprite table
    arg.count = 5;
    printf("Updating sprite attribute table...\n");
    if (ioctl(fd, FPGA_SPRITE_UPDATE_SPRITE_TABLE, &arg) < 0) {
        perror("Failed to update sprite table");
        close(fd);
        return -1;
    }
    
    // Play startup sound effect
    printf("Playing startup sound effect...\n");
    arg.audio_id = 1;
    if (ioctl(fd, FPGA_SPRITE_PLAY_AUDIO, &arg) < 0) {
        perror("Failed to play audio");
        close(fd);
        return -1;
    }
    
    // Main loop - move the center square and check for collisions
    printf("Entering main loop, press Ctrl+C to exit...\n");
    
    int vx = 2, vy = 1;  // Central square velocity
    int frame_count = 0;
    int x = 320, y = 240;  // Current position
    
    while (running) {
        // Update central square position
        x += vx;
        y += vy;
        
        // Update sprite with new position
        arg.sprite_update.index = 0;
        arg.sprite_update.attr = make_sprite(x, y, 1, 0, 1);
        
        // Simple collision detection and boundary bouncing
        if (x < 0 || x > 639 - 32) {
            vx = -vx;
            
            // Play collision sound effect
            arg.audio_id = 2;
            ioctl(fd, FPGA_SPRITE_PLAY_AUDIO, &arg);
        }
        
        if (y < 0 || y > 479 - 32) {
            vy = -vy;
            
            // Play collision sound effect
            arg.audio_id = 2;
            ioctl(fd, FPGA_SPRITE_PLAY_AUDIO, &arg);
        }
        
        // Switch tilemap every 60 frames
        if (frame_count % 60 == 0) {
            arg.tilemap_id = (frame_count / 60) % 4;
            printf("Switching to tilemap %d\n", arg.tilemap_id);
            ioctl(fd, FPGA_SPRITE_SET_TILEMAP, &arg);
        }
        
        // Wait for VBlank
        ioctl(fd, FPGA_SPRITE_WAIT_VBLANK, NULL);
        
        // Update sprite position
        ioctl(fd, FPGA_SPRITE_WRITE_SPRITE, &arg);
        
        // Read status register, check for collisions
        ioctl(fd, FPGA_SPRITE_READ_STATUS, &arg);
        if (arg.status.bits.collision_detected) {
            printf("Collision detected!\n");
            
            // Play special collision sound effect
            arg.audio_id = 3;
            ioctl(fd, FPGA_SPRITE_PLAY_AUDIO, &arg);
        }
        
        // Limit framerate to approximately 60FPS
        usleep(16667);  // ~60fps (1/60 second in microseconds)
        
        frame_count++;
    }
    
    // Clean up and exit
    close(fd);
    printf("=== Test program has exited ===\n");
    return 0;
} 