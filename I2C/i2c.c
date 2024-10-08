#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/kernel.h>

#define I2C_BUS_AVAILABLE  (1)          // I2C bus available 
#define SLAVE_DEVICE_NAME  ("PC8574")   //Slave name
#define SLAVE_ADDR         (0x67)       //Slave address

static struct i2c_adapter *etx_i2c_adapter = NULL; 
static struct i2c_client  *etx_i2c_client  = NULL;

//I2C write function
static int I2C_Write(uint8_t *buf, uint8_t len) {
    int ret = i2c_master_send(etx_i2c_client, buf, len);
    return ret;  
}

//I2C Read function
static int I2C_Read(uint8_t *buff_recv, uint8_t len) {
    int ret = i2c_master_recv(etx_i2c_client, buff_recv, len);
    return ret;
}

//I2C probe
static int etx_i2c_probe(struct i2c_client *client,
                         const struct i2c_device_id *id) {
    pr_info("I2C Probed!!!\n");
    return 0;
}

//I2C remove
static void etx_i2c_remove(struct i2c_client *client) {
    pr_info("I2C Remove!!!\n");
}

//Init I2C id
static const struct i2c_device_id etx_i2c_id[] = {
    { SLAVE_DEVICE_NAME, 0 },
    {}
};
MODULE_DEVICE_TABLE(i2c, etx_i2c_id);

// Init I2C Driver
static struct i2c_driver etx_i2c_driver = {
    .driver = {
        .name  = SLAVE_DEVICE_NAME,
        .owner = THIS_MODULE,
    },
    .probe     = etx_i2c_probe,
    .remove    = etx_i2c_remove,  
    .id_table  = etx_i2c_id,
};

//Init I2C board
static struct i2c_board_info etx_i2c_board_info = {
    I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SLAVE_ADDR)
};


static int __init etx_driver_init(void) {
    int ret = -1;
    etx_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);

    if (etx_i2c_adapter != NULL) {
        etx_i2c_client = i2c_new_client_device(etx_i2c_adapter, &etx_i2c_board_info);

        if (etx_i2c_client != NULL) {
            ret = i2c_add_driver(&etx_i2c_driver);
            if (ret) {
                pr_err("Failed to add i2c driver\n");
            }
        }
        i2c_put_adapter(etx_i2c_adapter);
    }

    pr_info("Driver Added\n");
    return ret;
}

static void __exit etx_driver_exit(void) {
    i2c_unregister_device(etx_i2c_client);
    i2c_del_driver(&etx_i2c_driver);
    pr_info("Driver Removed\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_DESCRIPTION("I2C driver");
MODULE_VERSION("1.0");