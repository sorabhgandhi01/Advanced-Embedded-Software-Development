#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR ("Sorabh Gandhi <sorabh.gandhi@colorado.edu>");
MODULE_DESCRIPTION ("Simple timer") ;
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static struct timer_list my_timer;
static int trigger_count = 0;
static char *name = "DEFAULT";
static int delay = 500;

module_param(name, charp, 0644);
MODULE_PARM_DESC(name, "A character string to hold the user specified name");
module_param(delay, int, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
MODULE_PARM_DESC(delay, "An unsigned integer to hold the delay period in milliseconds");

void my_timer_callback(struct timer_list *data)
{
    trigger_count++;
    printk(KERN_ALERT "Name = %s & Interrupt trigger Count = %d\n", name, trigger_count);
    mod_timer( &my_timer, jiffies + msecs_to_jiffies(delay));
}

int init_module(void)
{
    int ret;

    printk(KERN_INFO "Timer module installing\n");

    timer_setup( &my_timer, my_timer_callback, 0);

    printk(KERN_INFO "Starting timer to fire in %dms\n", delay);
    ret = mod_timer( &my_timer, jiffies + msecs_to_jiffies(delay));
    if (ret) printk(KERN_ERR "Error in mod_timer\n");

    return 0;
}

void cleanup_module(void)
{
    int ret;
 
    ret = del_timer( &my_timer );
    if (ret) printk(KERN_ERR "The timer is still in use...\n");
 
    printk(KERN_INFO "Timer module uninstalling\n");
 
    return;
}
