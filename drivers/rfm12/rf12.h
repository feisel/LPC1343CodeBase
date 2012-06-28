/*
 * rf12.h
 *
 *  Created on: 12.02.2012
 *      Author: kappos
 */

#ifndef RF12_H_
#define RF12_H_

#include "LPC134x.h"

extern  void Delay (uint32_t dlyTicks);
					// macro for calculating frequency value out of frequency in MHz
extern void SPIInit(void);
extern uint16_t rf12_trans(uint16_t wert);// transfer 1 word to/from module
extern void rf12_init(void);											// initialize module
extern void rf12_setfreq(unsigned short freq);							// set center frequency
extern void rf12_setbaud(unsigned short baud);							// set baudrate
extern void rf12_setpower(unsigned char power, unsigned char mod);		// set transmission settings
extern void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi);	// set receiver settings
extern void rf12_txdata(unsigned char *data, unsigned char number);		// transmit number of bytes from array
extern void rf12_rxdata(unsigned char *data, unsigned char number);		// receive number of bytes into array
extern void rf12_ready(void);
void rf12_enable_wakeup(void);

#endif /* RF12_H_ */
