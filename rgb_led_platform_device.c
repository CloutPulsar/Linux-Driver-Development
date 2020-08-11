#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

struct led_dev
{
    struct misc_device led_misc_device;
    uint32_t mask;
    const char *name;
    char led_value[8];
};

static const struct file_operations fops =
{
    .read = led_read,
    .write = led_write,
    .owner = THIS_MODULE,
};
static struct platform_driver led_platform_driver =
{
    .probe = led_probe,
    .remove = led_remove,
    .owner = THIS_MODULE,
};
static const struct of_device_ids my_of_ids[]=
{
    {.compatible = "arrows,RGBleds"},
    {}
};
module_int(led_init);
module_exit(led_exit);
