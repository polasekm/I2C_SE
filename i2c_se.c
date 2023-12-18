/*-----------------------------------------------------------------------------*/
/*
 * i2c_se.c
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
#include "i2c_se.h"
//---------------------------------------------------------------------

TIM_HandleTypeDef htim;

uint8_t i2c_se_state;
i2c_se_stat_t i2c_se_stat;
//---------------------------------------------------------------------
void i2c_se_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  //------------------------------------------------------

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL|I2C_SE_SDA, GPIO_PIN_SET);

  /*Configure GPIO pins : SCL_Pin SDA_Pin */
  GPIO_InitStruct.Pin = I2C_SE_SCL | I2C_SE_SDA;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(I2C_SE_PORT, &GPIO_InitStruct);
  //--------------------------------------------------------------------
  i2c_se_state = 0;
  //--------------------------------------------------------------------
  //i2c_se_stat.err_I2C_FLAG_BUSY = 0;
  i2c_se_stat.err_I2C_EVENT_MASTER_MODE_SELECT = 0;
  i2c_se_stat.err_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED = 0;
  i2c_se_stat.err_I2C_EVENT_MASTER_BYTE_TRANSMITTED = 0;
  i2c_se_stat.err_I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED = 0;
  i2c_se_stat.err_I2C_EVENT_MASTER_BYTE_RECEIVED = 0;
  //--------------------------------------------------------------------
}
//---------------------------------------------------------------------
void i2c_se_generate_start(void)
{
  /*I2C_GenerateSTART(I2C_se, ENABLE);
  i2c_se_delay(12);

  I2C_CheckEvent(I2C_se, I2C_EVENT_MASTER_MODE_SELECT);*/

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL|I2C_SE_SDA, GPIO_PIN_SET);
  i2c_se_delay(2);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_RESET);
  i2c_se_delay(3);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
  i2c_se_delay(1);
}
//---------------------------------------------------------------------
void i2c_se_generate_stop(void)
{
  /*I2C_GenerateSTOP(I2C_SE, ENABLE);
  i2c_se_delay(12);*/

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_RESET);
  i2c_se_delay(1);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
  i2c_se_delay(3);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);
  i2c_se_delay(2);

}
//---------------------------------------------------------------------
/* I2C ADD0 mask */
#define OAR1_ADD0_Set           ((uint16_t)0x01)
#define OAR1_ADD0_Reset         ((uint16_t)0xFE)
//---------------------------------------------------------------------
void i2c_se_send_7b_addr(uint8_t addr, uint8_t dir)
{
  /*I2C_Send7bitAddress(I2C_se, addr, dir);
  i2c_se_delay(102);

  I2C_CheckEvent(I2C_se, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);*/

  /* Check the parameters */
  //assert_param(IS_I2C_DIRECTION(dir));
  /* Test on the direction to set/reset the read/write bit */
  if(dir == I2C_SE_DIR_WRITE)
  {
    /* Reset the address bit0 for write */
    addr &= OAR1_ADD0_Reset;
  }
  else
  {
    /* Set the address bit0 for read */
    addr |= OAR1_ADD0_Set;
  }
  /* Send the address */
  i2c_se_send_data(addr);
}
//---------------------------------------------------------------------
void i2c_se_send_data(uint8_t data)
{
  /*I2C_SendData(I2C_se, data);
  i2c_se_delay(102);

  I2C_CheckEvent(I2C_se, I2C_EVENT_MASTER_BYTE_TRANSMITTED);*/

  uint8_t i;

  for(i = 0; i < 8; i++)
  {
    // DATA
    if((data & 0x80) == 0) HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_RESET);
                      else HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);

    data = data << 1;
    //-----------------------
    i2c_se_delay(1);

    HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
    i2c_se_delay(2);

    HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
    i2c_se_delay(1);
    //-----------------------
  }

  // ACK
  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);
  i2c_se_delay(1);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
  i2c_se_delay(2);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
  i2c_se_delay(1);
  //-----------------------
}
//---------------------------------------------------------------------
void i2c_se_send_data_wack(uint8_t data)   //send data without ack
{
  /*I2C_SendData(I2C_SE, data);
  i2c_se_delay(102);

  I2C_CheckEvent(I2C_SE, I2C_EVENT_MASTER_BYTE_TRANSMITTED);*/

  uint8_t i;

  for(i = 0; i < 8; i++)
  {
    // DATA
    if((data & 0x80) == 0) HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_RESET);
                      else HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);

    data = data << 1;
    //-----------------------
    i2c_se_delay(1);

    HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
    i2c_se_delay(2);

    HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
    i2c_se_delay(1);
    //-----------------------
  }
}
//---------------------------------------------------------------------
void i2c_se_send_ack()                            //send ack
{
  // ACK
  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);
  i2c_se_delay(1);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
  i2c_se_delay(2);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
  i2c_se_delay(1);
  //-----------------------
}
//---------------------------------------------------------------------
uint8_t i2c_se_read_data(void)
{
  uint8_t i;
  uint8_t b;
  uint8_t data = 0;
  uint8_t r = 0x80;

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);

  for(i = 0; i < 8; i++)
  {
    //-----------------------
    i2c_se_delay(1);

    HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
    i2c_se_delay(1);
    //-----------------------
    // DATA
    b = HAL_GPIO_ReadPin(I2C_SE_PORT, I2C_SE_SDA);

    if(b != 0) data = data | r;

    r = r >> 1;
    i2c_se_delay(1);
    //-----------------------

    HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
    i2c_se_delay(1);
    //-----------------------
  }

  // ACK
  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_RESET);
  i2c_se_delay(1);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
  i2c_se_delay(2);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
  i2c_se_delay(1);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);
  //-----------------------

  return data;
}
//---------------------------------------------------------------------
uint8_t i2c_se_read_data_wack(void)
{
  uint8_t i;
  uint8_t b;
  uint8_t data = 0;
  uint8_t r = 0x80;

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);

  for(i = 0; i < 8; i++)
  {
    //-----------------------
    i2c_se_delay(1);

    HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
    i2c_se_delay(1);
    //-----------------------
    // DATA
    b = HAL_GPIO_ReadPin(I2C_SE_PORT, I2C_SE_SDA);

    if(b != 0) data = data | r;

    r = r >> 1;
    i2c_se_delay(1);
    //-----------------------

    HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
    i2c_se_delay(1);
    //-----------------------
  }

  //WITHOUT ACK

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SDA, GPIO_PIN_SET);
  i2c_se_delay(1);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_SET);
  i2c_se_delay(2);

  HAL_GPIO_WritePin(I2C_SE_PORT, I2C_SE_SCL, GPIO_PIN_RESET);
  i2c_se_delay(1);

  //-----------------------

  return data;
}
//---------------------------------------------------------------------
void i2c_se_delay(uint16_t tck)
{
  HAL_Delay(tck);
}
//---------------------------------------------------------------------
