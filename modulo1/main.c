#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
    printk(KERN_INFO "Hola mundo\n");

    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "Adiós mundo\n");
}
