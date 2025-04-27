#ifndef HW_INTERFACE_H
#define HW_INTERFACE_H

#include <stdint.h>
#include <stdbool.h> // Include for bool type

/**
 * @file hw_interface.h
 * @brief Defines the hardware interface for the FPGA game.
 * 
 * This file contains definitions for memory-mapped I/O (MMIO) registers,
 * data structures for communication with the FPGA, and function prototypes
 * for interacting with the hardware components (VGA, sprites, audio, collision).
 */

// --- Placeholder Base Addresses --- 
// IMPORTANT: These addresses must be confirmed based on the actual hardware 
// implementation and the Avalon Bus memory map configuration in Quartus.
#define FPGA_MMIO_BASE      0xFF200000 // Example base address for Lightweight HPS-to-FPGA bridge

// --- Register Offsets from FPGA_MMIO_BASE --- 
#define CONTROL_REG_OFFSET  0x00 ///< Offset for the Control Register.
#define STATUS_REG_OFFSET   0x04 ///< Offset for the Status Register.
#define SPRITE_ATTR_TABLE_OFFSET 0x100 ///< Example offset for the Sprite Attribute Table in BRAM. Needs confirmation.

// --- Register Structures and Bit Definitions --- 

/**
 * @brief Structure representing the Control Register (Write Only by Software).
 * Allows software to control various hardware features.
 */
typedef union {
    uint32_t value; ///< Access the full 32-bit register value.
    struct {
        uint32_t active_tilemap : 2;  ///< Bits 1:0 - Selects the active background tilemap (0-2).
        uint32_t collision_enable : 1; ///< Bit 2   - Enables/disables the hardware collision detector (1=enabled, 0=disabled).
        uint32_t audio_select : 4;    ///< Bits 6:3 - Selects the audio clip to be played by the hardware.
        uint32_t reserved : 25;       ///< Bits 31:7 - Reserved for future use.
    } bits; ///< Access individual bit-fields.
} control_reg_t;

/**
 * @brief Structure representing the Status Register (Read Only by Software).
 * Provides status information from the hardware to the software.
 */
typedef union {
    uint32_t value; ///< Access the full 32-bit register value.
    struct {
        uint32_t vcount : 10;         ///< Bits 9:0   - Current VGA vertical scanline count (useful for VBlank synchronization).
        uint32_t reserved1 : 10;      ///< Bits 19:10 - Reserved.
        uint32_t collision_type : 4;  ///< Bits 23:20 - Result from the hardware collision detector (encoding is hardware-specific).
        uint32_t reserved2 : 8;       ///< Bits 31:24 - Reserved.
    } bits; ///< Access individual bit-fields.
} status_reg_t;

/**
 * @brief Structure representing a single entry in the Sprite Attribute Table.
 * Defines the properties of a sprite to be rendered by the hardware.
 * Located in FPGA BRAM, accessed via MMIO.
 */
typedef struct {
    uint16_t x;         ///< X coordinate of the sprite's top-left corner (in pixels).
    uint16_t y;         ///< Y coordinate of the sprite's top-left corner (in pixels).
    uint8_t frame_id;   ///< Index into the sprite pattern ROM to select the animation frame.
    uint8_t priority;   ///< Z-ordering priority (higher value means sprite is drawn on top).
    uint8_t width;      ///< Width of the sprite (for rendering and hardware collision).
    uint8_t height;     ///< Height of the sprite (for rendering and hardware collision).
    /**
     * @brief Flags associated with the sprite (e.g., status, state).
     * Using a union for easy access to the full 16 bits or individual flags.
     * Padded to 16 bits to make the total struct size 8 bytes.
     */
    union {
        uint16_t value; ///< Access the full 16-bit flags value.
        struct {
            uint16_t alive : 1;     ///< Example Flag: Bit 0 - Indicates if the sprite is active/visible.
            uint16_t jumping : 1;   ///< Example Flag: Bit 1 - Indicates if the character sprite is jumping.
            uint16_t on_switch : 1; ///< Example Flag: Bit 2 - Indicates if the character sprite is on a switch.
            // Add more flags as needed based on game logic requirements.
            uint16_t reserved : 13; ///< Remaining bits reserved for future flags.
        } bits; ///< Access individual flag bits.
    } flags;
} sprite_attr_t; // Total size: 2+2+1+1+1+1 = 8 Bytes

