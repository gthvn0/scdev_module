#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");
MODULE_DESCRIPTION("A simple char device");
MODULE_VERSION("0.1");

#define DEVICE_NAME "scdev"

struct scdev_buffer_struct {
	DECLARE_KFIFO(read_fifo, unsigned char, 128);
	DECLARE_KFIFO(write_fifo, unsigned char, 128);
};

static int scdev_major;
static struct class *scdev_class;
static struct device *scdev_device;
static struct scdev_buffer_struct scdev_buffer;

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

static ssize_t scdev_read(struct file *filep, char __user *buf, size_t count, loff_t *offset)
{
	unsigned int copied;

	printk(KERN_INFO "scdev: read called with count %lld (0x%llx) and offset %lld (0x%llx)\n",
			(long long)count, (long long)count,
			(long long)*offset, (long long)*offset);

	if (kfifo_is_empty(&scdev_buffer.read_fifo)) {
		printk(KERN_INFO "scdev: read buffer is empty\n");
		return 0;
	}

	kfifo_to_user(&scdev_buffer.read_fifo, buf, count, &copied);
	return copied;
}

static ssize_t scdev_write(struct file *filep, const char __user *buf, size_t count, loff_t *offset)
{
	unsigned int copied;

	printk(KERN_INFO "scdev: write called with count %lld (0x%llx) and offset %lld (0x%llx)\n",
			(long long)count, (long long)count,
			(long long)*offset, (long long)*offset);

	if (kfifo_is_full(&scdev_buffer.write_fifo)) {
		printk(KERN_INFO "scdev: write buffer is full\n");
		return 0;
	}

	kfifo_from_user(&scdev_buffer.read_fifo, buf, count, &copied);

	return copied;
}

static loff_t scdev_llseek(struct file *filep, loff_t offset, int whence)
{
	unsigned int val = (unsigned int) offset;

	printk(KERN_INFO "scdev: llseek called with val %u and whence %d\n", val, whence);

	if (kfifo_is_empty(&scdev_buffer.write_fifo)) {
		printk(KERN_INFO "scdev: nothing has been written before this call\n");
	} else {
		unsigned int lu;
		kfifo_out(&scdev_buffer.write_fifo, (unsigned char *)&lu, sizeof(lu));
		printk(KERN_INFO "scdev: %u read from write_fifo\n", lu);
	}

	kfifo_in(&scdev_buffer.read_fifo, (const unsigned char *)&val, sizeof(val));

	return 0;
}

static struct file_operations fops =
{
	.open = scdev_open,
	.read = scdev_read,
	.write = scdev_write,
	.llseek = scdev_llseek,
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

	INIT_KFIFO(scdev_buffer.read_fifo);
	INIT_KFIFO(scdev_buffer.write_fifo);
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

