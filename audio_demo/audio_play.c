/* 简单写一遍 ROM 中的数据到寄存器，或者产生 ramp */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include "fpga_audio.h"

int main(void)
{
	const char devname[] = "/dev/fpga_audio";
	int fd = open(devname, O_RDWR);
	if (fd < 0) { perror(devname); return 1; }

	fpga_audio_arg_t a = {0};
	a.ctrl = 1;                /* bit0=START */

	for (uint32_t i = 0; i < 0x2000; ++i) {
		a.left  = i << 16;     /* 16-bit ramp → 左声道 */
		a.right = (~i) << 16;  /* 右声道反向 */
		if (ioctl(fd, FPGA_AUDIO_WRITE, &a) < 0) {
			perror("ioctl"); break;
		}
		usleep(125);           /* ≈8 kHz */
	}
	close(fd);
	return 0;
}
