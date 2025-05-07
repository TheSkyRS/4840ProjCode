# FPGA Sprite Engine Kernel Driver

This document describes how to build, install, and use the FPGA Sprite Engine kernel driver.

## Overview

The FPGA Sprite Engine driver is a Linux kernel module that provides an interface to interact with the FPGA hardware. It handles:

- Sprite attribute management
- Tilemap selection
- Collision detection
- Audio playback
- VBlank synchronization

## Building the Driver

### Prerequisites

- Linux kernel headers (matching your target kernel version)
- ARM cross-compiler (for Embedded Linux systems)
- GNU Make

### Build Instructions

1. Set the `KERNEL_DIR` environment variable to point to your kernel build directory:

```bash
export KERNEL_DIR=/path/to/kernel/build
```

2. Build the kernel module:

```bash
make kernel_module
```

This will generate the `fpga_sprite.ko` kernel module.

## Installing the Driver

1. Install the compiled kernel module:

```bash
sudo make install
```

2. Manually load the module:

```bash
sudo make load
```

Or load using insmod:

```bash
sudo insmod fpga_sprite.ko
```

3. Verify the module is loaded:

```bash
lsmod | grep fpga_sprite
```

4. The driver creates a device file at `/dev/fpga_sprite`

## Building the Test Application

1. Build the test application:

```bash
make user_app
```

2. The application will be built at `build/fpga_sprite_test`

## Running the Test Application

1. Make sure the FPGA is properly connected and configured
2. Ensure the kernel module is loaded
3. Run the test application:

```bash
./build/fpga_sprite_test
```

## Unloading the Driver

```bash
sudo make unload
```

Or unload using rmmod:

```bash
sudo rmmod fpga_sprite
```

## Troubleshooting

### Common Issues

1. **Module not loading**: Check dmesg for error messages:
   ```bash
   dmesg | tail
   ```

2. **Permission denied when accessing /dev/fpga_sprite**: 
   Ensure you have proper permissions:
   ```bash
   sudo chmod 666 /dev/fpga_sprite
   ```

3. **No such device**: Ensure the FPGA is properly connected and the device tree has 
   the correct compatible string (`csee4840,fpga-sprite-1.0`)

## Developer Reference

The driver provides access to the FPGA hardware through the following IOCTLs:

- `FPGA_SPRITE_READ_STATUS` - Read hardware status
- `FPGA_SPRITE_WRITE_CONTROL` - Configure hardware control register
- `FPGA_SPRITE_READ_SPRITE` - Read sprite attributes
- `FPGA_SPRITE_WRITE_SPRITE` - Update a sprite
- `FPGA_SPRITE_UPDATE_SPRITE_TABLE` - Update multiple sprites at once
- `FPGA_SPRITE_SET_TILEMAP` - Set active tilemap
- `FPGA_SPRITE_SET_COLLISION` - Enable/disable collision detection
- `FPGA_SPRITE_PLAY_AUDIO` - Trigger audio playback
- `FPGA_SPRITE_WAIT_VBLANK` - Wait for vertical blanking interval

See `include/fpga_sprite.h` for the full interface definition.

## API Usage Guide

This section explains how to use the FPGA Sprite Engine driver in your applications.

### Basic Usage Pattern

1. Include the header file:
```c
#include "../include/fpga_sprite.h"
```

2. Open the device:
```c
int fd = open("/dev/fpga_sprite", O_RDWR);
if (fd < 0) {
    perror("Cannot open device");
    return -1;
}
```

3. Initialize a parameter structure:
```c
fpga_sprite_arg_t arg;
```

4. Perform operations using IOCTL calls

5. Close the device when done:
```c
close(fd);
```

### Reading Status Register

```c
fpga_sprite_arg_t arg;
if (ioctl(fd, FPGA_SPRITE_READ_STATUS, &arg) < 0) {
    perror("Failed to read status register");
    return -1;
}

// Access status register fields
printf("VCount: %d\n", arg.status.bits.vcount);
printf("HCount: %d\n", arg.status.bits.hcount);
printf("VBlank: %s\n", arg.status.bits.vblank ? "Yes" : "No");
printf("Collision Detected: %s\n", arg.status.bits.collision_detected ? "Yes" : "No");
```

### Configuring Control Register

