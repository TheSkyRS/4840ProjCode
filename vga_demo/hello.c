
#include <stdio.h>
#include <stdint.h>
#include "vga_top.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define HACTIVE 640
#define VACTIVE 480
#define SPRITE_W 16     /* sprite width  in pixels */
#define SPRITE_H 16     /* sprite height in pixels */

int vga_top_fd;

/* 生成 32-bit sprite 属性字
 *  enable : 0/1
 *  flip   : 0/1
 *  x,y    : 坐标
 *  frame  : 帧 ID
 */
static inline uint32_t make_attr_word(uint8_t enable, uint8_t flip,
									  uint16_t x, uint16_t y,
									  uint8_t frame)
{
	return ((uint32_t)(enable & 1) << 31) |
		   ((uint32_t)(flip & 1) << 30) |
		   (0u << 27) |					   /* reserved */
		   ((uint32_t)(y & 0x1FF) << 18) | /* 9 bits  */
		   ((uint32_t)(x & 0x3FF) << 8) |  /* 10 bits */
		   (frame & 0xFF);
}

static void write_ctrl(uint32_t value)
{
    vga_top_ctrl_arg_t arg = { .value = value };
    if (ioctl(vga_top_fd, VGA_TOP_WRITE_CTRL, &arg)) {
      perror("ioctl(VGA_TOP_WRITE_CTRL) failed");
      return;
  }
}

/* Write one sprite attribute entry */
static void write_sprite(uint8_t index,
						 uint8_t enable, uint8_t flip,
						 uint16_t x, uint16_t y,
						 uint8_t frame)
{
	vga_top_sprite_arg_t arg = {
		.index = index,
		.attr_word = make_attr_word(enable, flip, x, y, frame)};
	if (ioctl(vga_top_fd, VGA_TOP_WRITE_SPRITE, &arg)) {
		perror("ioctl(VGA_TOP_WRITE_SPRITE) failed");
		return;
	}
}

/* ---------- 读 STATUS_REG，得到 hcount/vcount ---------- */
static void read_status(unsigned *col, unsigned *row)
{
    vga_top_status_arg_t arg;
    if (ioctl(vga_top_fd, VGA_TOP_READ_STATUS, &arg)) {
        perror("ioctl(VGA_TOP_READ_STATUS) failed"); 
		return;
    }

    *col = (arg.value >> 10) & 0x3FF;   /* [19:10] → 0-639 */
    *row =  arg.value        & 0x3FF;   /* [9:0]   → 0-479 */
}

int main()
{
	static const char devname[] = "/dev/vga_top";

	if ((vga_top_fd = open(devname, O_RDWR)) == -1)
	{
		fprintf(stderr, "could not open %s\n", devname);
		return -1;
	}

	write_ctrl(0x00000000);
	
	/* 初始位置/速度/方向 */
	int  x  = 100, y  = 100;
	int  dx =  7,  dy =  4;
	uint8_t flip = 0;
	const uint8_t SPRITE_ID = 0;
	const uint8_t FRAME_ID  = 0;

	/* === 初始状态 === */
    int  x = 100, y = 100;
    int  dx = 7,  dy = 4;
    uint8_t flip = 0;

    const uint8_t SPRITE_ID = 0;
    const uint8_t FRAME_ID  = 0;

    /* 第一次写入 */
    write_sprite(SPRITE_ID, 1, flip, x, y, FRAME_ID);

    int  have_pending = 0;

    unsigned col, row;

    printf("sprite_demo: running – Ctrl-C to exit\n");
    while (1) {
        read_status(&col, &row);

        /* -------- 1) 可见区：只计算，不写硬件 -------- */
        if (row < VACTIVE) {
            if (!have_pending) {          /* 每帧只算一次 */
                /* 计算下一步位置 */
                x += dx;  y += dy;

                if (x <= 0 || x >= HACTIVE - SPRITE_W) {
                    dx = -dx;  x += dx;
                    flip ^= 1;        /* 左右反弹时翻转 */
                }
                if (y <= 0 || y >= VACTIVE - SPRITE_H) {
                    dy = -dy;  y += dy;
                }
                have_pending = 1;         /* 标记本帧已算 */
            }
        }
        /* -------- 2) 消隐区：把计算好的值写回硬件 ---- */
        else { /* row >= 480 */
            if (have_pending) {
                write_sprite(SPRITE_ID, 1,
                             flip, x, y, FRAME_ID);
                have_pending = 0;
            }
        }
    }

    close(vga_top_fd);
    return 0;
}