#define MAX_SPRITES 16 ///< Maximum number of sprites supported by the hardware (as per design doc).

// --- Volatile Pointers for MMIO Access --- 
// These pointers are declared in hw_interface.c and initialized after memory mapping.
// Using 'volatile' is crucial to prevent compiler optimizations that could 
// interfere with direct hardware register access.

// --- Constants --- 
#define VGA_V_PIXELS 480 ///< Vertical resolution of the VGA display (640x480).

// --- Function Prototypes --- 

/**
 * @brief Initializes the hardware interface.
 * This function opens /dev/mem and maps the FPGA's MMIO region into the 
 * process's address space.
 * @return 0 on success, -1 on failure (e.g., cannot open /dev/mem, mmap fails).
 */
int hw_interface_init();

/**
 * @brief Cleans up the hardware interface.
 * Unmaps the MMIO memory region and closes the /dev/mem file descriptor.
 */
void hw_interface_cleanup();

/**
 * @brief Reads the current value of the Status Register from the hardware.
 * @return The current status register value (status_reg_t).
 */
status_reg_t read_status_reg();

/**
 * @brief Writes a value to the Control Register in the hardware.
 * @param control The control_reg_t value to write.
 */
void write_control_reg(control_reg_t control);

/**
 * @brief Updates the attributes of a single sprite in the hardware's Sprite Attribute Table.
 * @param sprite_index The index of the sprite to update (0 to MAX_SPRITES-1).
 * @param sprite The sprite_attr_t data containing the new attributes.
 * @return 0 on success, -1 on failure (e.g., index out of bounds, interface not initialized).
 */
int update_sprite(uint8_t sprite_index, sprite_attr_t sprite);

/**
 * @brief Updates the attributes of multiple sprites in the hardware's Sprite Attribute Table.
 * Typically used to update all active sprites in one go.
 * @param sprites Pointer to an array of sprite_attr_t data.
 * @param count The number of sprites in the array to update (up to MAX_SPRITES).
 * @return 0 on success, -1 on failure (e.g., interface not initialized).
 */
int update_sprite_table(sprite_attr_t* sprites, uint8_t count);

/**
 * @brief Waits until the VGA controller enters the vertical blanking interval (VBlank).
 * Then, optionally executes a provided callback function during VBlank.
 * This is crucial for updating hardware registers (like sprite attributes or control registers)
 * without causing visual artifacts (tearing).
 * @param update_func A pointer to a function to be called during VBlank. Can be NULL.
 * @param arg An argument to be passed to the update_func callback.
 */
void wait_for_vblank(void (*update_func)(void*), void* arg);

/**
 * @brief Sets the active background tilemap in the hardware via the control register.
 * This function waits for VBlank before writing to the control register.
 * @param map_id The ID of the tilemap to activate (as defined by hardware, e.g., 0-2).
 */
void set_active_tilemap(uint8_t map_id);

/**
 * @brief Enables or disables the hardware collision detection feature via the control register.
 * This function waits for VBlank before writing to the control register.
 * @param enable Set to true to enable hardware collision detection, false to disable.
 */
void set_collision_detection(bool enable);

/**
 * @brief Triggers the playback of a specific audio clip by the hardware via the control register.
 * This function waits for VBlank before writing to the control register.
 * @param audio_id The ID of the audio clip to play (as defined by hardware, e.g., 0-15).
 */
void play_audio(uint8_t audio_id);


#endif // HW_INTERFACE_H

