#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define DRIVER_AUTHOR "Rafael Fernández López <ereslibre@kde.org>"
#define DRIVER_DESC "A driver example"

static int __init init_test(void)
{
    printk(KERN_INFO "Hola, mundo\n");
    return 0;
}

static void __exit cleanup_test(void)
{
    printk(KERN_INFO "Adiós, mundo\n");
    
}

module_init(init_test);
module_exit(cleanup_test);

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

MODULE_SUPPORTED_DEVICE("testdevice");
