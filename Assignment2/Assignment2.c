
#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "charDev"
#define BUFF_LEN 1000

//Modules

MODULE_LICENSE("GPL");
MUDULE_DESCRIPTION("Character Device Driver");
MODULE_AUTHOR("Group 50");

static char msg[BUFF_LEN];
static short readIndex = 0;
static int count = 0;
int major = -1;

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
    major = register_chrdev(0, DEVICE_NAME, &fops); //register device & get back a major device number

    if( major < 0)
        printk(KERN_ALERT "Device registration failed.. \n");
    else
        printk(KERN_INFO "Device registered with major number: %d\n", major);

    return major;
}

//Unload driver
void cleanup_module(void)
{

    unregister_chrdev(major, DEVICE_NAME);
}

//called when opening the device driver
static int dev_open(struct inode *inod, struct file * fp)
{
    count++;
    printk(KERN_INFO "Device opened %d times\n", count);
    return 0;
}

//Called when making a read request to the driver
static ssize_t dev_read(struct file *fp, char *buff, size_t length, loff_t *off)
{

    short bytes = 0;

    if(readIndex < length)
    {
        printk(KERN_NOTICE "Can't service full read request, only %d bytes remain\n", readIndex);
    }

    while(length &&(msg[readIndex]!= 0))
    {
        put_user(msg[readIndex], buff++);
        bytes++;
        length--;
        readIndex--;
    }

    return bytes;
}

//Called when making a write request to the driver
static ssize_t dev_write(struct file *fp, const char *buff, size_t length, loff_t *off)
{
    short index = length-1;
    short bytes = 0;

    while(readIndex < BUFF_LEN && length > 0)
    {
        msg[bytes++] = buff[ind--];
        length--;
        readIndex++;
    }

    return bytes;
}

//Called when closing the device driver
static int dev_release(struct *inode, struct file *fp)
{
    printk(KERN_INFO "Device closed");
    return 0;
}
