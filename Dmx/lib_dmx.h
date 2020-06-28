/***************************************************************************
*
* Title          : Arduino DMX512 library. 4 input/output universes.
* Version        : v 0.3 beta
* Last updated   : 07.07.2012
* Target         : Arduino mega 2560, Arduino mega 1280, Arduino nano (1 universe)  
* Author         : Toni Merino - merino.toni at gmail.com
* Web            : www.deskontrol.net/blog
*
* Based on ATmega8515 Dmx library written by Hendrik Hoelscher, www.hoelscher-hi.de
***************************************************************************
 This program is free software; you can redistribute it and/or 
 modify it under the terms of the GNU General Public License 
 as published by the Free Software Foundation; either version2 of 
 the License, or (at your option) any later version. 

 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 General Public License for more details. 

 If you have no copy of the GNU General Public License, write to the 
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 

 For other license models, please contact the author.

;***************************************************************************/
#ifndef __INC_DMX_H
#define __INC_DMX_H

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#if ARDUINO >= 100
  #include "Arduino.h"
#else
   #include "WProgram.h"
#endif

//#define        USE_INTERBYTE_DELAY     // rare cases of equipment non full DMX-512 compliant, need this

// *** comment UARTs not used ***
#define        USE_UART0
//#define        USE_UART1
//#define        USE_UART2
//#define        USE_UART3

// New DMX modes *** EXPERIMENTAL ***
#define        DMX512            (0)    // DMX-512 (250 kbaud - 512 channels) Standard USITT DMX-512
#define        DMX1024           (1)    // DMX-1024 (500 kbaud - 1024 channels) Completely non standard - TESTED ok
#define        DMX2048           (2)    // DMX-2048 (1000 kbaud - 2048 channels) called by manufacturers DMX1000K, DMX 4x or DMX 1M ???

// DMX-512  (250 kbaud - 512 channels) Standard USITT DMX-512
#define        IBG_512           (10)                      // interbyte gap [us]
#define        DMX_512           ((F_CPU/(250000*8))-1)    // 250 kbaud
#define        BREAK_512         ( F_CPU/(100000*8))       // 90.9 kbaud

// DMX-1024 (500 kbaud - 1024 channels) Completely non standard
#define        IBG_1024          (5)                       // interbyte gap [us]
#define        DMX_1024          ((F_CPU/(500000*8))-1)    // 500 kbaud
#define        BREAK_1024        ( F_CPU/(200000*8))       // 181.8 kbaud

// DMX-2048 (1000 kbaud - 2048 channels) Non standard, but used by manufacturers as DMX1000K or DMX-4x or DMX 1M ???
#define        IBG_2048          (2)                       // interbyte gap [us] + nop's to reach 2.5 uS
#define        DMX_2048          ((F_CPU/(1000000*8))-1)   // 1000 kbaud
#define        BREAK_2048        ( F_CPU/(400000*8))       // 363.6 kbaud

// Inline assembly: do nothing for one clock cycle.
#define        nop()             __asm__ __volatile__("nop")

#ifdef __cplusplus
extern "C" { 
#endif
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    #if defined(USE_UART0)
      void SIG_USART0_RECV  (void) __attribute__((__always_inline__));
      void SIG_USART0_TRANS (void) __attribute__((__always_inline__));
    #endif
    #if defined(USE_UART1)
      void SIG_USART1_RECV  (void) __attribute__((__always_inline__));
      void SIG_USART1_TRANS (void) __attribute__((__always_inline__));
    #endif
    #if defined(USE_UART2)  
      void SIG_USART2_RECV  (void) __attribute__((__always_inline__));
      void SIG_USART2_TRANS (void) __attribute__((__always_inline__));
    #endif
    #if defined(USE_UART3)  
      void SIG_USART3_RECV  (void) __attribute__((__always_inline__));
      void SIG_USART3_TRANS (void) __attribute__((__always_inline__));
    #endif
  #elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    #if defined(USE_UART0)
      void USART_RX_vect    (void) __attribute__((__always_inline__));
      void USART_TX_vect    (void) __attribute__((__always_inline__));
    #endif
  #endif
#ifdef __cplusplus
};
#endif

