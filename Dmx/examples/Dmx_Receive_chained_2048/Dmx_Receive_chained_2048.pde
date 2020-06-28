/*************************************************************************************************************
*
* Title			    : Example DMX mode conversion and reception with 3 Universes chained
* Version		    : v 0.3
* Last updated	            : 07.07.2012
* Target		    : Arduino mega 2560, Arduino mega 1280
* Author                    : Toni Merino - merino.toni at gmail.com
* Web                       : www.deskontrol.net/blog
*
**************************************************************************************************************/
#include <lib_dmx.h>  // comment/uncomment #define USE_UARTx in lib_dmx.h as needed

// This example test 3 universes at one time with diferent DMX modes:  RX-3 DMX-512 - TX-2 DMX1000K - RX-1 DMX1000K
// and handle 512 input channels + 2048 output channels + 2048 input channels, asincronous data updates in main loop.
// GoooOOO Arduino

// *** Place a wire loop between output pin of universe 2 (Arduino pin 16 - TX2) and universe 1 input pin (Arduino pin 19 - RX1) ***

// Signal from external controller, inputs at universe 3 RX, copies universe 3 input buffer to universe 2 output buffer and loops 
// with a wire to universe 1 input, then received data on universe 1 is write to analog output pins... 

// Remember:
// Standard DMX-512 signal from external controller is applied to MAX 485 in universe 3 input pin (Arduino pin 15 - RX3), data received in 
// universe 3 INPUT buffer are copied in main loop to universe 2 OUTPUT buffer (4 times to fill 2048 channels), and transmitted 
// by universe 2 in DMX-1000K (2048 channels) mode, then received by universe 1 in DMX-1000K (2048 channels) mode (via the loop wire),
// and received value from original input channel 512 written to PWM pins 2 to 5.

// And voila, our original value from input channel 512 is now at DMX-1000K channels 512-1024-1536-2048, Enjoy ;)

// Is this useful??? 
// Advantages are clear, DMX-1000K is 4 times faster than USITT DMX512, 4 times faster is 168 Hz refresh rate with 512 channels,
// or 4 times faster is 4 times more channels in one universe at DMX standard refresh rate of 42 Hz.
// Many manufacturers are producing led matrix and other led systems and controllers with unknow (for me) specification
// called DMX-1000K: unoficial?, but standard de facto...

// If you have any information about strange DMX modes used by led lighting, (DMX derivates like 1536 channels mode, modes 
// with/without break and other funny modes, not SPI) please contact me.

//*********************************************************************************************************
//                        New DMX modes *** EXPERIMENTAL ***
//*********************************************************************************************************
#define    DMX512     (0)    // (250 kbaud - 2 to 512 channels) Standard USITT DMX-512
#define    DMX1024    (1)    // (500 kbaud - 2 to 1024 channels) Completely non standard - TESTED ok
#define    DMX2048    (2)    // (1000 kbaud - 2 to 2048 channels) called by manufacturers DMX1000K, DMX 4x or DMX 1M ???

#define    CHANNELS   (512)

void setup() 
{
  ArduinoDmx3.set_rx_address(1);              // set RX 3 DMX start address
  ArduinoDmx3.set_rx_channels(CHANNELS);      // number of RX channels universe 3
  ArduinoDmx3.init_rx( DMX512 );              // starts universe 3 as RX (DMX input from external controller), DMX mode: standard USITT DMX-512  
  
  ArduinoDmx2.set_tx_address(1);              // set TX 2 DMX start address
  ArduinoDmx2.set_tx_channels(CHANNELS * 4);  // number of TX channels
  ArduinoDmx2.init_tx( DMX2048 );             // starts universe 2 as TX (wire loop to universe 1), DMX mode: DMX2048 (DMX1000K)
  
  ArduinoDmx1.set_rx_address(1);              // set RX 1 DMX start address
  ArduinoDmx1.set_rx_channels(CHANNELS * 4);  // number of RX channels
  ArduinoDmx1.init_rx( DMX2048 );             // starts universe 1 as RX (wire loop from universe 2), DMX mode: DMX2048 (DMX1000K)
  
}  //end setup()

void loop()
{
  // copy data from RX 3 buffer to TX 2 buffer 4 times (repeat values 4 times in order to fill 2048 output channels of DMX1000K)
  
  // To channels 1-512
  memcpy((void *)ArduinoDmx2.TxBuffer, (void *)ArduinoDmx3.RxBuffer, CHANNELS);
  // To channels 513-1024
  memcpy((void *)&ArduinoDmx2.TxBuffer[512], (void *)ArduinoDmx3.RxBuffer, CHANNELS);
  // To channels 1025-1536
  memcpy((void *)&ArduinoDmx2.TxBuffer[1024], (void *)ArduinoDmx3.RxBuffer, CHANNELS);
  // To channels 1537-2048
  memcpy((void *)&ArduinoDmx2.TxBuffer[1536], (void *)ArduinoDmx3.RxBuffer, CHANNELS);
  
  // write values from RX 1 buffer, (dmx channel 512 only, to arduino pwm pins 2-5)
  analogWrite(2, ArduinoDmx1.RxBuffer[2047]);  // DMX input channel 512 -> DMX1000K channel 2048
  analogWrite(3, ArduinoDmx1.RxBuffer[1535]);  // DMX input channel 512 -> DMX1000K channel 1536
  analogWrite(4, ArduinoDmx1.RxBuffer[1023]);  // DMX input channel 512 -> DMX1000K channel 1024
  analogWrite(5, ArduinoDmx1.RxBuffer[511]);   // DMX input channel 512 -> DMX1000K channel 512
}  //end loop()

