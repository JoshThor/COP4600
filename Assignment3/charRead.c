#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "charRead"
#define BUF_LEN 1024

//Modules

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Read Device Driver");
MODULE_AUTHOR("Group 50");

//extern char Message[BUF_LEN];
extern char *Message_Ptr;
extern int Byte_Count;
extern struct mutex char_mutex;
int count = 0;
int Major = -1;


//Function declaration
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);


//File operations
static struct file_operations fops =
{
    .read = dev_read,
    .open = dev_open,
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
    if(!mutex_trylock(&char_mutex)){
        printk(KERN_ALERT "Char Read: Device in use by another process");
        return -EBUSY;
    }
    count++;
    printk(KERN_INFO "Read device opened %d times\n", count);
    return 0;
}

//Called when making a read request to the driver
static ssize_t dev_read(struct file *fp, char *buffer, size_t length, loff_t *offset)
{
	short bytes_read = 0;

	printk(KERN_INFO "Attempting to read from device file...\n");

	if (Byte_Count == 0)
		return 0;

    	if (*Message_Ptr == 0)
		return 0;

    //puts data into the buffer
	while (length && *Message_Ptr && (Byte_Count > 0)) {

		put_user(*(Message_Ptr++), buffer++);
		length--;
		bytes_read++;
		Byte_Count--;
	}

	return bytes_read;
}

//Called when closing the device driver
static int dev_release(struct inode *inod, struct file *fp)
{
    mutex_unlock(&char_mutex);
    printk(KERN_INFO "Read device closed");
    return 0;
}
