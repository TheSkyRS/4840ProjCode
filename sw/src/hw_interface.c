/**
 * @file hw_interface.c
 * @brief Implements the hardware interface functions for the FPGA game.
 *
 * This file provides the implementation for functions declared in hw_interface.h,
 * including memory mapping for MMIO access, and functions to read/write
 * hardware registers and BRAM.
 */

#include "../include/hw_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

// --- Static Global Variables ---

/// File descriptor for /dev/mem, used for memory mapping.
static int fd = -1;
/// Base pointer for the mapped MMIO memory region.
static void *mmio_base = NULL;

/// Pointer to the Control Register, derived from mmio_base after mapping.
static volatile control_reg_t *hw_control_reg_ptr = NULL;
/// Pointer to the Status Register, derived from mmio_base after mapping.
static volatile status_reg_t *hw_status_reg_ptr = NULL;
/// Pointer to the Sprite Attribute Table in BRAM, derived from mmio_base after mapping.
static volatile sprite_attr_t *hw_sprite_attr_table_ptr = NULL;

// --- Function Implementations ---

/**
 * @brief Initializes the hardware interface.
 * Opens /dev/mem and maps the FPGA MMIO region (defined by FPGA_MMIO_BASE)
 * into the process's virtual address space. It also initializes the pointers
 * to the specific hardware registers and BRAM table.
 *
 * @return 0 on success.
 * @return -1 on failure (e.g., cannot open /dev/mem, mmap fails).
 */
int hw_interface_init()
{
    // Check if already initialized
    if (fd >= 0 || mmio_base != NULL)
    {
        fprintf(stderr, "Warning: Hardware interface already initialized.\n");
        return 0; // Consider it a success if already initialized
    }

    // Open /dev/mem for physical memory access
    // Requires root privileges.
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0)
    {
        perror("Error opening /dev/mem");
        return -1;
    }

    // Map the FPGA MMIO region into virtual address space.
    // The size (0x1000) should be large enough to cover all required registers and BRAM.
    // MAP_SHARED ensures changes are written back to the hardware.
    mmio_base = mmap(NULL,                   // Let the kernel choose the address
                     0x1000,                 // Map 4KB (adjust if needed)
                     PROT_READ | PROT_WRITE, // Allow read and write access
                     MAP_SHARED,             // Share changes with other processes/hardware
                     fd,                     // File descriptor for /dev/mem
                     FPGA_MMIO_BASE);        // Physical base address of the MMIO region

    if (mmio_base == MAP_FAILED)
    {
        perror("Error mapping memory");
        close(fd);
        fd = -1;
        mmio_base = NULL; // Ensure mmio_base is NULL on failure
        return -1;
    }

    // Derive specific register/memory pointers from the base mapped address.
    // Casting mmio_base to char* allows for byte-level pointer arithmetic.
    hw_control_reg_ptr = (volatile control_reg_t *)((char *)mmio_base + CONTROL_REG_OFFSET);
    hw_status_reg_ptr = (volatile status_reg_t *)((char *)mmio_base + STATUS_REG_OFFSET);
    hw_sprite_attr_table_ptr = (volatile sprite_attr_t *)((char *)mmio_base + SPRITE_ATTR_TABLE_OFFSET);

    printf("Hardware interface initialized successfully. MMIO mapped at %p\n", mmio_base);
    return 0;
}

/**
 * @brief Cleans up the hardware interface.
 * Unmaps the previously mapped MMIO memory region and closes the /dev/mem
 * file descriptor. Resets the global pointers.
 */
void hw_interface_cleanup()
{
    if (mmio_base != NULL)
    {
        // Unmap the memory region
        if (munmap(mmio_base, 0x1000) == -1)
        {
            perror("Error unmapping memory");
        }
        mmio_base = NULL;
        hw_control_reg_ptr = NULL;
        hw_status_reg_ptr = NULL;
        hw_sprite_attr_table_ptr = NULL;
    }

    if (fd >= 0)
    {
        // Close the file descriptor
        if (close(fd) == -1)
        {
            perror("Error closing /dev/mem");
        }
        fd = -1;
    }

    printf("Hardware interface cleaned up.\n");
}

/**
 * @brief Reads the current value of the Status Register from the hardware.
 * Ensures the interface is initialized before attempting to read.
 *
 * @return The current status register value (status_reg_t). If the interface
 *         is not initialized, returns a zeroed structure.
 */
