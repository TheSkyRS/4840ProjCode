#include "vga_top.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>

int vga_top_fd;

inline uint32_t make_attr_word(uint8_t enable, uint8_t flip,
                               uint16_t x, uint16_t y,
                               uint8_t frame)
{
    return ((uint32_t)(enable & 1) << 31) |
           ((uint32_t)(flip & 1) << 30) |
           (0u << 27) |
           ((uint32_t)(y & 0x1FF) << 18) |
           ((uint32_t)(x & 0x3FF) << 8) |
           (frame & 0xFF);
}

void write_ctrl(uint32_t value)
{
    vga_top_ctrl_arg_t arg = {.value = value};
    if (ioctl(vga_top_fd, VGA_TOP_WRITE_CTRL, &arg))
    {
        perror("ioctl(VGA_TOP_WRITE_CTRL) failed");
        return;
    }
}

void write_sprite(uint8_t index,
                  uint8_t enable, uint8_t flip,
                  uint16_t x, uint16_t y,
                  uint8_t frame)
{
    vga_top_sprite_arg_t arg = {
        .index = index,
        .attr_word = make_attr_word(enable, flip, x, y, frame)};
    if (ioctl(vga_top_fd, VGA_TOP_WRITE_SPRITE, &arg))
    {
        perror("ioctl(VGA_TOP_WRITE_SPRITE) failed");
        return;
    }
}

void read_status(unsigned *col, unsigned *row)
{
    vga_top_status_arg_t arg;
    if (ioctl(vga_top_fd, VGA_TOP_READ_STATUS, &arg))
    {
        perror("ioctl(VGA_TOP_READ_STATUS) failed");
        return;
    }

    *col = (arg.value >> 10) & 0x3FF;
    *row = arg.value & 0x3FF;
}
