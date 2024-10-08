#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/err.h>
#include<linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_DESCRIPTION("Create Devie File for Linux Kernel");

/* Khởi tạo struct */
dev_t dev = 0;
static struct class *dev_class;

static int __init DriverInit(void){
    /* Cấp phát số Major */
    if((alloc_chrdev_region(&dev,0,1,"etx_Dev"))<0){
        pr_err("Cannot allocate major number for device\n");
        return -1;
    }
    /* Nếu thành công thì in ra số Major và Minor */
    pr_info("Major = %d, Minor =%d \n",MAJOR(dev),MINOR(dev));

    /* Khởi tạo class device file*/
    dev_class = class_create(THIS_MODULE,"etx_class");
    if(IS_ERR(dev_class)){
        pr_err("Cannot create the struct class for device\n");
        goto r_class;
    }

    /* Khởi tạo device */
    if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
        pr_err("Cannot create the Device\n");
        goto r_device;
    }
    pr_info("Kernel Module Insert Successfully\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    return -1;
}

static void __exit DriverExit(void){
    /* Gỡ bỏ device */
    device_destroy(dev_class,dev);
    /* Gỡ bỏ class */
    class_destroy(dev_class);
    /* Gỡ bỏ số Major và MINOR*/
    unregister_chrdev_region(dev,1);
    pr_info("Kernel Module Remove Successfully\n");
}

module_init(DriverInit);
module_exit(DriverExit);


