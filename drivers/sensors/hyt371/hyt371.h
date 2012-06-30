#ifndef _HYT371_H_
#define _HYT371_H_

#include "projectconfig.h"
#include "core/i2c/i2c.h"


#define HYT371_ADDRESS (0x50) // 010 1000 shifted left 1 bit = 0x90
#define HYT371_READBIT (0x01)
#define HYT371_WRITEBIT (0x00)


typedef enum
{
  HYT371_ERROR_OK = 0,               // Everything executed normally
  HYT371_ERROR_I2CINIT,              // Unable to initialise I2C
  HYT371_ERROR_I2CBUSY,              // I2C already in use
  HYT371_ERROR_LAST
}
hyt371Error_e;

hyt371Error_e yht371Init(void);
hyt371Error_e hyt371bRead(int32_t *hum,int32_t *temp);
hyt371Error_e hyt371DoMR(void);

#endif