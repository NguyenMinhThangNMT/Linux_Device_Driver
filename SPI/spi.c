#include <linux/module.h>
#include <linux/init.h>
#include <linux/spi/spi.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NMT");
MODULE_DESCRIPTION("Driver SPI for Linux kernel");

// Khai báo hàm SPI_Send trước khi sử dụng
static int SPI_Send(struct spi_device *client, uint8_t reg, uint8_t value);

static int my_spi_probe(struct spi_device *client);
static void my_spi_remove(struct spi_device *client);

static struct of_device_id my_driver_ids[] = {
    {
        .compatible = "raspberry,spi",
    }, { }
};

MODULE_DEVICE_TABLE(of, my_driver_ids);

static struct spi_device_id my_spi[] = {
    {"my_spi", 0},
    { },
};
MODULE_DEVICE_TABLE(spi, my_spi);

static struct spi_driver my_driver = {
    .probe = my_spi_probe,
    .remove = my_spi_remove,
    .id_table = my_spi,
    .driver = {
        .name = "my_spi",
        .of_match_table = my_driver_ids,
    },
};

// Hàm probe được gọi khi driver được liên kết với thiết bị
static int my_spi_probe(struct spi_device *client) {
    uint8_t mssv[8] = {8,8,8,8,8,8,8,8};
    pr_info("my_spi: Probe function called\n");

    // Gửi dữ liệu qua SPI
    SPI_Send(client,0x09, 0xff);
    // set intensity: 0x0A09
    SPI_Send(client,0x0A, 9);
    // set scanlimit
    SPI_Send(client,0x0B, 7);
    // no shutdown, turn off display test
    SPI_Send(client,0x0C, 1);
    SPI_Send(client,0x0F, 0);

    for(int i=0;i<8;i++){
        SPI_Send(client,i+1, mssv[7-i]);
    }

    pr_info("my_spi: Data sent successfully\n");
    return 0;
}

// Hàm remove được gọi khi driver bị hủy liên kết với thiết bị
static void my_spi_remove(struct spi_device *client) {
    pr_info("my_spi: Remove function called\n");
    // Thực hiện các tác vụ cần thiết khi thiết bị bị gỡ bỏ
}

static int SPI_Send(struct spi_device *client, uint8_t reg, uint8_t value) {
    int ret;
    uint8_t buffer[2];
    buffer[0] = reg;
    buffer[1] = value;

    ret = spi_write(client, buffer, sizeof(buffer));
    if (ret < 0) {
        pr_err("my_spi: Send data error\n");
        return ret;
    }
    

    return 0;
}

static int __init my_driver_init(void) {
    int ret;

    pr_info("my_spi: Initializing SPI driver\n");

    ret = spi_register_driver(&my_driver);
    if (ret) {
        pr_err("my_spi: Failed to register SPI driver\n");
        return ret;
    }
    

    pr_info("my_spi: SPI driver registered successfully\n");
    return 0;
}

static void __exit my_driver_exit(void) {
    pr_info("my_spi: Exiting SPI driver\n");

    spi_unregister_driver(&my_driver);

    pr_info("my_spi: SPI driver unregistered successfully\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);
