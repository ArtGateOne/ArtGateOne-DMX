/*************************************************************************************************************
*
* Title			    : Example HTP DMX Muxer for Arduino 4 universes DMX library. 
* Version		    : v 0.3
* Last updated	            : 07.07.2012
* Target		    : Arduino mega 2560, Arduino mega 1280
* Author                    : Toni Merino - merino.toni at gmail.com
* Web                       : www.deskontrol.net/blog
*
**************************************************************************************************************/
#include <lib_dmx.h>  // comment/uncomment #define USE_UARTx in lib_dmx.h as needed

// This sample get 512 channels from universe 1 + 512 channels from universe 2, 
// and write the highgest value to universe 3

//*********************************************************************************************************
//                        New DMX modes *** EXPERIMENTAL ***
//*********************************************************************************************************
#define    DMX512     (0)    // (250 kbaud - 2 to 512 channels) Standard USITT DMX-512
#define    DMX1024    (1)    // (500 kbaud - 2 to 1024 channels) Completely non standard - TESTED ok
#define    DMX2048    (2)    // (1000 kbaud - 2 to 2048 channels) called by manufacturers DMX1000K, DMX 4x or DMX 1M ???

void setup() 
{
  
  ArduinoDmx1.set_control_pin(24);   // Arduino output pin for MAX485 input/output control (connect to MAX485-1 pins 2-3) 
  ArduinoDmx2.set_control_pin(26);   // Arduino output pin for MAX485 input/output control (connect to MAX485-2 pins 2-3) 
  ArduinoDmx3.set_control_pin(28);   // Arduino output pin for MAX485 input/output control (connect to MAX485-3 pins 2-3) 
  
  ArduinoDmx1.set_rx_address(1);    // set rx1 start address
  ArduinoDmx2.set_rx_address(1);    // set rx2 start address
  ArduinoDmx3.set_tx_address(1);    // set tx start address
  
  ArduinoDmx1.set_rx_channels(512); // 2 to 2048!! channels in DMX1000K (512 in standard mode) See lib_dmx.h  *** new *** EXPERIMENTAL
  ArduinoDmx2.set_rx_channels(512); // 2 to 2048!! channels in DMX1000K (512 in standard mode) See lib_dmx.h  *** new *** EXPERIMENTAL
  ArduinoDmx3.set_tx_channels(512); // 2 to 2048!! channels in DMX1000K (512 in standard mode) See lib_dmx.h  *** new *** EXPERIMENTAL
  
  // New parameter needed in init_tx and init_rx: DMX Mode
  ArduinoDmx1.init_rx(DMX512);    // starts universe 1 as rx, standard DMX 512 - See lib_dmx.h, now support for DMX faster modes (DMX 1000K)
  ArduinoDmx2.init_rx(DMX512);    // starts universe 2 as rx, standard DMX 512 - See lib_dmx.h, now support for DMX faster modes (DMX 1000K)
  ArduinoDmx3.init_tx(DMX512);    // starts universe 3 as tx, standard DMX 512 - See lib_dmx.h, now support for DMX faster modes (DMX 1000K)
  
}//end setup()

void loop()
{
  for(int i=0;i<512;i++)  //buffers 0 indexed (0-511)
  {
    // copy values from input buffers to output buffer
    // High value take priority ( HTP )
    if(ArduinoDmx1.RxBuffer[i] > ArduinoDmx2.RxBuffer[i])
      ArduinoDmx3.TxBuffer[i] = ArduinoDmx1.RxBuffer[i];    
    else
      ArduinoDmx3.TxBuffer[i] = ArduinoDmx2.RxBuffer[i];
  }
}  //end loop()

