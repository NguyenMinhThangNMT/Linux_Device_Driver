#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/err.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_DESCRIPTION("Create Devie File for Linux Kernel");

/* Khởi tạo struct */
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static char kernel_buffer[1024];
static int buffer_pointer = 0;

static int etx_open(struct inode *inode , struct file *file){
    pr_info("Driver open function Called\n");
    return 0;
}

static int etx_release(struct inode *inode , struct file *file){
    pr_info("Driver Release function Called\n");
    return 0;
}

static ssize_t etx_read(struct file *file,char __user *buf,size_t len,loff_t *offs){
    int to_copy, not_copied, delta;

	/* Kiểm tra số byte copy */
	to_copy = min(len,(size_t)buffer_pointer);

	/* Kiểm tra số byte không copy được nếu copy hết thì là 0 */
	not_copied = copy_to_user(buf, kernel_buffer, to_copy);

	/* Tính toán số byte trả về */
	delta = to_copy - not_copied;

	return delta;
}

static ssize_t etx_write(struct file *file,const char __user *buf,size_t len,loff_t *offs){
    int to_copy, not_copied, delta;

	/* Kiểm tra số byte copy */
	to_copy = min(len,sizeof(kernel_buffer));

	/* Kiểm tra số byte không copy được nếu copy hết thì là 0 */
	not_copied = copy_from_user(kernel_buffer, buf, to_copy);
    buffer_pointer = to_copy;

	/* Tính toán số byte trả về */
	delta = to_copy - not_copied;

	return delta;

}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .read    = etx_read,
    .write   = etx_write,
    .open    = etx_open,
    .release = etx_release,
};

static int __init DriverInit(void){
    /* Cấp phát số Major */
    if((alloc_chrdev_region(&dev,0,1,"etx_Dev"))<0){
        pr_err("Cannot allocate major number for device\n");
        return -1;
    }
    /* Nếu thành công thì in ra số Major và Minor */
    pr_info("Major = %d, Minor =%d \n",MAJOR(dev),MINOR(dev));

    /* Khởi tạo cdev struct */
    cdev_init(&etx_cdev,&fops);
    /* Thêm cdev vào hệ thống */
    if((cdev_add(&etx_cdev,dev,1))<0){
        pr_err("Cannot add the device to the system\n");
        goto r_class;
    }

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
    /* Gỡ bỏ cdev*/
    cdev_del(&etx_cdev);
    /* Gỡ bỏ số Major và MINOR*/
    unregister_chrdev_region(dev,1);
    pr_info("Kernel Module Remove Successfully\n");
}

module_init(DriverInit);
module_exit(DriverExit);


