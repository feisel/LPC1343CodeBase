#include "sht21.h"

extern volatile uint8_t   I2CMasterBuffer[I2C_BUFSIZE];
extern volatile uint8_t   I2CSlaveBuffer[I2C_BUFSIZE];
extern volatile uint32_t  I2CReadLength, I2CWriteLength;


static bool _sht21Initialised = false;


/**************************************************************************/
/*! 
    @brief  Initialises the I2C block
*/
/**************************************************************************/
sht21Error_e sht21Init(void)
{
  // Initialise I2C
  if (i2cInit(I2CMASTER) == false)
  {
    return SHT21_ERROR_I2CINIT;    /* Fatal error */
  }
  
  _sht21Initialised = true;

  return SHT21_ERROR_OK;
  
}

//sends a Measurement Requests to the sensor
sht21Error_e sht21MeasureRH(void)
{
    // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 0;


  I2CMasterBuffer[0] = SHT21_ADDRESS; // I2C device address
  I2CMasterBuffer[1] =0xE5;// 0x0; //Meassurement RH Hold-Master 0xE5
  I2CMasterBuffer[2] = SHT21_ADDRESS | SHT21_WRITEBIT;  

   i2cEngine();

   return SHT21_ERROR_OK;
}

sht21Error_e sht21Read(int32_t *hum,int32_t *temp)
{
  // Clear write buffers
  uint32_t i;
  for ( i = 0; i < I2C_BUFSIZE; i++ )
  {
    I2CMasterBuffer[i] = 0x00;
  }

  I2CWriteLength = 2;
  I2CReadLength = 4;

  I2CMasterBuffer[0] = SHT21_ADDRESS; // I2C device address
  I2CMasterBuffer[1] = 0x0;
  I2CMasterBuffer[2] = SHT21_ADDRESS | SHT21_READBIT;  
  i2cEngine();

  int32_t tempHum = 0;
  tempHum = ((I2CSlaveBuffer[0]<<8) | (I2CSlaveBuffer[1]));
  tempHum &= 0x3FFF;
  tempHum = (100 * tempHum) / 16384;

  int32_t tempTemp = 0;
   tempTemp = 0;
  tempTemp = ((I2CSlaveBuffer[2]<<8) | (I2CSlaveBuffer[3]));
  tempTemp = tempTemp >> 2;   
  tempTemp = (165 *tempTemp) / 16384 -40;

  *temp = tempTemp;
  *hum =tempHum;

  return SHT21_ERROR_OK;
}