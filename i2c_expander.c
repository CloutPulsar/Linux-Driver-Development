#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/uaccess.h>

struct ioexp_dev{
    struct i2c_client *client;
    struct misc_device ioexp_miscdevice;
    char name[8];

};
static ssize_t ioexp_read_file(struct file *file, char __user *userbuf, size_t count, loff_t *ppos)
{
    int expval, size; 
    char buf[3];
    struct ioexp_dev *ioexp;
    ioexp = container_of(file->private_data, struct ioexp_dev, ioexp_miscdevice);
    expval = i2c_smbus_read_byte(ioexp->client);
    if(expval < 0)
        return -EFAULT;
    size = sprintf(buf, "%02x", expval);
    buf[size] = '\n';
    if(*ppos ==0)
    {
        if(copy_to_user(userbuf, buf, size+1)){
            pr_info("Failed to return led_value to user space \n");
            return -EFAULT;
        }
        *ppose +=1;
        return size+1;
    }
    return 0;
}
static ssize_t ioexp_write_file(struct file *file, const char __user *userbuf, size_t count, loff_t *ppos)
{
    int ret;
    unsigned long val;
    char buf[4];
    struct ioexp_dev *ioexp;
    ioexp = container_of(file->private_data, stuct ioexp_dev, ioexp_miscdevice);
    dev_info(&ioexp->client->dev, "ioexp_write_file entered on %s\n", ioexp->name);
    dev_info(&ioexp->client->dev, "we have written %zu characters\n", count);
    if(copy_from_user(buf, userbuf, count)){
        dev_err(&ioexp->client->dev, "Bad copied value\n");
        return -EFAULT;
    }
    buf[count-1] = '\0';
    ret = kstrtoul(buf, 0 , &val);
    if(ret)
        return -EINVAL;
    dev_info(&ioexp->client->dev, "the value is %lu\n", val);
    ret = i2c_smbus_write_byte(ioexp->client, val);
    if (ret < 0)
    {
        dev_err(&ioexp->client->dev, "the device is not found \n");

    }
    dev_info(&ioexp->client->dev, "ioexp_write_file exited on %s\n", ioexp->name);
    return count;
}
static const struct file_operations fops =
{
    .owner = THIS_MODULE,
    .write = ioexp_write_file,
    .read = ioexp_read_file,
};
static int ioexp_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    static int counter = 0;
    struct ioexp_dev *ioexp;
    ioexp = devm_kzalloc(&client->dev, sizeof(struct ioexp_dev), GFP_KERNEL);
    i2c_set_clientdata(client, ioexp);
    ioexp->client = client;

    sprintf(ioexp->name, "ioexp%02d", counter++);
    dev_info(&cient->dev, "ioexp_probe is entered %s\n". ioexp->name);
    ioexp->ioexp_miscdevice.name = ioexp->name;
    ioexp->ioexp_miscdevice.minor = MISC_DYNAMIC_MINOR;
    ioexp->ioexp_miscdevice.fops = &fops;

    //Register the misc device
    return misc_register(&ioexp->ioexp_miscdevice);
    
    dev_info(&client->dev, 
		 "ioexp_probe is exited on %s\n", ioexp->name);

	return 0;
}