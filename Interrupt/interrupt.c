#include<linux/module.h>
#include<linux/init.h>
#include<linux/gpio.h>
#include<linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_DESCRIPTION("A simple interrupt for linux");

#define BUTTON 16
#define LED 21
static unsigned int irq_number;//bien luu tru IRQ tuong uong voi gpio
static int a = 0;
/* Interrupt handler */
static irqreturn_t irq_handler(int irq,void *dev_id){
    
    int gpio_state = gpio_get_value(BUTTON); // Lấy trạng thái hiện tại của GPIO

    if (gpio_state == 0) {
        a++;
        if(a == 10){
            a=0;
        }
    }
    if(a % 2 == 0){
        gpio_set_value(LED,1);
    }
    if(a % 2 == 1){
        gpio_set_value(LED,0);
    }

    return IRQ_HANDLED;
}

static int __init ModuleInit(void){
    int result;
    if(gpio_is_valid(BUTTON)==false){
        pr_err("BUTTON is not valid\n");
        return -1;
    }
    if(gpio_is_valid(LED)==false){
        pr_err("BUTTON is not valid\n");
        return -1;
    }
    if(gpio_request(BUTTON,"my_button")<0){
        pr_err("Error: Button request\n");
        goto r_gpio;
    }
    if(gpio_request(LED,"my_led")<0){
        pr_err("Error: Button request\n");
        goto r_gpio;
    }
    gpio_direction_input(BUTTON);
    gpio_direction_output(LED,0);

    gpio_export(BUTTON,false);
    gpio_export(LED,false);

    irq_number = gpio_to_irq(BUTTON);
    pr_info("Button is map to IRQ %d\n",irq_number);
    result = request_irq(irq_number,(irq_handler_t)irq_handler,IRQF_TRIGGER_FALLING,"my irq",NULL);

    if(result){
        pr_err("Cannot request IRQ %d\n",irq_number);
        return result;
    }
    pr_info("IRQ %d successfully requested\n",irq_number);
    return 0;
r_gpio:
    gpio_free(BUTTON);
    gpio_free(LED);
    return -1;
}

static void __exit ModuleExit(void){
    gpio_set_value(LED,0);
    free_irq(irq_number,NULL);
    gpio_unexport(BUTTON);
    gpio_unexport(LED);
    gpio_free(BUTTON);
    gpio_free(LED);
    pr_info("Driver Removed\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);