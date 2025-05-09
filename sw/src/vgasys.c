#include "vgasys.h"
#include "vga_top.h" // ���� ioctl ��ؽṹ��������

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// �ڲ��ļ���������ָ�� /dev/vga_top
static int vgasys_fd = -1;

// ================================
// ��ʼ��������
// ================================

int vgasys_init(const char *dev_name)
{
    vgasys_fd = open(dev_name, O_RDWR);
    if (vgasys_fd < 0)
    {
        perror("[vgasys] Failed to open VGA device");
        return -1;
    }
    return 0;
}

void vgasys_cleanup()
{
    if (vgasys_fd >= 0)
    {
        close(vgasys_fd);
        vgasys_fd = -1;
    }
}

// ================================
// ���� Sprite ������
// ================================

uint32_t make_attr_word(uint8_t enable, uint8_t flip,
                        uint16_t x, uint16_t y,
                        uint8_t frame)
{
    return ((uint32_t)(enable & 1) << 31) |
           ((uint32_t)(flip & 1) << 30) |
           (0u << 27) |                    // reserved
           ((uint32_t)(y & 0x1FF) << 18) | // 9 bits
           ((uint32_t)(x & 0x3FF) << 8) |  // 10 bits
           (frame & 0xFF);                 // 8 bits
}

// ================================
// д�� Sprite ����
// ================================

void write_sprite(uint8_t index, uint8_t enable, uint8_t flip,
                  uint16_t x, uint16_t y, uint8_t frame)
{
    if (vgasys_fd < 0)
    {
        fprintf(stderr, "[vgasys] Device not initialized\n");
        return;
    }

    vga_top_sprite_arg_t arg = {
        .index = index,
        .attr_word = make_attr_word(enable, flip, x, y, frame)};

    if (ioctl(vgasys_fd, VGA_TOP_WRITE_SPRITE, &arg) < 0)
    {
        perror("[vgasys] ioctl(VGA_TOP_WRITE_SPRITE) failed");
    }
}

// ================================
// ���ƼĴ���д��
// ================================

void write_ctrl(uint32_t value)
{
    if (vgasys_fd < 0)
    {
        fprintf(stderr, "[vgasys] Device not initialized\n");
        return;
    }

    vga_top_ctrl_arg_t arg = {.value = value};
    if (ioctl(vgasys_fd, VGA_TOP_WRITE_CTRL, &arg) < 0)
    {
        perror("[vgasys] ioctl(VGA_TOP_WRITE_CTRL) failed");
    }
}

// ================================
// VGA ״̬��ȡ
// ================================

void read_status(unsigned *col, unsigned *row)
{
    if (vgasys_fd < 0)
    {
        fprintf(stderr, "[vgasys] Device not initialized\n");
        *col = *row = 0;
        return;
    }

    vga_top_status_arg_t arg;
    if (ioctl(vgasys_fd, VGA_TOP_READ_STATUS, &arg) < 0)
    {
        perror("[vgasys] ioctl(VGA_TOP_READ_STATUS) failed");
        *col = *row = 0;
        return;
    }

    *col = (arg.value >> 10) & 0x3FF; // [19:10] -> col
    *row = (arg.value >> 0) & 0x3FF;  // [9:0]   -> row
}
