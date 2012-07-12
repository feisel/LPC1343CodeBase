#ifndef _SHT21_H_
#define _SHT21_H_

#include "projectconfig.h"
#include "core/i2c/i2c.h"


#define SHT21_ADDRESS (0x50) // 010 1000 shifted left 1 bit = 0x90
#define SHT21_READBIT (0x01)
#define SHT21_WRITEBIT (0x00)


typedef enum
{
  SHT21_ERROR_OK = 0,               // Everything executed normally
  SHT21_ERROR_I2CINIT,              // Unable to initialise I2C
  SHT21_ERROR_I2CBUSY,              // I2C already in use
  SHT21_ERROR_LAST
}
sht21Error_e;

sht21Error_e sht21Init(void);
sht21Error_e sht21Read(int32_t *hum,int32_t *temp);
sht21Error_e sht21MeasureRH(void);

#endif