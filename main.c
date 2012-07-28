/**************************************************************************/
/*! 
    @file     main.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2011, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "projectconfig.h"
#include "sysinit.h"

#include "core/iap/iap.h"
#include "core/gpio/gpio.h"
#include "core/systick/systick.h"
#include "drivers/rf/rfm12/rf12.h"
#include "drivers/sensors/lm75b/lm75b.h"

#include "drivers/sensors/hyt371/hyt371.h"

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif

#define RF12FREQ(freq)	((freq-430.0)/0.0025)
void InitRFM12()
{
	SPIInit();

	rf12_init();					// ein paar Register setzen (z.B. CLK auf 10MHz)
	rf12_setfreq(RF12FREQ(433.92));	// Sende/Empfangsfrequenz auf 433,92MHz einstellen
	rf12_setbandwidth(4, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm
	rf12_setbaud(1000);			// 19200 baud
	rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120kHz Frequenzshift

}



int main2(void)
{
  systemInit();

  InitRFM12();

    char Buffer[20];
 printf("Init Success");
  while(1)
  {
  rf12_rxdata(Buffer,10);
//  systickDelay(2000);
 // printf("data:");

  printf("data: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x ;\n", Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4]
  ,Buffer[5], Buffer[6], Buffer[7], Buffer[8], Buffer[9]);
  }
}

/**************************************************************************/
/*! 
    Main program entry point.  After reset, normal code execution will
    begin here.
*/
/**************************************************************************/
int main(void)
{
systemInit();
  //CPU Init
 // cpuInit();
// systemInit();


  //Declare Vars
  uint32_t pmuRegVal;
 

  systickDelay(5000);
  //Geräte initieren
  InitRFM12();
 // yht371Init();

   lm75bInit();


  uint32_t temp;
  uint32_t hum;

 

    // Inidicate which peripherals should be disabled in deep-sleep
    pmuRegVal = SCB_PDSLEEPCFG_IRCOUT_PD | 
                SCB_PDSLEEPCFG_IRC_PD | 
                SCB_PDSLEEPCFG_FLASH_PD | 
                SCB_PDSLEEPCFG_USBPLL_PD | 
                SCB_PDSLEEPCFG_SYSPLL_PD | 
                SCB_PDSLEEPCFG_SYSOSC_PD | 
                SCB_PDSLEEPCFG_ADC_PD | 
                SCB_PDSLEEPCFG_BOD_PD |//;// |
                 SCB_PDSLEEPCFG_USBPAD_PD |
                 SCB_PDSLEEPCFG_WDTOSC_PD;

    while(1)
    {
    //Int in Char Array umwandeln
    char Buffer[20];
   
 //Feuchtesensor neu messen
    //hyt371DoMR();
    systickDelay(20);
  //  hyt371bRead(&hum,&temp);

lm75bError_e d;
   d = lm75bGetTemperature(&temp);

  temp *= 125;
  temp=temp*0.001;

    //Feuchtigkeit umverpacken und senden
    //itoa( hum, Buffer, 10 );
      //Daten versenden
    //rf12_txdata(Buffer,5);

   //Kurz warten
    uint16_t status = rf12_trans(0x0);	
    systickDelay(100);
      
       status = rf12_trans(0x0);	

 RF12_Packet_T packet;
/*
       //Packet für Feuchte bauen und senden
      
       packet.DeviceId=1;
       packet.PacketId=1;
       packet.CRC[0]=1;
       packet.CRC[1]=1;
       packet.Type = RF12_PacketType_Hum;
       packet.DataLength=1;
       packet.Data[0]=hum;
       rf12_SendPacket(packet);
       */

       
       //Packet für Temperatue bauen und senden
       packet.DeviceId=1;
       packet.PacketId=1;
       packet.CRC[0]=1;
       packet.CRC[1]=1;
       packet.Type = RF12_PacketType_Temp;
       packet.DataLength=1;
       packet.Data[0]=temp;
       packet.Data[1]=0;
       packet.Data[2]=0;
       packet.Data[3]=0;
       rf12_SendPacket(packet);
       

       // RF12 ausschalten um strom zu sparen
        rf12_trans(0x8201);


      systickDelay(5000);        
     //sleep aktiv setzen
    //  pmuInit();
    //  pmuDeepSleep(pmuRegVal, 10);
    }
    return 0;
}