class CArduinoDmx 
{ 
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    #if defined(USE_UART0)
      friend void SIG_USART0_RECV  (void);
      friend void SIG_USART0_TRANS (void);
    #endif
    #if defined(USE_UART1)
      friend void SIG_USART1_RECV  (void);
      friend void SIG_USART1_TRANS (void);
    #endif
    #if defined(USE_UART2)  
      friend void SIG_USART2_RECV  (void);
      friend void SIG_USART2_TRANS (void);
    #endif
    #if defined(USE_UART3)  
      friend void SIG_USART3_RECV  (void);
      friend void SIG_USART3_TRANS (void);
    #endif
  #elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    #if defined(USE_UART0)
      friend void USART_RX_vect    (void);
      friend void USART_TX_vect    (void);
    #endif
  #endif
  
public:
   enum {IDLE, BREAK, STARTB, STARTADR};     // RX DMX states
   enum {TXBREAK, TXSTARTB, TXDATA};         // TX DMX states
  
   volatile uint8_t    *RxBuffer;            // array of RX DMX values
   volatile uint8_t    *TxBuffer;            // array of TX DMX values

private:
   uint8_t     gRxState;
   uint8_t    *gRxPnt;
   uint8_t     IndicatorCount;
   uint8_t     USARTstate;		
   uint8_t     RxByte;     
   uint8_t     RxState;
   uint8_t     mUART;
   uint8_t     gTxState;
   uint16_t    RxCount;
   uint16_t    gCurTxCh;		
   uint16_t    rx_channels;                  // rx channels number
   uint16_t    tx_channels;                  // tx channels number
   uint16_t    rx_address;                   // rx start address
   uint16_t    tx_address;                   // tx start address
   int8_t      rx_led;                       // rx indicator led pin
   int8_t      tx_led;                       // tx indicator led pin
   int8_t      control_pin;                  // max485 input/output selection pin
   uint8_t     dmx_mode;                     // Standard USITT DMX512 = 0, non standard DMX1024 = 1, non standard DMX2048 (DMX1000K) = 2
   uint8_t     speed_dmx;
   uint8_t     speed_break;
   uint16_t    CurTxCh;
   uint8_t     TxState;
   uint8_t    *RxPnt;
   
#if defined(USE_INTERBYTE_DELAY)   
   void        delay_gap          ();
#endif

public:
   void        stop_dmx           ();
   void        set_speed          (uint8_t mode);
   void        set_control_pin    (int8_t  pin)        { control_pin     = pin;      }
   void        init_rx            (uint8_t mode);  // Standard USITT DMX512 = 0, non standard DMX1024 = 1, non standard DMX2048 (DMX1000K) = 2
   void        set_rx_address     (uint16_t address)   { rx_address      = address;  }
   void        set_rx_channels    (uint16_t channels)  { rx_channels     = channels; }
   void        init_tx            (uint8_t mode);  // Standard USITT DMX512 = 0, non standard DMX1024 = 1, non standard DMX2048 (DMX1000K) = 2
   void        set_tx_address     (uint16_t address)   { tx_address      = address;  }
   void        set_tx_channels    (uint16_t channels)  { tx_channels     = channels; }

   void        attachTXInterrupt  (void (*isr)(uint8_t uart))      { TXisrCallback   = isr; }   // register the user TX callback
   void        attachRXInterrupt  (void (*isr)(uint8_t uart))      { RXisrCallback   = isr; }   // register the user RX callback

   void        (*TXisrCallback)   (uint8_t uart);
   void        (*RXisrCallback)   (uint8_t uart);

   inline void Process_ISR_RX     (uint8_t  rx_isr_number);
   inline void Process_ISR_TX     (uint8_t  tx_isr_number);
  
public:
   CArduinoDmx                    (uint8_t uart)       { rx_address      = 1; 
                                                         rx_channels     = 8;
                                                         tx_address      = 1; 
                                                         tx_channels     = 8;
                                                         mUART           = uart; }
 
};

#if defined(USE_UART0)
  extern CArduinoDmx ArduinoDmx0;
#endif
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if defined(USE_UART1)
    extern CArduinoDmx ArduinoDmx1;
  #endif
  #if defined(USE_UART2)
    extern CArduinoDmx ArduinoDmx2;
  #endif
  #if defined(USE_UART3)
    extern CArduinoDmx ArduinoDmx3;
  #endif
#endif

#endif
