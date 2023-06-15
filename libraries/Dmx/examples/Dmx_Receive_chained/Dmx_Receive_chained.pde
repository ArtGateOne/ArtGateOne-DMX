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

// This example test 3 universes at one time with diferent DMX modes:  RX-3 DMX-512 - TX-2 DMX-1000K - RX-1 DMX-1000K
// and handle 512 input channels + 512 output channels + 512 input channels, asincronous data updates in main loop.
// GoooOOO Arduino

// *** Place a wire loop between output pin of universe 2 (Arduino pin 16 - TX2) and universe 1 input pin (Arduino pin 19 - RX1) ***

// Signal from external controller, inputs at universe 3 RX, copies universe 3 input buffer to universe 2 output buffer and loops 
// with a wire to universe 1 input, then received data on universe 1 is write to analog output pins... 
// (all proccess with 512 channels, but only 4 DMX channels are written to Arduino PWM outputs)

// Remember:
// Standard DMX-512 signal from controller is applied to universe 3 input pin (Arduino pin 15 - RX3), data received in 
// universe 3 INPUT buffer are copied in main loop to universe 2 OUTPUT buffer, and transmitted by universe 2 in DMX-1000K mode, 
// then received by universe 1 in DMX-1000K mode (via the loop wire), and received values from first 4 channels written 
// to Arduino PWM pins 2 to 5... Enjoy ;)

// Is this useful??? 
// Advantages are clear, DMX-1000K is 4 times faster than USITT DMX512, 4 times faster is 168 Hz refresh rate with 512 channels,
// or 4 times faster is 4 times more channels in one universe at DMX standard refresh rate of 42 Hz.
// Many manufacturers are producing led matrix and other led systems and controllers with unknow (for me) specification
// called DMX-1000K: unoficial?, but standard de facto...

// If you have any information about strange DMX modes used by led lighting, (DMX derivates like 1536 channels mode, modes 
// with/without break and other funny modes, not SPI) please contact me.

//*********************************************************************************************************
//                        New DMX modes   *** EXPERIMENTAL ***
//*********************************************************************************************************
#define    DMX512     (0)    // (250 kbaud - 2 to 512 channels) Standard USITT DMX-512
#define    DMX1024    (1)    // (500 kbaud - 2 to 1024 channels) Completely non standard - TESTED ok
#define    DMX2048    (2)    // (1000 kbaud - 2 to 2048 channels) called by manufacturers DMX1000K, DMX 4x or DMX 1M ???

#define    CHANNELS   (512)

void setup() 
{
  ArduinoDmx3.set_rx_address(1);           // set RX 3 DMX start address
  ArduinoDmx3.set_rx_channels(CHANNELS);   // number of RX channels universe 3
  ArduinoDmx3.init_rx( DMX512 );           // starts universe 3 as RX (DMX input from external controller), DMX mode: standard USITT DMX-512  
  
  ArduinoDmx2.set_tx_address(1);           // set TX 2 DMX start address
  ArduinoDmx2.set_tx_channels(CHANNELS);   // number of TX channels
  ArduinoDmx2.init_tx( DMX2048 );          // starts universe 2 as TX (wire loop to universe 1), DMX mode: DMX2048 (DMX1000K)
  
  ArduinoDmx1.set_rx_address(1);           // set RX 1 DMX start address
  ArduinoDmx1.set_rx_channels(CHANNELS);   // number of RX channels
  ArduinoDmx1.init_rx( DMX2048 );          // starts universe 1 as RX (wire loop from universe 2), DMX mode: DMX2048 (DMX1000K)
  
}  //end setup()

void loop()
{
  // copy data from RX 3 buffer to TX 2 buffer
  memcpy((void *)ArduinoDmx2.TxBuffer, (void *)ArduinoDmx3.RxBuffer, CHANNELS);
  
  // write values from RX 1 buffer, (dmx channels 1-4 only, to arduino pwm pins 2-5)
  analogWrite(2, ArduinoDmx1.RxBuffer[0]);  // DMX channel 1 (buffers 0 indexed)
  analogWrite(3, ArduinoDmx1.RxBuffer[1]);
  analogWrite(4, ArduinoDmx1.RxBuffer[2]);
  analogWrite(5, ArduinoDmx1.RxBuffer[3]);
}  //end loop()

