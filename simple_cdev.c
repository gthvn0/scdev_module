#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");
MODULE_DESCRIPTION("A simple char device");
MODULE_VERSION("0.1");

#define DEVICE_NAME "scdev"

static int scdev_major;
static struct class *scdev_class;
static struct device *scdev_device;

static int scdev_open(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "scdev: opened\n");
	return 0;
}

static int scdev_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "scdev: released\n");
	return 0;
}

static ssize_t scdev_read(struct file *filep, char *buf, size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t scdev_write(struct file *filep, const char *buf, size_t count, loff_t *ppos)
{
	return 0;
}

static struct file_operations fops =
{
	.open = scdev_open,
	.read = scdev_read,
	.write = scdev_write,
	.release = scdev_release,
};

static int __init simple_cdev_init(void)
{
	scdev_major = register_chrdev(0, DEVICE_NAME, &fops);
	if (scdev_major < 0) {
		printk(KERN_ALERT "simple_char: failed to register a major number\n");
		return scdev_major;
	}
	printk(KERN_INFO "Simple char device got major %d\n", scdev_major);

	scdev_class = class_create(THIS_MODULE, "scdev");
	if (IS_ERR(scdev_class)) {
		unregister_chrdev(scdev_major, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(scdev_class);
	}
	printk(KERN_INFO "Simple char device class created\n");

	scdev_device = device_create(scdev_class, NULL, MKDEV(scdev_major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(scdev_device)){
		class_destroy(scdev_class);
		unregister_chrdev(scdev_major, DEVICE_NAME);
		printk(KERN_ALERT "Simple char device: Failed to create the device\n");
		return PTR_ERR(scdev_device);
	}
	printk(KERN_INFO "Simple char device has been created\n");

	return 0;
}

static void __exit simple_cdev_exit(void)
{
	device_destroy(scdev_class, MKDEV(scdev_major, 0));
	class_destroy(scdev_class);
	unregister_chrdev(scdev_major, DEVICE_NAME);
	printk(KERN_INFO "Simple char device exited\n");
}

module_init(simple_cdev_init);
module_exit(simple_cdev_exit);

