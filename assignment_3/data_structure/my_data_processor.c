#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sort.h>
#include <uapi/linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_AUTHOR ("Sorabh Gandhi <sorabh.gandhi@colorado.edu>");
MODULE_DESCRIPTION ("Kernel Data Processing") ;
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

#define ARRAY_LENGTH (50)

static char *input_list[10] = { "Frog",
    "Camel",
    "Chicken",
    "Alpaca",
    "Ant",
    "Cat",
    "Bat",
    "Ape",
    "Ant",
    "Donkey",
    "Tiger",
    "Lion",
    "Camel",
    "Bat",
    "Bear",
    "Beaver",
    "Bee",
    "Frog",
    "Boar",
    "Buffalo",
    "Camel",
    "Camel",
    "Frog",
    "Cat",
    "Bat",
    "Cat",
    "Cat",
    "Cattle",
    "Chamois",
    "Cheetah",
    "Chicken",
    "Chicken",
    "Bear",
    "Bee",
    "Frog",
    "Snake",
    "Cod",
    "Cod",
    "Lion",
    "Lion",
    "Crab",
    "Crane",
    "Crab",
    "Crow",
    "Frog",
    "Deer",
    "Frog",
    "Dog",
    "Frog"};

    static int len = 50;
    static int duplicate_input_count[ARRAY_LENGTH];

    static char sorted_input_list[ARRAY_LENGTH][10];
    static int sorted_input_count[ARRAY_LENGTH];

    static char temp_array[ARRAY_LENGTH][10];
    static int temp_array_count[ARRAY_LENGTH];

    struct animals {
        char name[10];
        int count;
        struct list_head list;
    };

    struct animals sorted_list;
    struct animals filtered_list;
    struct animals *temp;

    static char *user_name = "DEFAULT";
    static int user_count = 1;

    module_param(user_name, charp, 0644);
    MODULE_PARM_DESC(user_name, "A character string to hold the user specified Animal name");
    module_param(user_count, int, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
    MODULE_PARM_DESC(user_count, "An integer to hold the user specified count value");

    module_param_array(input_list, charp, &len, 0644);
    MODULE_PARM_DESC(input_list, "An character array of input animal names");

int compare_function(const void *a, const void *b) {
    char *x = (char *) a;
    char *y = (char *) b;

    if ((strcmp(x, y)) >= 0) {
        return 1;
    } else {
        return -1;
    }
}

int init_module(void)
{
    int i = 0, j = 0, array_count = 0, copy_element = 0, count = 0, elements = 0;

    printk(KERN_INFO "Initializing Kernel Data Processing module\n");

    INIT_LIST_HEAD(&sorted_list.list);
    elements = (sizeof(input_list) / sizeof(input_list[0]));
    //elements = 50;

    sort(input_list, elements, sizeof(input_list[0]), compare_function, NULL);

    for (i = 0; i < elements; i++)
    {
        for (j = 0; j < elements; j++)
        {
            if (strcmp(input_list[i], input_list[j]) == 0) {
                duplicate_input_count[i]++;
            }
        }
    }

    printk(KERN_INFO "Printing the sorted list\n");

    for (i = 0; i < elements; i++)
    {
        for (j = 0; j < elements; j++)
        {
            if (strcmp(input_list[i], sorted_input_list[j]) == 0) {
                copy_element = 1;
            }
        }
    
        if (copy_element == 0) {
            strcpy(sorted_input_list[array_count], input_list[i]);
            sorted_input_count[array_count] = duplicate_input_count[i];
            printk(KERN_ALERT "Animal Name = %s & Count = %d\n", sorted_input_list[array_count], sorted_input_count[array_count]);
            array_count++;
        }
        copy_element = 0;
    }

    //printk(KERN_INFO "*************Debug Print******************\n");

    for (i = 0; i < array_count; i++)
    {
        temp = (struct animals *) kmalloc(sizeof(struct animals), GFP_KERNEL);

        if (temp != NULL) {
            strcpy(temp->name, sorted_input_list[i]);
            temp->count = sorted_input_count[i];
           // printk(KERN_ALERT "Animal Name = %s & Count = %d\n", temp->name, temp->count);
            list_add_tail(&(temp->list), &(sorted_list.list));
        } else {
            printk("kmallc error\n");
        }
    }

    printk(KERN_ALERT "Total node count for 1st linked list = %d\n", array_count);
    printk(KERN_ALERT "Total memory allocated for 1st linked list = %d\n", (array_count * sizeof(struct animals)));

    //printk(KERN_INFO "*************Filtering Print******************\n");
    //Filter according to the user-defined count
    list_for_each_entry(temp, &(sorted_list.list), list)
    {
       if ((strcmp(user_name, "DEFAULT") == 0)) {
            if ((temp->count) >= user_count) {
                strcpy(temp_array[count], (temp->name));
                temp_array_count[count] = (temp->count);
                count++;
            }
        }
        else { 
            if ((strcmp(temp->name, user_name) == 0) && ((temp->count) >= user_count)) {
                strcpy(temp_array[count], (temp->name));
                temp_array_count[count] = (temp->count);
                
               // printk(KERN_ALERT "Animal Name = %s & Count = %d\n", temp->name, temp->count);
               // printk(KERN_ALERT "Animal Name = %s & Count = %d\n", temp_array[count], temp_array_count[count]);
                count++;
           }
        }
    }

    INIT_LIST_HEAD(&filtered_list.list);

   // printk(KERN_INFO "*************Filtering Linked list******************\n");
    for (j = 0; j < count; j++)
    {
        temp = (struct animals *) kmalloc(sizeof(struct animals), GFP_KERNEL);

        if (temp != NULL) {
            strcpy(temp->name, temp_array[j]);
            temp->count = temp_array_count[j];
            //printk(KERN_ALERT "Animal Name = %s & Count = %d\n", temp->name, temp->count);
            list_add_tail(&(temp->list), &(filtered_list.list));
        } else {
            printk("kmallc error\n");
        }

    }

    printk(KERN_ALERT "Total node count for 2nd linked list = %d\n", count);
    printk(KERN_ALERT "Total memory allocated for 2st linked list = %d\n", (count * sizeof(struct animals)));
    printk(KERN_ALERT "Printing final filtered list\n");

    list_for_each_entry(temp, &(filtered_list.list), list)
    {
        printk("Animal Name = %s & Count = %d\n", temp->name, temp->count);
    }

    return 0;
}

void cleanup_module(void)
{
    int count1 = 0, count2 = 0;
    struct animals *temp1, *tempanimal;

    list_for_each_entry_safe(temp1, tempanimal, &sorted_list.list, list) {
        
        list_del(&temp1->list);
        kfree(temp1);
        count1++;
    }

    list_for_each_entry_safe(temp1, tempanimal, &filtered_list.list, list) {

        list_del(&temp1->list);
        kfree(temp1);
        count2++;
    }

    printk(KERN_ALERT "Total memory freed in the cleanup function = %d\n", ((count1 * sizeof(struct animals)) + (count2 * sizeof(struct animals))));
    printk(KERN_INFO "Kernel Data Processing module uninstalling\n");

    return;
}
