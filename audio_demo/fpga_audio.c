/*
 * fpga_audio.c  –  Minimal misc driver for Avalon-MM audio core
 * 模板完全来自 vga_ball.c
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of_address.h>
#include <linux/io.h>
#include "fpga_audio.h"

#define DRIVER_NAME "fpga_audio"
#define REG(o)      (dev.virtbase + ((o) << 2))

struct {
	struct resource  res;
	void __iomem    *virtbase;
	fpga_audio_arg_t cache;
} dev;

/* ---------- ioctl ---------- */
static long fpga_audio_ioctl(struct file *f,
		unsigned int cmd, unsigned long arg)
{
	fpga_audio_arg_t a;

	switch (cmd) {
	case FPGA_AUDIO_WRITE:
		if (copy_from_user(&a, (void __user *)arg, sizeof a))
			return -EFAULT;
		iowrite32(a.left , REG(AUDIO_REG_LEFT ));
		iowrite32(a.right, REG(AUDIO_REG_RIGHT));
		iowrite32(a.ctrl , REG(AUDIO_REG_CTRL ));
		dev.cache = a;
		break;
	case FPGA_AUDIO_READ:
		if (copy_to_user((void __user *)arg, &dev.cache, sizeof a))
			return -EFAULT;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static const struct file_operations fpga_audio_fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = fpga_audio_ioctl,
};

static struct miscdevice fpga_audio_misc = {
	.minor  = MISC_DYNAMIC_MINOR,
	.name   = DRIVER_NAME,
	.fops   = &fpga_audio_fops,
};

/* ---------- probe/remove ---------- */
static int fpga_audio_probe(struct platform_device *pdev)
{
	int ret;

	ret = misc_register(&fpga_audio_misc);
	if (ret) return ret;

	if (of_address_to_resource(pdev->dev.of_node, 0, &dev.res))
		return -ENOENT;

	if (!request_mem_region(dev.res.start,
				resource_size(&dev.res), DRIVER_NAME))
		return -EBUSY;

	dev.virtbase = of_iomap(pdev->dev.of_node, 0);
	if (!dev.virtbase) return -ENOMEM;

	pr_info("%s: mapped at %pa\n", DRIVER_NAME, &dev.res.start);
	return 0;
}

static int fpga_audio_remove(struct platform_device *pdev)
{
	iounmap(dev.virtbase);
	release_mem_region(dev.res.start, resource_size(&dev.res));
	misc_deregister(&fpga_audio_misc);
	return 0;
}

static const struct of_device_id fpga_audio_of[] = {
	{ .compatible = "csee4840,fpga_audio-1.0" },
	{}
};
MODULE_DEVICE_TABLE(of, fpga_audio_of);

static struct platform_driver fpga_audio_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = fpga_audio_of,
	},
	.remove = fpga_audio_remove,
};

module_platform_driver_probe(fpga_audio_driver, fpga_audio_probe);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("4840-Audio");
MODULE_DESCRIPTION("Avalon-MM Audio driver");
