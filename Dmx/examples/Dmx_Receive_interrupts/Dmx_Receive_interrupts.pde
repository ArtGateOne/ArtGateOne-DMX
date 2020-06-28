/*************************************************************************************************************
*
* Title			    : Example DMX Receiver with received frame interrupt
* Version		    : v 0.3
* Last updated	            : 07.07.2012
* Target		    : Arduino mega 2560, Arduino mega 1280, Arduino nano
* Author                    : Toni Merino - merino.toni at gmail.com
* Web                       : www.deskontrol.net/blog
*
**************************************************************************************************************/
#include <lib_dmx.h>  // comment/uncomment #define USE_UARTx in lib_dmx.h as needed

// This example receive 4 channels from address 1 to 4 and write analog values to PWM pins 2 to 5

//*********************************************************************************************************
//                                          *** NEW ***  
//*********************************************************************************************************
// ArduinoDmxN.attachRXInterrupt(my_rx_ISR) don't waste time updating values in main loop
// ArduinoDmxN.attachTXInterrupt(my_tx_ISR) don't waste time updating values in main loop

//*********************************************************************************************************
//                        New DMX modes *** EXPERIMENTAL ***
//*********************************************************************************************************
#define    DMX512     (0)    // (250 kbaud - 2 to 512 channels) Standard USITT DMX-512
#define    DMX1024    (1)    // (500 kbaud - 2 to 1024 channels) Completely non standard - TESTED ok
#define    DMX2048    (2)    // (1000 kbaud - 2 to 2048 channels) called by manufacturers DMX1000K, DMX 4x or DMX 1M ???

void setup() 
{

  ArduinoDmx0.set_control_pin    (22);     // Arduino output pin for MAX485 input/output control (connect to MAX485 pins 2-3)
  ArduinoDmx0.set_rx_address     (1);      // set rx0 dmx start address
  ArduinoDmx0.set_rx_channels    (4);      // number of rx channels
  
  // *** NEW *** attach RX service routine here, fired when all channels in one universe are received.
  ArduinoDmx0.attachRXInterrupt  (frame_received); 
  //ArduinoDmx1.attachRXInterrupt  (frame_received);
  //ArduinoDmx2.attachRXInterrupt  (frame_received);
  //ArduinoDmx3.attachRXInterrupt  (frame_received);
  
  // *** NEW *** attach TX service routine here, fired when all channels in one universe are send.
  // ArduinoDmx0.attachTXInterrupt  (frame_send); 
  // ArduinoDmx1.attachTXInterrupt  (frame_send); 
  // ArduinoDmx2.attachTXInterrupt  (frame_send); 
  // ArduinoDmx3.attachTXInterrupt  (frame_send); 
  
  ArduinoDmx0.init_rx            (DMX512); // starts universe 0 as rx, *** NEW Parameter DMX mode ***
  
}  //end setup()

void loop()
{
  
  // YOUR CODE HERE
  
}  //end loop()

void frame_received(uint8_t universe) // Custom ISR: fired when all channels in one universe are received
{
  if (universe == 0) // USART0
  {
    //write values from dmx channels 1-4 universe 0 to arduino pwm pins 2-5
    analogWrite(2, ArduinoDmx0.RxBuffer[0]);  //buffers 0 indexed
    analogWrite(3, ArduinoDmx0.RxBuffer[1]);
    analogWrite(4, ArduinoDmx0.RxBuffer[2]);
    analogWrite(5, ArduinoDmx0.RxBuffer[3]);
  }
}  // end of ISR

/*
void frame_send(uint8_t universe) // Custom ISR: fired when all channels in one universe are send
{
  if (universe == 0) // USART0
  {
    // *****
  }
  else if (universe == 1) // USART1
  {
    // *****
  }
}  // end of ISR
*/