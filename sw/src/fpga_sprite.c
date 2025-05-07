/**
 * @file fpga_sprite.c
 * @brief FPGA Sprite Engine Kernel Driver
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include "../include/fpga_sprite.h"

#define DRIVER_NAME "fpga_sprite"

/* Device register offsets */
#define CONTROL_REG_OFFSET     0x00  // Control register offset
#define STATUS_REG_OFFSET      0x04  // Status register offset
#define SPRITE_ATTR_TABLE_OFFSET 0x100  // Sprite attribute table offset

#define MAX_SPRITES 64  // Maximum number of supported sprites
#define VGA_V_PIXELS 480  // Vertical pixel count for VBlank detection

/**
 * Device information structure
 */
struct fpga_sprite_dev {
    struct resource res;          /* Resource: registers */
    void __iomem *virtbase;       /* Register memory-mapped location */
    fpga_control_reg_t control;   /* Current control register value */
};

static struct fpga_sprite_dev dev;

/**
 * Write to control register
 */
static void write_control_reg(fpga_control_reg_t *control)
{
    /* Write to control register */
    iowrite32(control->value, dev.virtbase + CONTROL_REG_OFFSET);
    dev.control = *control;
}

/**
 * Read status register
 */
static void read_status_reg(fpga_status_reg_t *status)
{
    /* Read status register */
    status->value = ioread32(dev.virtbase + STATUS_REG_OFFSET);
}

/**
 * Write a single sprite attribute
 */
static void write_sprite(unsigned int index, fpga_sprite_attr_t *sprite)
{
    if (index < MAX_SPRITES) {
        iowrite32(sprite->value, dev.virtbase + SPRITE_ATTR_TABLE_OFFSET + (index * sizeof(uint32_t)));
    }
}

/**
 * Read a single sprite attribute
 */
static void read_sprite(unsigned int index, fpga_sprite_attr_t *sprite)
{
    if (index < MAX_SPRITES) {
        sprite->value = ioread32(dev.virtbase + SPRITE_ATTR_TABLE_OFFSET + (index * sizeof(uint32_t)));
    } else {
        sprite->value = 0;
    }
}

/**
 * Wait for VBlank
 */
static void wait_for_vblank(void)
{
    fpga_status_reg_t status;

    /* 1. Wait until not in VBlank state */
    do {
        read_status_reg(&status);
    } while (status.bits.vcount >= VGA_V_PIXELS);

    /* 2. Wait until entering VBlank state */
    do {
        read_status_reg(&status);
    } while (status.bits.vcount < VGA_V_PIXELS);
}

/**
 * Set the active tilemap
 */
static void set_active_tilemap(unsigned int map_id)
{
    if (map_id > 15) {
        map_id = 15;  // Limit to valid range
    }

    fpga_control_reg_t control = dev.control;
    control.bits.active_tilemap = map_id;
    write_control_reg(&control);
}

/**
 * Enable/disable collision detection
 */
static void set_collision_detection(unsigned int enable)
{
    fpga_control_reg_t control = dev.control;
    control.bits.enable_collision = enable ? 1 : 0;
    write_control_reg(&control);
}

/**
 * Play audio
 */
static void play_audio(unsigned int audio_id)
{
    fpga_control_reg_t control = dev.control;
    control.bits.audio_id = audio_id & 0xFF;  // Ensure only 8 bits are used
    write_control_reg(&control);
}

/**
 * Handle ioctl calls from user space
 */
