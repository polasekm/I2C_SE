/*-----------------------------------------------------------------------------*/
/*
 * i2c_se.h
 *
 *  SW emulated I2C interface
 *
 *     Created:  9. 5. 2011
 *    Modified: 19. 9. 2019
 *      Author: Martin Polasek, polasek.martin@seznam.cz, http://www.polasekm.cz
 *
 *        ver.: 2.1.0
 */
/*-----------------------------------------------------------------------------*/
#ifndef I2C_SE_H_INCLUDED
#define I2C_SE_H_INCLUDED
//---------------------------------------------------------------------
#include <stddef.h>

#include "main.h"
//---------------------------------------------------------------------
#define I2C_SE_DIR_WRITE 0
#define I2C_SE_DIR_READ 1
//---------------------------------------------------------------------
#define I2C_SE_SCL     GPIO_PIN_6
#define I2C_SE_SDA     GPIO_PIN_7
#define I2C_SE_PORT    GPIOB
#define I2C_SE         I2C1
//---------------------------------------------------------------------
typedef struct
{
  uint16_t err_I2C_FLAG_BUSY;
  uint16_t err_I2C_EVENT_MASTER_MODE_SELECT;
  uint16_t err_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED;
  uint16_t err_I2C_EVENT_MASTER_BYTE_TRANSMITTED;
  uint16_t err_I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED;
  uint16_t err_I2C_EVENT_MASTER_BYTE_RECEIVED;

} i2c_se_stat_t;

//---------------------------------------------------------------------
void i2c_se_init(void);

void i2c_se_generate_start(void);
void i2c_se_generate_stop(void);
void i2c_se_send_7b_addr(uint8_t addr, uint8_t dir);

void i2c_se_send_data(uint8_t data);        //send data
void i2c_se_send_data_wack(uint8_t data);   //send data without ack
void i2c_se_send_ack();                     //send ack

uint8_t i2c_se_read_data(void);             //read data
uint8_t i2c_se_read_data_wack(void);        //read data without ack set

void i2c_se_delay(uint16_t tck);
//---------------------------------------------------------------------

#endif /* I2C_SE_H_INCLUDED */
