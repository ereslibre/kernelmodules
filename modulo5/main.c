#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h> //put_user

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode*, struct file*);
static int device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);

#define SUCCESS 0
#define DEVICE_NAME "scull"
#define BUF_LEN 80

static int Major;
static int Index = 0;
static int Device_Open = 0;

static char msg[BUF_LEN];
static char *msg_Ptr;

static struct cdev *scull_cdev;

static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

int init_module(void)
{
    int result, err;
    dev_t devno;
    dev_t dev = MKDEV(Major, 0);

    if (Major) {
        result = register_chrdev_region(dev, 1, DEVICE_NAME);
    } else {
        result = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
        Major = MAJOR(dev);
	scull_cdev = cdev_alloc();
    }

    if (result < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	return Major;
    }

    if (Major == 0) {
        Major = result;
    }

    cdev_init(scull_cdev, &fops);
    scull_cdev->owner = THIS_MODULE;
    scull_cdev->ops = &fops;
    err = cdev_add(scull_cdev, dev, 1);

    if (err) {
        printk(KERN_NOTICE "Error during adding %s device\n", DEVICE_NAME);
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}

void cleanup_module(void)
{
    cdev_del(scull_cdev);
}

static int device_open(struct inode *inode, struct file *file)
{
    static int counter = 0;

    if (Device_Open) {
        return -EBUSY;
    }

    Device_Open++;
    sprintf(msg, "I already told you %d times Hello world!\n", counter++);
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;

    module_put(THIS_MODULE);

    return 0;
}

static ssize_t device_read(struct file *filp,
			   char *buffer,
			   size_t length,
			   loff_t *offset)
{
    int bytes_read = 0;

    if (*msg_Ptr == 0) {
        return 0;
    }
   
    while (length && *msg_Ptr) {
        put_user(*(msg_Ptr++), buffer++);
	length--;
	bytes_read++;
    }

    return bytes_read;
}

static ssize_t device_write(struct file *filp,
			    const char *buffer,
			    size_t length,
			    loff_t *offset)
{
    printk(KERN_ALERT "Sorry, this operation is not supported\n");
    return -EINVAL;
}
