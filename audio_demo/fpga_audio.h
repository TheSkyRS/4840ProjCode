#ifndef _FPGA_AUDIO_H_
#define _FPGA_AUDIO_H_

#include <linux/ioctl.h>
#include <linux/types.h>

/* 寄存器 word 偏移 (×4 = 字节地址) */
#define AUDIO_REG_LEFT   0
#define AUDIO_REG_RIGHT  1
#define AUDIO_REG_CTRL   2   /* bit0 = START, 其余自用 */

/* 用户 ioctl 结构 */
typedef struct {
    __u32 left;
    __u32 right;
    __u32 ctrl;
} fpga_audio_arg_t;

#define FPGA_AUDIO_MAGIC 'a'
#define FPGA_AUDIO_WRITE  _IOW(FPGA_AUDIO_MAGIC, 1, fpga_audio_arg_t)
#define FPGA_AUDIO_READ   _IOR(FPGA_AUDIO_MAGIC, 2, fpga_audio_arg_t)

#endif /* _FPGA_AUDIO_H_ */
