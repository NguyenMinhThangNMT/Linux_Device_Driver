#include<linux/module.h>
#include<linux/init.h>
#include<linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_DESCRIPTION("A simple gpio for linux kernel");

#define LED 21

static int __init ModuleInit(void){
    /* */
    if(gpio_is_valid(LED)==false){
        pr_err("GPIO %d is not valid\n",LED);
        return -1;
    }
    if(gpio_request(LED,"LED")<0){
        pr_err("Error : GPIO %d request\n",LED);
        goto r_gpio;
    }
    gpio_direction_output(LED,1);
    gpio_export(LED,false);
    pr_info("GPIO Driver Insert Done\n");
    return 0;

r_gpio:
    gpio_free(LED);
    return -1;
}

static void __exit ModuleExit(void){
    gpio_direction_output(LED,0);
    gpio_unexport(LED);
    gpio_free(LED);
    pr_info("Driver Removed\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