status_reg_t read_status_reg()
{
    status_reg_t status;
    if (hw_status_reg_ptr == NULL)
    {
        fprintf(stderr, "Error: Hardware interface not initialized. Cannot read status register.\n");
        status.value = 0; // Return a default/safe value
        return status;
    }
    // Read directly from the volatile pointer to the mapped register.
    status.value = hw_status_reg_ptr->value;
    return status;
}

/**
 * @brief Writes a value to the Control Register in the hardware.
 * Ensures the interface is initialized before attempting to write.
 *
 * @param control The control_reg_t value to write to the register.
 */
void write_control_reg(control_reg_t control)
{
    if (hw_control_reg_ptr == NULL)
    {
        fprintf(stderr, "Error: Hardware interface not initialized. Cannot write control register.\n");
        return;
    }
    // Write directly to the volatile pointer to the mapped register.
    hw_control_reg_ptr->value = control.value;
}

/**
 * @brief Updates the attributes of a single sprite in the hardware's Sprite Attribute Table.
 * Ensures the interface is initialized and the index is valid.
 *
 * @param sprite_index The index of the sprite to update (0 to MAX_SPRITES-1).
 * @param sprite The sprite_attr_t data containing the new attributes.
 * @return 0 on success.
 * @return -1 on failure (e.g., index out of bounds, interface not initialized).
 */
int update_sprite(uint8_t sprite_index, sprite_attr_t sprite)
{
    if (hw_sprite_attr_table_ptr == NULL)
    {
        fprintf(stderr, "Error: Hardware interface not initialized. Cannot update sprite.\n");
        return -1;
    }

    if (sprite_index >= MAX_SPRITES)
    {
        fprintf(stderr, "Error: Sprite index %d out of bounds (Max: %d).\n", sprite_index, MAX_SPRITES - 1);
        return -1;
    }

    // Write the sprite data to the corresponding location in the mapped BRAM table.
    hw_sprite_attr_table_ptr[sprite_index] = sprite;
    return 0;
}

/**
 * @brief Updates the attributes of multiple sprites in the hardware's Sprite Attribute Table.
 * Ensures the interface is initialized.
 *
 * @param sprites Pointer to an array of sprite_attr_t data.
 * @param count The number of sprites in the array to update (clamped to MAX_SPRITES).
 * @return 0 on success.
 * @return -1 on failure (e.g., interface not initialized).
 */
int update_sprite_table(sprite_attr_t *sprites, uint8_t count)
{
    if (hw_sprite_attr_table_ptr == NULL)
    {
        fprintf(stderr, "Error: Hardware interface not initialized. Cannot update sprite table.\n");
        return -1;
    }

    // Clamp count to the maximum number of sprites
    if (count > MAX_SPRITES)
    {
        fprintf(stderr, "Warning: Sprite count %d exceeds MAX_SPRITES (%d). Clamping to %d.\n", count, MAX_SPRITES, MAX_SPRITES);
        count = MAX_SPRITES;
    }

    // Copy the sprite data to the mapped BRAM table.
    for (uint8_t i = 0; i < count; i++)
    {
        hw_sprite_attr_table_ptr[i] = sprites[i];
    }

    return 0;
}

/**
 * @brief Waits until the VGA controller enters the vertical blanking interval (VBlank).
 * Then, optionally executes a provided callback function during VBlank.
 * Reads the vcount from the status register to determine the VBlank period.
 *
 * @param update_func A pointer to a function (void (*)(void*)) to be called during VBlank. Can be NULL.
 * @param arg An argument (void*) to be passed to the update_func callback.
 */
void wait_for_vblank(void (*update_func)(void *), void *arg)
{
    status_reg_t status;

    // 1. Wait until the display is *not* in VBlank.
    // This ensures we don't accidentally catch the end of a previous VBlank.
    do
    {
        status = read_status_reg();
        // Check if read_status_reg failed (interface not initialized)
        if (hw_status_reg_ptr == NULL)
            return;
    } while (status.bits.vcount >= VGA_V_PIXELS);

    // 2. Wait until the display *enters* VBlank.
    do
    {
        status = read_status_reg();
        // Check if read_status_reg failed
        if (hw_status_reg_ptr == NULL)
            return;
    } while (status.bits.vcount < VGA_V_PIXELS);

    // 3. Now we are in VBlank. Execute the callback function if provided.
    if (update_func != NULL)
    {
        update_func(arg);
    }
    // VBlank period is relatively long, so updates should complete within it.
}

/**
 * @brief Wrapper function for write_control_reg.
 * This function matches the signature required by wait_for_vblank (void (*)(void*)).
 * It casts the void* argument back to a control_reg_t* and calls write_control_reg.
 *
 * @param arg A void pointer expected to point to a control_reg_t structure.
 */
