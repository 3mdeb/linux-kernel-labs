/*
 * Character device drivers lab
 *
 * All tasks
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/wait.h>

#include "../include/my_cdev.h"

MODULE_DESCRIPTION("SO2 character device");
MODULE_AUTHOR("SO2");
MODULE_LICENSE("GPL");

#define LOG_LEVEL	KERN_DEBUG

#define MY_MAJOR		42
#define MY_MINOR		0
#define NUM_MINORS		1
#define MODULE_NAME		"my_cdev"
#define MESSAGE			"Welcome!\n"
#define IOCTL_MESSAGE		"Hello ioctl!"

#ifndef BUFSIZ
#define BUFSIZ		4096
#endif


struct my_device_data {
	struct cdev cdev;
	char buffer[BUFSIZ];
	size_t size;
	atomic_t access;
};

struct my_device_data devs[NUM_MINORS];

static int my_cdev_open(struct inode *inode, struct file *file)
{
	struct my_device_data *data;

	data = container_of(inode->i_cdev, struct my_device_data, cdev);

	file->private_data = data;

	if (atomic_cmpxchg(&data->access, 0, 1) != 0)
		return -EBUSY;

	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(10);

	printk(KERN_INFO "my_module: Device opened.\n");

	return 0;
}

static int my_cdev_release(struct inode *inode, struct file *file)
{
	struct my_device_data *data = (struct my_device_data *) file->private_data;
	printk(KERN_INFO "my_module: Device released.\n");

	atomic_set(&data->access, 0);
	return 0;
}

static ssize_t my_cdev_read(struct file *file,
		char __user *user_buffer,
		size_t size, loff_t *offset)
{
	struct my_device_data *data =
		(struct my_device_data *) file->private_data;
	size_t to_read;

	to_read = (size > data->size - *offset) ? (data->size - *offset) : size;
	if (copy_to_user(user_buffer, data->buffer + *offset, to_read) != 0)
		return -EFAULT;
	*offset += to_read;

	return to_read;
}

static ssize_t my_cdev_write(struct file *file,
		const char __user *user_buffer,
		size_t size, loff_t *offset)
{
	struct my_device_data *data =
		(struct my_device_data *) file->private_data;


	size = (*offset + size > BUFSIZ) ? (BUFSIZ - *offset) : size;
	if (copy_from_user(data->buffer + *offset, user_buffer, size) != 0)
		return -EFAULT;
	*offset += size;
	data->size = *offset;

	return size;
}

static long my_cdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	switch (cmd) {
	default:
		ret = -EINVAL;
	}

	return ret;
}

static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = my_cdev_open,
	.release = my_cdev_release,
	.read = my_cdev_read,
	.write = my_cdev_write,
};

static int my_cdev_init(void)
{
	int err;
	int i;

	err = register_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR),
			NUM_MINORS, MODULE_NAME);
	if (err != 0) {
		pr_info("register_chrdev_region");
		return err;
	}
	printk(KERN_INFO "my_module: Module registered.\n");

	for (i = 0; i < NUM_MINORS; i++) {
		memcpy(devs[i].buffer, MESSAGE, sizeof(MESSAGE));
		devs[i].size = sizeof(MESSAGE);
		atomic_set(&devs[i].access, 0);
		cdev_init(&devs[i].cdev, &my_fops);
		cdev_add(&devs[i].cdev, MKDEV(MY_MAJOR, i), 1);
	}

	return 0;
}

static void my_cdev_exit(void)
{
	int i;

	for (i = 0; i < NUM_MINORS; i++) {
		cdev_del(&devs[i].cdev);
	}

	printk(KERN_INFO "my_module: Module unregistered.\n");
	unregister_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), NUM_MINORS);
}

module_init(my_cdev_init);
module_exit(my_cdev_exit);
