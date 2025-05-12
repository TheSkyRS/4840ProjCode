obj-m += fpga_audio.o

CFLAGS_audio_play.o = -Wall

audio_play: audio_play.c fpga_audio.h
	$(CC) $< -o $@

all: fpga_audio.ko audio_play

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f audio_play
