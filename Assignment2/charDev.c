
#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "charDev"
#define BUF_LEN 1000

//Modules

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Device Driver");
MODULE_AUTHOR("Group 50");

static char Message[BUF_LEN];
static char *Message_Ptr;
static int count = 0;
int Major = -1;

//Function declaration
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


//File operations
static struct file_operations fops =
{

    .read = dev_read,
    .open = dev_open,
    .write = dev_write,
    .release = dev_release
};

//Initialize driver and get a major number
int init_module(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &fops); //register device & get back a major device number

	if (Major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	  return Major;
	}

	printk(KERN_INFO "I was assigned major number %d\n", Major);
	printk(KERN_INFO "Command to create device file: 'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);

    return 0;
}

//Unload driver
void cleanup_module(void)
{
	/*
	 * Unregister the device
	 */
	unregister_chrdev(Major, DEVICE_NAME);
}

//called when opening the device driver
static int dev_open(struct inode *inod, struct file * fp)
{
    count++;
    printk(KERN_INFO "Device opened %d times\n", count);
    return 0;
}

//Called when making a read request to the driver
static ssize_t dev_read(struct file *fp, char *buffer, size_t length, loff_t *offset)
{

    short bytes_read = 0;

    if (*Message_Ptr == 0)
		return 0;

    //puts data into the buffer
	while (length && *Message_Ptr) {

		put_user(*(Message_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

	return bytes_read;
}

//Called when making a write request to the driver
static ssize_t dev_write(struct file *fp, const char *buff, size_t length, loff_t *off)
{
    int i;

    //gets data from user input
	for (i = 0; i < length && i < BUF_LEN; i++)
		get_user(Message[i], buffer + i);

	Message_Ptr = Message;

	return i;
}

//Called when closing the device driver
static int dev_release(struct inode *inod, struct file *fp)
{
    printk(KERN_INFO "Device closed");
    return 0;
}
