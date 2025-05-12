/*
 * fpga_audio – misc driver for Avalon-MM audio core
 */

 #include <linux/module.h>
 #include <linux/platform_device.h>
 #include <linux/miscdevice.h>
 #include <linux/io.h>
 #include <linux/of_address.h>
 #include <stdint.h>  
 #include "audio_fpga.h"
 
 #define DRIVER_NAME "fpga_audio"
 #define REG(o)      (dev.virtbase + ((o) << 2))
 
 struct {
     struct resource res;
     void __iomem   *virtbase;
     audio_sample_t cache;
 } dev;
 
 /* ---------- ioctl ---------- */
 static long audio_ioctl(struct file *f,
                         unsigned int cmd, unsigned long arg)
 {
     audio_sample_t s;
 
     switch (cmd) {
     case AUDIO_WRITE_SAMPLE:
         if (copy_from_user(&s, (void __user *)arg, sizeof s))
             return -EFAULT;
         iowrite32(s.left,  REG(AUDIO_REG_LEFT));
         iowrite32(s.right, REG(AUDIO_REG_RIGHT));
         iowrite8 (s.gain,  REG(AUDIO_REG_GAIN));
         dev.cache = s;
         break;
     case AUDIO_READ_SAMPLE:
         if (copy_to_user((void __user *)arg, &dev.cache, sizeof s))
             return -EFAULT;
         break;
     default:
         return -EINVAL;
     }
     return 0;
 }
 
 static const struct file_operations audio_fops = {
     .owner          = THIS_MODULE,
     .unlocked_ioctl = audio_ioctl,
 };
 
 static struct miscdevice audio_misc = {
     .minor  = MISC_DYNAMIC_MINOR,
     .name   = DRIVER_NAME,
     .fops   = &audio_fops,
 };
 
 /* ---------- probe / remove ---------- */
 static int audio_probe(struct platform_device *pdev)
 {
     int ret;
 
     ret = misc_register(&audio_misc);
     if (ret) return ret;
 
     if (of_address_to_resource(pdev->dev.of_node, 0, &dev.res))
         return -ENOENT;
     if (!request_mem_region(dev.res.start, resource_size(&dev.res), DRIVER_NAME))
         return -EBUSY;
 
     dev.virtbase = of_iomap(pdev->dev.of_node, 0);
     if (!dev.virtbase) return -ENOMEM;
 
     pr_info(DRIVER_NAME ": mapped at %pa\n", &dev.res.start);
     return 0;
 }
 
 static int audio_remove(struct platform_device *pdev)
 {
     iounmap(dev.virtbase);
     release_mem_region(dev.res.start, resource_size(&dev.res));
     misc_deregister(&audio_misc);
     return 0;
 }
 
 static const struct of_device_id audio_of_ids[] = {
     { .compatible = "csee4840,fpga_audio-1.0" }, {}
 };
 MODULE_DEVICE_TABLE(of, audio_of_ids);
 
 static struct platform_driver audio_drv = {
     .driver = {
         .name = DRIVER_NAME,
         .of_match_table = audio_of_ids,
     },
     .remove = audio_remove,
 };
 module_platform_driver_probe(audio_drv, audio_probe);
 
 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("4840-Audio");
 MODULE_DESCRIPTION("FPGA Audio driver");
 