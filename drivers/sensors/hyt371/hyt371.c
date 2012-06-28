#include "hyt371.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;


static bool _hyt371Initialised = false;


/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
hyt371Error_e yht371Init(void)
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return HYT371_ERROR_I2CINIT;    /* Fatal error */
  }
  
  _hyt371Initialised = true;

  return HYT371_ERROR_OK;
  
}

hyt371Error_e hyt371bRead(int32_t *hum,int32_t *temp)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 1;
  I2CReadLength = 4;
  I2CMasterBuffer[0] = HYT371_ADDRESS;             // I2C device address
  // Append address w/read bit
  I2CMasterBuffer[1] = HYT371_ADDRESS | HYT371_READBIT;  
  i2cEngine();

  int32_t tempHum = 0;
  tempHum = ((I2CSlaveBuffer[0]<<8) | (I2CSlaveBuffer[1]));
  tempHum &= 0x3FFF;
  tempHum = (100 * tempHum) / 16384;

  int32_t tempTemp = 0;
   tempTemp = 0;
  tempTemp = ((I2CSlaveBuffer[2]<<8) | (I2CSlaveBuffer[3]));
  tempTemp &= 0x3FFF;
  tempTemp = (165 *tempTemp) / 16384;

  *temp = tempTemp;
  *hum =tempHum;

/*
  //  Sign extend negative numbers
  if (I2CSlaveBuffer[0] & 0x80)
  {
    // Negative number
    *value |= 0xFFFFFC00;
  }
  */
  return HYT371_ERROR_OK;
}