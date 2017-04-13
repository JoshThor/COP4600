#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "charWrite"
#define BUF_LEN 1024

//Modules

static DEFINE_MUTEX(char_mutex);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Write Device Driver");
MODULE_AUTHOR("Group 50");

static char Message[BUF_LEN];
char *Message_Ptr;
int count = 0;
int Major = -1;
int Byte_Count = 0;

//Function declaration
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


//File operations
static struct file_operations fops =
{
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

	mutex_init(&char_mutex);

    return 0;
}

//Unload driver
void cleanup_module(void)
{
	/*
	 * Unregister the device
	 */
    mutex_destroy(&char_mutex);
	unregister_chrdev(Major, DEVICE_NAME);
}

//called when opening the device driver
static int dev_open(struct inode *inod, struct file * fp)
{
    if(!mutex_trylock(&char_mutex)){
        printk(KERN_ALERT "CharWrite: Device in use by another process");
        return -EBUSY;
    }
    count++;
    printk(KERN_INFO "Write device opened %d times\n", count);
    return 0;
}

//Called when making a write request to the driver
static ssize_t dev_write(struct file *fp, const char *buffer, size_t length, loff_t *off)
{
    int i = 0;

	printk(KERN_INFO "Attempting to write to device file...\n");

	while (i < length && Byte_Count < BUF_LEN)
	{
		get_user(Message[Byte_Count], buffer + i);
		Byte_Count++;
		i++;
	}

	Message_Ptr = Message;

	return i;

}

//Called when closing the device driver
static int dev_release(struct inode *inod, struct file *fp)
{
    mutex_unlock(&char_mutex);
    printk(KERN_INFO "Write device closed");
    return 0;
}

EXPORT_SYMBOL(Message_Ptr);
EXPORT_SYMBOL(Byte_Count);
EXPORT_SYMBOL(char_mutex);
