#ifndef HW_INTERACT_H
#define HW_INTERACT_H

#include <stdint.h>

extern int vga_top_fd;

// 将 sprite 属性写入 FPGA SPRITE ATTR TABLE
void write_sprite(uint8_t index,
                  uint8_t enable, uint8_t flip,
                  uint16_t x, uint16_t y,
                  uint8_t frame);

// 从 STATUS_REG 读取当前 VGA 扫描位置
void read_status(unsigned *col, unsigned *row);

// 构造 32-bit sprite 属性字
uint32_t make_attr_word(uint8_t enable, uint8_t flip,
                        uint16_t x, uint16_t y,
                        uint8_t frame);

#endif // HW_INTERACT_H

/*
set_map_and_audio(1,0,0)
set_map_and_audio(1,1,0)

set_map_and_audio(1,1,1)

*/