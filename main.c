#include "rpmsg/rpmsg_rtos.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "board.h"
#include "mu_imx.h"
#include "debug_console_imx.h"
#include "rdc_semaphore.h"
#include "gpio_imx.h"
#include "gpio_pins.h"
#include "queue.h"
#include "i2c_interface.h"
#include "bme280.h"


struct bme280_dev dev;

int8_t stream_sensor_data_normal_mode(struct bme280_dev *dev)
{
	int8_t rslt;
	uint8_t settings_sel;
	struct bme280_data comp_data;

	/* Recommended mode of operation: Indoor navigation */
	dev->settings.osr_h = BME280_OVERSAMPLING_1X;
	dev->settings.osr_p = BME280_OVERSAMPLING_16X;
	dev->settings.osr_t = BME280_OVERSAMPLING_2X;
	dev->settings.filter = BME280_FILTER_COEFF_16;
	dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);

	PRINTF("Temperature, Pressure, Humidity\r\n");
	while (1) {
		/* Delay while the sensor completes a measurement */
		dev->delay_ms(70);
		rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
		print_sensor_data(&comp_data);
	}

	return rslt;
}

void print_sensor_data(struct bme280_data *comp_data)
{
#ifdef BME280_FLOAT_ENABLE
        PRINTF("%0.2f, %0.2f, %0.2f\r\n",comp_data->temperature, comp_data->pressure, comp_data->humidity);
#else
        PRINTF("%ld, %ld, %ld\r\n",comp_data->temperature, comp_data->pressure, comp_data->humidity);
#endif
}

/*!
 * @brief Toggle LED
 */
static void setLED(bool value)
{
#ifdef BOARD_GPIO_LED_CONFIG
	RDC_SEMAPHORE_Lock(BOARD_GPIO_LED_RDC_PDAP);
	GPIO_WritePinOutput(BOARD_GPIO_LED_CONFIG->base,	// GPIO bank
			BOARD_GPIO_LED_CONFIG->pin, 	// GPIO pin
			value);				// Value
	RDC_SEMAPHORE_Unlock(BOARD_GPIO_LED_RDC_PDAP);
#endif
}

void HelloTask(void *pvParameters)
{
    uint8_t receiveBuff;

    // Print the initial banner
    PRINTF("\r\nHello World!\n\n\r");
	stream_sensor_data_normal_mode(&dev);
	

    while(1)
    {
        // Main routine that simply echoes received characters forever

        // First, get character
        receiveBuff = GETCHAR();

        // Now echo the received character
        PUTCHAR(receiveBuff);
    }
}

int main(void)
{
	// Init RDC, Clock, memory
	hardware_init();

	// Configuration
	i2c_init_config_t i2cInitConfig = {
			.baudRate     = 100000u,	// Baud rate pour communiquer avec le composant iAQ
			.slaveAddress = 0x00		// Notre adresse, pas celle de notre slave
	};
	// Init clock
	i2cInitConfig.clockRate = get_i2c_clock_freq(BOARD_I2C_BASEADDR);
	// Init i2c
	I2C_Init(BOARD_I2C_BASEADDR, &i2cInitConfig);
	// Enable i2c
	I2C_Enable(BOARD_I2C_BASEADDR);

	// Affichage sur la liaison série
	PRINTF("\r\n== GLMF Demo Started ==\r\n");

	
	int8_t rslt = BME280_OK;

	//dev.dev_id = BME280_I2C_ADDR_PRIM;
	//dev.intf = BME280_I2C_INTF;	
	//dev.read = user_i2c_read;
	//dev.write = user_i2c_write;
	//dev.delay_ms = user_delay_ms;

	//rslt = bme280_init(&dev);
	//PRINTF("Result : %d", rslt);
	
	uint8_t dev_id = 0x76;
	uint8_t reg_addr = 0xD0;
	uint8_t reg_data;
	uint16_t len = 1;

	user_i2c_read(dev_id, reg_addr, *reg_data, len);
	printf("ID = 0x%x\n", reg_data);
	// Create a demo task which will print Hello world and echo user's input.
    	xTaskCreate(HelloTask, "Print Task", configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY+1, NULL);

	// Start FreeRTOS scheduler
	vTaskStartScheduler();

	while (true);
}