static void write_control_reg_wrapper(void *arg)
{
    if (arg == NULL)
    {
        fprintf(stderr, "Error: NULL argument passed to write_control_reg_wrapper.\n");
        return;
    }
    control_reg_t *control_ptr = (control_reg_t *)arg;
    write_control_reg(*control_ptr);
}

/**
 * @brief Sets the active background tilemap in the hardware via the control register.
 * Reads the current control register value first to preserve other bits.
 * This function waits for VBlank before writing to the control register.
 *
 * @param map_id The ID of the tilemap to activate (e.g., 0-2, based on hardware design).
 */
void set_active_tilemap(uint8_t map_id)
{
    if (map_id > 2)
    { // Example check, adjust based on actual number of maps
        fprintf(stderr, "Error: Invalid map ID %d.\n", map_id);
        return;
    }

    // Read current control register value to preserve other settings.
    control_reg_t current_control = {0};
    if (hw_control_reg_ptr != NULL)
    { // Read only if interface is initialized
        current_control.value = hw_control_reg_ptr->value;
    }
    else
    {
        fprintf(stderr, "Warning: Cannot read current control register (not initialized). Setting only active_tilemap bit.\n");
    }

    // Modify only the active_tilemap bits.
    current_control.bits.active_tilemap = map_id;

    // Write the updated value during VBlank.
    wait_for_vblank(write_control_reg_wrapper, &current_control);
}

/**
 * @brief Enables or disables the hardware collision detection feature via the control register.
 * Reads the current control register value first to preserve other bits.
 * This function waits for VBlank before writing to the control register.
 *
 * @param enable Set to true to enable hardware collision detection, false to disable.
 */
void set_collision_detection(bool enable)
{
    // Read current control register value to preserve other settings.
    control_reg_t current_control = {0};
    if (hw_control_reg_ptr != NULL)
    { // Read only if interface is initialized
        current_control.value = hw_control_reg_ptr->value;
    }
    else
    {
        fprintf(stderr, "Warning: Cannot read current control register (not initialized). Setting only collision_enable bit.\n");
    }

    // Modify only the collision_enable bit.
    current_control.bits.collision_enable = enable ? 1 : 0;

    // Write the updated value during VBlank.
    wait_for_vblank(write_control_reg_wrapper, &current_control);
}

/**
 * 构造一个 sprite_attr_t 对象（用于角色或地图物体）
 * 包含坐标、图块帧、尺寸、优先级、可见标志、方向翻转标志等
 */
sprite_attr_t make_sprite(float x, float y, int frame_id, int width, int height, int priority, bool alive, bool facing_right)
{
    sprite_attr_t s;
    s.x = (uint16_t)x;                                   // 精灵绘制X位置
    s.y = (uint16_t)y;                                   // 精灵绘制Y位置
    s.frame_id = (uint8_t)frame_id;                      // 图块帧编号
    s.priority = (uint8_t)priority;                      // 显示优先级（小值靠前）
    s.width = (uint8_t)width;                            // 精灵宽度（图块单位）
    s.height = (uint8_t)height;                          // 精灵高度
    s.flags.value = 0;                                   // 清空所有标志位
    s.flags.bits.alive = alive ? 1 : 0;                  // 是否激活（显示）
    s.flags.bits.flip_horizontal = facing_right ? 0 : 1; // 是否水平翻转（左翻转）
    return s;
}
s

    /**
     * @brief Triggers the playback of a specific audio clip by the hardware via the control register.
     * Reads the current control register value first to preserve other bits.
     * This function waits for VBlank before writing to the control register.
     *
     * @param audio_id The ID of the audio clip to play (e.g., 0-15, based on hardware design).
     */
    void
    play_audio(uint8_t audio_id)
{
    // Check if audio_id is within the valid range (assuming 4 bits for audio_select -> 0-15)
    if (audio_id > 15)
    {
        fprintf(stderr, "Error: Invalid audio ID %d.\n", audio_id);
        return;
    }

    // Read current control register value to preserve other settings.
    control_reg_t current_control = {0};
    if (hw_control_reg_ptr != NULL)
    { // Read only if interface is initialized
        current_control.value = hw_control_reg_ptr->value;
    }
    else
    {
        fprintf(stderr, "Warning: Cannot read current control register (not initialized). Setting only audio_select bit.\n");
    }

    // Modify only the audio_select bits.
    current_control.bits.audio_select = audio_id;

    // Write the updated value during VBlank.
    wait_for_vblank(write_control_reg_wrapper, &current_control);
}