static long fpga_sprite_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    fpga_sprite_arg_t karg;
    unsigned int i;

    switch (cmd) {
    case FPGA_SPRITE_READ_STATUS:
        read_status_reg(&karg.status);
        if (copy_to_user((fpga_sprite_arg_t *)arg, &karg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        break;

    case FPGA_SPRITE_WRITE_CONTROL:
        if (copy_from_user(&karg, (fpga_sprite_arg_t *)arg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        write_control_reg(&karg.control);
        break;

    case FPGA_SPRITE_READ_SPRITE:
        if (copy_from_user(&karg, (fpga_sprite_arg_t *)arg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        if (karg.sprite_update.index >= MAX_SPRITES)
            return -EINVAL;
        read_sprite(karg.sprite_update.index, &karg.sprite_update.attr);
        if (copy_to_user((fpga_sprite_arg_t *)arg, &karg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        break;

    case FPGA_SPRITE_WRITE_SPRITE:
        if (copy_from_user(&karg, (fpga_sprite_arg_t *)arg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        if (karg.sprite_update.index >= MAX_SPRITES)
            return -EINVAL;
        write_sprite(karg.sprite_update.index, &karg.sprite_update.attr);
        break;

    case FPGA_SPRITE_UPDATE_SPRITE_TABLE:
        if (copy_from_user(&karg, (fpga_sprite_arg_t *)arg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        
        /* Limit quantity to valid range */
        if (karg.count > MAX_SPRITES)
            karg.count = MAX_SPRITES;
            
        /* Update sprite table */
        for (i = 0; i < karg.count; i++) {
            write_sprite(i, &karg.sprites[i]);
        }
        break;

    case FPGA_SPRITE_SET_TILEMAP:
        if (copy_from_user(&karg, (fpga_sprite_arg_t *)arg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        set_active_tilemap(karg.tilemap_id);
        break;

    case FPGA_SPRITE_SET_COLLISION:
        if (copy_from_user(&karg, (fpga_sprite_arg_t *)arg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        set_collision_detection(karg.enable_collision);
        break;

    case FPGA_SPRITE_PLAY_AUDIO:
        if (copy_from_user(&karg, (fpga_sprite_arg_t *)arg, sizeof(fpga_sprite_arg_t)))
            return -EACCES;
        play_audio(karg.audio_id);
        break;

    case FPGA_SPRITE_WAIT_VBLANK:
        wait_for_vblank();
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

/* Device file operations structure */
static const struct file_operations fpga_sprite_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = fpga_sprite_ioctl,
};

/* "misc" framework device info */
static struct miscdevice fpga_sprite_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DRIVER_NAME,
    .fops  = &fpga_sprite_fops,
};

/**
 * Initialization code: get resources (registers)
 */
static int __init fpga_sprite_probe(struct platform_device *pdev)
{
    int ret;

    /* Register as misc device: creates /dev/fpga_sprite */
    ret = misc_register(&fpga_sprite_misc_device);
    if (ret)
        return ret;

    /* Get register address from device tree */
    ret = of_address_to_resource(pdev->dev.of_node, 0, &dev.res);
    if (ret) {
        ret = -ENOENT;
        goto out_deregister;
    }

    /* Ensure these registers can be used */
    if (request_mem_region(dev.res.start, resource_size(&dev.res), DRIVER_NAME) == NULL) {
        ret = -EBUSY;
        goto out_deregister;
    }

    /* Set up access to registers */
    dev.virtbase = of_iomap(pdev->dev.of_node, 0);
    if (dev.virtbase == NULL) {
        ret = -ENOMEM;
        goto out_release_mem_region;
    }

    /* Initialize control register default values */
    dev.control.value = 0;
    dev.control.bits.enable_display = 1;
    dev.control.bits.enable_sprites = 1;
    write_control_reg(&dev.control);

    printk(KERN_INFO DRIVER_NAME ": Initialization successful, register base address 0x%lx\n", 
           (unsigned long)dev.res.start);
    return 0;

out_release_mem_region:
    release_mem_region(dev.res.start, resource_size(&dev.res));
out_deregister:
    misc_deregister(&fpga_sprite_misc_device);
    return ret;
}

/**
 * Cleanup code: release resources
 */
static int fpga_sprite_remove(struct platform_device *pdev)
{
    iounmap(dev.virtbase);
    release_mem_region(dev.res.start, resource_size(&dev.res));
    misc_deregister(&fpga_sprite_misc_device);
    return 0;
}

/* "compatible" string to search for in device tree */
#ifdef CONFIG_OF
static const struct of_device_id fpga_sprite_of_match[] = {
    { .compatible = "csee4840,fpga-sprite-1.0" },
    {},
};
MODULE_DEVICE_TABLE(of, fpga_sprite_of_match);
#endif

/* Info for registering as a "platform" driver */
static struct platform_driver fpga_sprite_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(fpga_sprite_of_match),
    },
    .remove = __exit_p(fpga_sprite_remove),
};

/**
 * Called when module is loaded: setup
 */
static int __init fpga_sprite_init(void)
{
    pr_info(DRIVER_NAME ": Initializing\n");
    return platform_driver_probe(&fpga_sprite_driver, fpga_sprite_probe);
}

/**
 * Called when module is unloaded: release resources
 */
static void __exit fpga_sprite_exit(void)
{
    platform_driver_unregister(&fpga_sprite_driver);
    pr_info(DRIVER_NAME ": Exiting\n");
}

module_init(fpga_sprite_init);
module_exit(fpga_sprite_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yifan Mao");
MODULE_DESCRIPTION("FPGA Sprite Engine Driver"); 