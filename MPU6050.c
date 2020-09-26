#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/uaccess.h>

//This a simple driver that interfaces an MPU6050 with a linux based system. Specifically in this  source file I am interfacing 
//the mpu6050 sensor module with a Raspberry Pi 3. The current source file has implementation for the file_operations and driver probing. 
//To test if the driver was functioning correctly I wrote a simple read function that would read the from a register on the sensor module.
struct mpu6050 {
    struct i2c_client *client;
    struct miscdevice mpu6050_misc;
    char name[32];
};
static ssize_t mpu6050_dev_write(struct file *file, const char __user *userbuf, size_t count, loff_t *ppos){
    int ret;
    unsigned long value;
    char buf[16];
    struct mpu6050 *mpu6050;
    mpu6050 = container_of(file -> private_data, struct mpu6050, mpu6050_misc);
    dev_info(&mpu6050->client->dev,  "mpu6050 write method has been entered on %s\n", mpu6050->name);
    dev_info(&mpu6050->client->dev, " %zu characters were written\n", count);
    if(copy_from_user(buf, userbuf, count)) {

        dev_err(&mpu6050->client->dev, "Bad copied value\n");
        return -EFAULT;
    }
    buf[count - 1] = '\0';
    //convert the string to an unsigned long
    ret = kstrtoul(buf, 0, &value);
    if(ret)
        return -EINVAL;
    dev_info(&mpu6050->client->dev, "the value is %lu\n", value);
    ret = i2c_smbus_write_byte(mpu6050->client, value);
    if (ret < 0)
        dev_err(&mpu6050 ->client->dev, "the device is not found\n");
    dev_info(&mpu6050->client->dev, "ioexp_write_file exited on %s\n", mpu6050->name);
    return count;
}
static ssize_t mpu6050_dev_read(struct file *file, char __user *userbuf, size_t count, loff_t *ppos){
    int expval, size;
    char buf[16];
    struct mpu6050 *mpu6050;
    uint8_t whoami = (uint8_t) 0x75;
    uint8_t d[2];
    mpu6050 = container_of(file -> private_data, struct mpu6050, mpu6050_misc);
    //transmit who am i
    expval = i2c_smbus_write_byte(mpu6050 -> client, whoami);
    if(expval< 0)
        pr_info("Error: Could not write to client");
    else
        pr_info("SUCCESS: Wrote to client the whoami register to get");
    expval = i2c_smbus_read_byte(mpu6050 -> client);
    pr_info("%02X\n", expval);
    if(expval < 0)
        return -EFAULT;
    size = sprintf(buf, "%02x", expval);
    buf[size] = '\n';
    if(*ppos == 0){
        if(copy_to_user(userbuf, buf, size + 1))
        {
            pr_info("Failed to return value read to user space\n");
            return -EFAULT;
        }
        *ppos +=1;
        return size +1;
    }
    return 0;
}
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = mpu6050_dev_write,
    .read = mpu6050_dev_read,
};
static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id){
    struct mpu6050 *mpu6050;
    static int counter = 0;
    //Allocate Device Memory
    mpu6050 = devm_kzalloc(&client -> dev, sizeof(struct mpu6050), GFP_KERNEL);
    i2c_set_clientdata(client, mpu6050);
    mpu6050 -> client = client;

    //Set the name of the newly created char device
    sprintf(mpu6050 -> name, "mpu6050%02d", counter++);
    //Intialize and register the misc device
    mpu6050 -> mpu6050_misc.name = mpu6050 -> name;
    mpu6050 -> mpu6050_misc.minor = MISC_DYNAMIC_MINOR;
    mpu6050 -> mpu6050_misc.fops = &fops;
    dev_info(&client->dev, "mpu6050_probe is exited by %s\n", mpu6050->name);
    if(misc_register(&mpu6050 -> mpu6050_misc) != 0)
    {
        pr_info("ERROR: Could not register device: %s", mpu6050 -> name);
        return -1;
    }else
        pr_info("Registered NEW device: %s", mpu6050 -> name);
    return 0;

}

static int mpu6050_remove(struct i2c_client *client){
    struct mpu6050 *mpu6050;
    //Get device structure from bus data
    mpu6050 = i2c_get_clientdata(client);
    dev_info(&client->dev, "mpu6050_remove is entered by %s\n", mpu6050->name);
    misc_deregister(&mpu6050 -> mpu6050_misc);
    return 0;

}
static const struct of_device_id i2c_ids[] =
{
    {.compatible = "arrow,MPU6050", },
    { }
};
MODULE_DEVICE_TABLE(of, i2c_ids);
static const struct i2c_device_id mpu6050_ids[] = {
    {.name = "mpu6050", },
    { }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_ids);
static struct i2c_driver mpu6050_driver = {
    .driver = {
        .name = "mpu6050Driver",
        .owner = THIS_MODULE,
        .of_match_table = i2c_ids,
    },
    .probe = mpu6050_probe,
    .remove = mpu6050_remove,
    .id_table = mpu6050_ids,
};
module_i2c_driver(mpu6050_driver);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Edgar Granados");
MODULE_DESCRIPTION("Kernel Space Driver used to connect an MPU6050 module with a Raspberry Pi");

