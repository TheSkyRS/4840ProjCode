#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>  
#include "audio_fpga.h"

int main(void)
{
    int fd = open("/dev/fpga_audio", O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    audio_sample_t s = {0};
    s.gain = 0x80;              /* 中等音量 */

    /* 播放 ROM：驱动侧看到写入就会启动 FIFO */
    for (uint32_t i = 0; i < 0x4000; ++i) {
        s.left  = i << 20;      /* 简单 ramp 作示范 */
        s.right = ~s.left;
        if (ioctl(fd, AUDIO_WRITE_SAMPLE, &s) < 0) {
            perror("ioctl"); break;
        }
        usleep(125);            /* ≈8 kHz */
    }
    close(fd);
    return 0;
}
