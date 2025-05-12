#ifndef _AUDIO_FPGA_H
#define _AUDIO_FPGA_H

#include <linux/ioctl.h>
#include <linux/types.h>

/* ------- 寄存器 word 偏移 ------- */
#define AUDIO_REG_LEFT   0   /* 左声道 32-bit PCM 样本 */
#define AUDIO_REG_RIGHT  1   /* 右声道 32-bit PCM 样本 */
#define AUDIO_REG_GAIN   2   /* 低 8 位：音量 */

typedef struct {
    __u32 left;
    __u32 right;
    __u8  gain;
} audio_sample_t;

/* ------- ioctl ------- */
#define AUDIO_MAGIC 'a'
#define AUDIO_WRITE_SAMPLE _IOW(AUDIO_MAGIC, 0x01, audio_sample_t)
#define AUDIO_READ_SAMPLE  _IOR(AUDIO_MAGIC, 0x02, audio_sample_t)

#endif /* _AUDIO_FPGA_H */
