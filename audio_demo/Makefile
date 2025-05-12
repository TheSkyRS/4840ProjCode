obj-m += audio_fpga.o

audio_test: audio_test.c
	$(CC) $^ -o $@

all: audio_fpga.ko audio_test
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f audio_test