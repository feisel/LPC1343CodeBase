/*
 * rf12.c
 *
 *  Created on: 12.02.2012
 *      Author: kappos
 */

#include "rf12.h"
#include "LPC134x.h"
//#include "common.h"
#include "core/systick/systick.h"




int static mosiPin=3;
int static misoPin=2;
int static sckPin=1;
int static sselPin=0;


volatile uint32_t msTicks;                            /* counts 1ms timeTicks */

void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
 void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}


void SPIInit(void){

	// MOSI und CLK auf Ausgang setzen
	 // SOFT_SPI_MOSI_DDR |=(1<<SOFT_SPI_MOSI_BIT);
	//  SOFT_SPI_CLK_DDR  |=(1<<SOFT_SPI_CLK_BIT);
	GPIO_GPIO2DIR  |=(1<<mosiPin);
	GPIO_GPIO2DIR  |=(1<<sckPin);
	GPIO_GPIO2DIR  |=(1<<sselPin);

  // MOSI und CLK auf HIGH setzen
	GPIO_GPIO2DATA  |=(1<<mosiPin);

	//sck auf low
	GPIO_GPIO2DATA  &=~(1<<sckPin);

	//Chip Select auf high setzen
	GPIO_GPIO2DATA  |=(1<<sselPin);
  // MISO auf Eingang setzen
	GPIO_GPIO2DIR &=~(1<<misoPin);
}

uint16_t rf12_trans(uint16_t wert)
{	uint16_t werti=0;
	int i;

	 GPIO_GPIO2DATA &=~(1<<sselPin);
	for (i=0; i<16; i++)
	{	if (wert&32768)
		GPIO_GPIO2DATA |=(1<<mosiPin);
			//sbi(RF_PORT, SDI);
		else
			GPIO_GPIO2DATA &=~(1<<mosiPin);
		werti<<=1;

		if(GPIO_GPIO2DATA &(1<<misoPin))
			werti|=1;
		GPIO_GPIO2DATA |=(1<<sckPin);
		wert<<=1;

		//Delay(1);
		GPIO_GPIO2DATA &=~(1<<sckPin);
	}
	GPIO_GPIO2DATA  |=(1<<sselPin);
	return werti;
}



void rf12_init(void)
{
systickDelay(30);
	

	rf12_trans(0xC0E0);			// AVR CLK: 10MHz
	rf12_trans(0x80D7);			// Enable FIFO
	rf12_trans(0xC2AB);			// Data Filter: internal
	rf12_trans(0xCA81);			// Set FIFO mode
	rf12_trans(0xE000);			// disable wakeuptimer
	rf12_trans(0xC800);			// disable low duty cycle
	rf12_trans(0xC4F7);			// AFC settings: autotuning: -10kHz...+7,5kHz
}

void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi)
{
	rf12_trans(0x9400|((bandwidth&7)<<5)|((gain&3)<<3)|(drssi&7));
}

void rf12_setfreq(unsigned short freq)
{	if (freq<96)				// 430,2400MHz
		freq=96;
	else if (freq>3903)			// 439,7575MHz
		freq=3903;
	rf12_trans(0xA000|freq);
}

void rf12_setbaud(unsigned short baud)
{
	if (baud<663)
		return;
	if (baud<5400)					// Baudrate= 344827,58621/(R+1)/(1+CS*7)
		rf12_trans(0xC680|((43104/baud)-1));
	else
		rf12_trans(0xC600|((344828UL/baud)-1));
}

void rf12_setpower(unsigned char power, unsigned char mod)
{
	rf12_trans(0x9800|(power&7)|((mod&15)<<4));
}

void rf12_enable_wakeup(void)
{
//	 rf12_trans(0x8201);
	 rf12_trans(0x8201);

  //rf12_trans(0x8221);
  rf12_trans(0xE730);    // Wakup-Timer 6144 ms
  rf12_trans(0x8203);    // Wakeup aktivieren, Clockoutput an
  rf12_trans(0x0000);    // Statusregister auslesen;
}

void rf12_ready(void)
{
	GPIO_GPIO2DATA  &= ~(1<<sselPin);
	//asm("nop");
	//asm("nop");
	//asm("nop");
	while (!(GPIO_GPIO2DATA&(1<<misoPin))); // wait until FIFO ready
}

void rf12_txdata(unsigned char *data, unsigned char number)
{
	unsigned char i;

	rf12_trans(0x8238);			// TX on
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB82D);
	rf12_ready();
	rf12_trans(0xB8D4);
	for (i=0; i<number; i++)
	{
		rf12_ready();
		rf12_trans(0xB800|(*data++));
	}
	rf12_ready();
	rf12_trans(0x8208);			// TX off
}

void rf12_rxdata(unsigned char *data, unsigned char number)
{	unsigned char i;

	rf12_trans(0x82C8);			// RX on
	rf12_trans(0xCA81);			// set FIFO mode
	rf12_trans(0xCA83);			// enable FIFO mit syncword
	//rf12_trans(0xCA86); // ohne synword

	for (i=0; i<number; i++)
	{
		rf12_ready();
		*data++=rf12_trans(0xB000);
	}
	rf12_trans(0x8208);			// RX off
}
