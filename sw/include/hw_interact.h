#ifndef HW_INTERACT_H
#define HW_INTERACT_H

#include <stdint.h>

extern int vga_top_fd;

// �� sprite ����д�� FPGA SPRITE ATTR TABLE
void write_sprite(uint8_t index,
                  uint8_t enable, uint8_t flip,
                  uint16_t x, uint16_t y,
                  uint8_t frame);

// �� STATUS_REG ��ȡ��ǰ VGA ɨ��λ��
void read_status(unsigned *col, unsigned *row);

// ���� 32-bit sprite ������
uint32_t make_attr_word(uint8_t enable, uint8_t flip,
                        uint16_t x, uint16_t y,
                        uint8_t frame);

#endif // HW_INTERACT_H

/*
set_map_and_audio(1,0,0)
set_map_and_audio(1,1,0)

set_map_and_audio(1,1,1)

*/