```c
fpga_sprite_arg_t arg;
arg.control.value = 0;  // Start with clean state

// Set individual control bits
arg.control.bits.enable_display = 1;
arg.control.bits.enable_sprites = 1;
arg.control.bits.enable_collision = 1;
arg.control.bits.active_tilemap = 0;

if (ioctl(fd, FPGA_SPRITE_WRITE_CONTROL, &arg) < 0) {
    perror("Failed to write control register");
    return -1;
}
```

### Creating and Updating Sprites

Helper function to create sprite attributes:
```c
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
```

Update a single sprite:
```c
fpga_sprite_arg_t arg;
arg.sprite_update.index = 0;  // Sprite index to update
arg.sprite_update.attr = make_sprite(320, 240, 1, 0, 1);  // Center, frame 1, no flip, enabled

if (ioctl(fd, FPGA_SPRITE_WRITE_SPRITE, &arg) < 0) {
    perror("Failed to update sprite");
    return -1;
}
```

Update multiple sprites at once:
```c
fpga_sprite_arg_t arg;

// Create sprite attributes
arg.sprites[0] = make_sprite(320, 240, 1, 0, 1);  // Center
arg.sprites[1] = make_sprite(100, 100, 2, 0, 1);  // Top left
arg.sprites[2] = make_sprite(540, 100, 3, 1, 1);  // Top right, flipped
arg.sprites[3] = make_sprite(100, 380, 4, 0, 1);  // Bottom left
arg.sprites[4] = make_sprite(540, 380, 5, 1, 1);  // Bottom right, flipped

// Set the number of sprites to update
arg.count = 5;

if (ioctl(fd, FPGA_SPRITE_UPDATE_SPRITE_TABLE, &arg) < 0) {
    perror("Failed to update sprite table");
    return -1;
}
```

### Setting Tilemap and Collision Detection

Set active tilemap:
```c
fpga_sprite_arg_t arg;
arg.tilemap_id = 2;  // Set active tilemap to #2

if (ioctl(fd, FPGA_SPRITE_SET_TILEMAP, &arg) < 0) {
    perror("Failed to set tilemap");
    return -1;
}
```

Enable/disable collision detection:
```c
fpga_sprite_arg_t arg;
arg.enable_collision = 1;  // Enable collision detection

if (ioctl(fd, FPGA_SPRITE_SET_COLLISION, &arg) < 0) {
    perror("Failed to set collision detection");
    return -1;
}
```

### Playing Audio

```c
fpga_sprite_arg_t arg;
arg.audio_id = 1;  // Play audio ID #1

if (ioctl(fd, FPGA_SPRITE_PLAY_AUDIO, &arg) < 0) {
    perror("Failed to play audio");
    return -1;
}
```

### Waiting for VBlank

```c
if (ioctl(fd, FPGA_SPRITE_WAIT_VBLANK, NULL) < 0) {
    perror("Failed to wait for VBlank");
    return -1;
}
```

### Complete Example: Animation Loop

```c
int fd = open("/dev/fpga_sprite", O_RDWR);
if (fd < 0) {
    perror("Cannot open device");
    return -1;
}

// Initialize sprites and control register
fpga_sprite_arg_t arg;

// Set up control register
arg.control.value = 0;
arg.control.bits.enable_display = 1;
arg.control.bits.enable_sprites = 1;
arg.control.bits.enable_collision = 1;
ioctl(fd, FPGA_SPRITE_WRITE_CONTROL, &arg);

// Create a sprite
int x = 320, y = 240;
int vx = 2, vy = 1;

// Main animation loop
while (running) {
    // Update position
    x += vx;
    y += vy;
    
    // Boundary checks and bounce
    if (x < 0 || x > 639 - 32) {
        vx = -vx;
    }
    
    if (y < 0 || y > 479 - 32) {
        vy = -vy;
    }
    
    // Update sprite
    arg.sprite_update.index = 0;
    arg.sprite_update.attr = make_sprite(x, y, 1, 0, 1);
    
    // Wait for VBlank
    ioctl(fd, FPGA_SPRITE_WAIT_VBLANK, NULL);
    
    // Write sprite in VBlank period
    ioctl(fd, FPGA_SPRITE_WRITE_SPRITE, &arg);
    
    // Check for collisions
    ioctl(fd, FPGA_SPRITE_READ_STATUS, &arg);
    if (arg.status.bits.collision_detected) {
        printf("Collision detected!\n");
    }
}

close(fd);
```

See the full test program in `src/fpga_sprite_test.c` for a more complete example. 