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


static int my_cdev_init(void)
{
	int err;

	err = register_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR),
			NUM_MINORS, MODULE_NAME);
	if (err != 0) {
		pr_info("register_chrdev_region");
		return err;
	}
	printk(KERN_INFO "my_module: Module registered.\n");

	return 0;
}

static void my_cdev_exit(void)
{
	printk(KERN_INFO "my_module: Module unregistered.\n");
	unregister_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), NUM_MINORS);
}

module_init(my_cdev_init);
module_exit(my_cdev_exit);
