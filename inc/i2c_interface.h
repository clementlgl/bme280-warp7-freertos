#include "board.h"
#include "debug_console_imx.h"
#include "i2c_imx.h"

// Device

// Status codes
#define IAQCORE_STATUS_OK			(0x00)
#define IAQCORE_STATUS_RUNIN			(0x10)
#define IAQCORE_STATUS_BUSY			(0x01)
#define IAQCORE_STATUS_ERROR			(0x80)

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
void user_delay_ms(uint32_t period);

bool I2C_MasterSendDataPolling(I2C_Type *base,
                                      const uint8_t *cmdBuff,
                                      uint32_t cmdSize,
                                      const uint8_t *txBuff,
                                      uint32_t txSize);
                                      
bool I2C_MasterReceiveDataPolling(I2C_Type *base,
                                         const uint8_t *cmdBuff,
                                         uint32_t cmdSize,
                                         uint8_t *rxBuff,
                                         uint32_t rxSize);
                                                                              
