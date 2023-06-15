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
#include "lib_dmx.h"

#if defined(USE_UART0)
  CArduinoDmx ArduinoDmx0(0);
#endif
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if defined(USE_UART1)
    CArduinoDmx ArduinoDmx1(1);
  #endif
  #if defined(USE_UART2)
    CArduinoDmx ArduinoDmx2(2);
  #endif
  #if defined(USE_UART3)
    CArduinoDmx ArduinoDmx3(3);
  #endif
#endif

// *************** DMX Transmision Initialisation ****************
void CArduinoDmx::init_tx(uint8_t mode)
{
  cli();          //disable interrupts
  stop_dmx();                         //stop uart
  dmx_mode = mode;
  set_speed(dmx_mode);
  
  if(control_pin != -1)
  {
    pinMode(control_pin,OUTPUT);        // max485 I/O control
    digitalWrite(control_pin, HIGH);    // set 485 as output
  }
  
  if(mUART == 0)
  {
    pinMode(1, OUTPUT);
    UBRR0H   = 0;
    UBRR0L   = speed_dmx;  
    UCSR0A  |= (1<<U2X0);
    UCSR0C  |= (3<<UCSZ00)|(1<<USBS0);
    UCSR0B  |= (1<<TXEN0) |(1<<TXCIE0);
    UDR0     = 0;							          //start USART 0
  }
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  else if(mUART == 1)
  {
    pinMode(18, OUTPUT); 
    UBRR1H   = 0;
    UBRR1L   = speed_dmx;   
    UCSR1A  |= (1<<U2X1);
    UCSR1C  |= (3<<UCSZ10)|(1<<USBS1);
    UCSR1B  |= (1<<TXEN1) |(1<<TXCIE1);
    UDR1     = 0;							          //start USART 1
  }
  else if(mUART == 2)
  {
    pinMode(16, OUTPUT); 
    UBRR2H   = 0;
    UBRR2L   = speed_dmx;   
    UCSR2A  |= (1<<U2X2);
    UCSR2C  |= (3<<UCSZ20)|(1<<USBS2);
    UCSR2B  |= (1<<TXEN2) |(1<<TXCIE2);
    UDR2     = 0;							          //start USART 2
  }
  else if(mUART == 3)
  {
    pinMode(14, OUTPUT); 
    UBRR3H   = 0;
    UBRR3L   = speed_dmx;    
    UCSR3A  |= (1<<U2X3);
    UCSR3C  |= (3<<UCSZ30)|(1<<USBS3);
    UCSR3B  |= (1<<TXEN3) |(1<<TXCIE3);
    UDR3     = 0;							          //start USART 3
  }
#endif

  gTxState = BREAK;					                    // start with break
  TxBuffer = (uint8_t*)malloc(tx_channels);     // allocate mem for buffer
  memset((uint8_t*)TxBuffer, 0, tx_channels);   // fill buffer with 0's
  sei();          //enable interrupts
}

// ************************ DMX Stop ***************************
void CArduinoDmx::stop_dmx()
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if(mUART == 0)
  {
    UCSR0B &= ~((1<<RXCIE0) | (1<<TXCIE0) | (1<<RXEN0) | (1<<TXEN0));
  }
  else if(mUART == 1)
  {
    UCSR1B &= ~((1<<RXCIE1) | (1<<TXCIE1) | (1<<RXEN1) | (1<<TXEN1));
  }
  else if(mUART == 2)
  {
    UCSR2B &= ~((1<<RXCIE2) | (1<<TXCIE2) | (1<<RXEN2) | (1<<TXEN2));
  }
  else if(mUART == 3)
  {
    UCSR3B &= ~((1<<RXCIE3) | (1<<TXCIE3) | (1<<RXEN3) | (1<<TXEN3));
  }
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  if(mUART == 0)
  {
    UCSR0B &= ~((1<<RXCIE0) | (1<<TXCIE0) | (1<<RXEN0) | (1<<TXEN0));
  }
#endif
}

// *************** DMX Reception Initialisation ****************
void CArduinoDmx::init_rx(uint8_t mode)
{
  cli();          //disable interrupts
  stop_dmx();
  dmx_mode = mode;
  set_speed(dmx_mode);
  
  if(control_pin != -1)
  {
    pinMode(control_pin,OUTPUT);        //max485 I/O control
    digitalWrite(control_pin, LOW);     //set 485 as input
  }
  
  if(mUART == 0)
  {
    pinMode(0, INPUT); 
    UBRR0H   = 0;
    UBRR0L   = speed_dmx;
    UCSR0A  |= (1<<U2X0);
    UCSR0C  |= (3<<UCSZ00)|(1<<USBS0);
    UCSR0B  |= (1<<RXEN0) |(1<<RXCIE0);
  }
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  else if(mUART == 1)
  {
    pinMode(19, INPUT); 
    UBRR1H   = 0;
    UBRR1L   = speed_dmx;
    UCSR1A  |= (1<<U2X1);
    UCSR1C  |= (3<<UCSZ10)|(1<<USBS1);
    UCSR1B  |= (1<<RXEN1) |(1<<RXCIE1);
  }
  else if(mUART == 2)
  {
    pinMode(17, INPUT); 
    UBRR2H   = 0;
    UBRR2L   = speed_dmx;
    UCSR2A  |= (1<<U2X2); 
    UCSR2C  |= (3<<UCSZ20)|(1<<USBS2);
    UCSR2B  |= (1<<RXEN2) |(1<<RXCIE2);
  }
  else if(mUART == 3)
  {
    pinMode(15, INPUT); 
    UBRR3H   = 0;
    UBRR3L   = speed_dmx; 
    UCSR3A  |= (1<<U2X3);
    UCSR3C  |= (3<<UCSZ30)|(1<<USBS3);
    UCSR3B  |= (1<<RXEN3) |(1<<RXCIE3);
  }
#endif
  
  gRxState = IDLE;
  RxBuffer = (uint8_t*)malloc(rx_channels);   // allocate mem for buffer
  memset((uint8_t*)RxBuffer, 0, rx_channels); // fill buffer with 0's
  sei();          //enable interrupts
}

// *************** DMX Reception ISR ****************
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if defined(USE_UART0)
    ISR (USART0_RX_vect)
    {
      ArduinoDmx0.Process_ISR_RX(0);
    }
  #endif
  #if defined(USE_UART1)
    ISR (USART1_RX_vect)
    {
      ArduinoDmx1.Process_ISR_RX(1);
    }
  #endif
  #if defined(USE_UART2)
    ISR (USART2_RX_vect)
    {
      ArduinoDmx2.Process_ISR_RX(2);
    }
  #endif
  #if defined(USE_UART3)
    ISR (USART3_RX_vect)
    {
      ArduinoDmx3.Process_ISR_RX(3);
    }
  #endif
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  #if defined(USE_UART0)
    ISR (USART_RX_vect)
    {
      ArduinoDmx0.Process_ISR_RX(0);
    }  
  #endif
#endif

void CArduinoDmx::Process_ISR_RX(uint8_t rx_isr_number)
{
  if(rx_isr_number == 0)
  {
    USARTstate = UCSR0A;		          //get state
    RxByte     = UDR0;                //get data
    RxState    = gRxState;					  //just get once from SRAM!!!
    if (USARTstate &(1<<FE0))         //check for break
    {					
      UCSR0A  &= ~(1<<FE0);				    //reset flag
      RxCount  = rx_address;				  //reset frame counter
      gRxState = BREAK;
    }
  }
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

  else if(rx_isr_number == 1)
  {
    USARTstate = UCSR1A;		          //get state
    RxByte     = UDR1;                //get data
    RxState    = gRxState;					  //just get once from SRAM!!!
    if (USARTstate &(1<<FE1))         //check for break
    {					
      UCSR1A  &= ~(1<<FE1);				    //reset flag
      RxCount  = rx_address;				  //reset frame counter
      gRxState = BREAK;
    }
  }
  else if(rx_isr_number == 2)
  {
    USARTstate = UCSR2A;		          //get state
    RxByte     = UDR2;                //get data
    RxState    = gRxState;					  //just get once from SRAM!!!
    if (USARTstate &(1<<FE2))         //check for break
    {					
      UCSR2A  &= ~(1<<FE2);				    //reset flag
      RxCount  = rx_address;				  //reset frame counter
      gRxState = BREAK;
    }
  }
  else if(rx_isr_number == 3)
  {
    USARTstate = UCSR3A;		          //get state
    RxByte     = UDR3;                //get data
    RxState    = gRxState;					  //just get once from SRAM!!!
    if (USARTstate &(1<<FE3))         //check for break
    {					
      UCSR3A  &= ~(1<<FE3);				    //reset flag
      RxCount  = rx_address;				  //reset frame counter
      gRxState = BREAK;
    }
  }
#endif

  if (RxState == BREAK)
  {
    if (RxByte == 0) 
    {
      gRxState = STARTB;						  //normal start code detected
      gRxPnt   = ((uint8_t*)RxBuffer + 1);
    }
    else 
      gRxState = IDLE;
  }
  else if (RxState == STARTB)
  {
    if (--RxCount == 0)						    //start address reached?
    {
      gRxState   = STARTADR;
      RxBuffer[0]= RxByte;
    }
  }
  else if (RxState == STARTADR)
  {
    RxPnt  = gRxPnt;
    *RxPnt = RxByte;
    if (++RxPnt >= (RxBuffer + rx_channels)) 	//all ch received?
    {
      gRxState= IDLE;
      if (*RXisrCallback) RXisrCallback(mUART);   // fire callback for read data
    }
    else 
    {
      gRxPnt = RxPnt;
    }
  }							
}

// *************** DMX Transmision ISR ****************
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #if defined(USE_UART0)
    ISR(USART0_TX_vect)
    {
      ArduinoDmx0.Process_ISR_TX(0);
    }
  #endif
  #if defined(USE_UART1)
    ISR(USART1_TX_vect)
    {
      ArduinoDmx1.Process_ISR_TX(1);
    }
  #endif
  #if defined(USE_UART2)
    ISR(USART2_TX_vect)
    {
      ArduinoDmx2.Process_ISR_TX(2);
    }
  #endif
  #if defined(USE_UART3)
    ISR(USART3_TX_vect)
    {
      ArduinoDmx3.Process_ISR_TX(3);
    }
  #endif
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  #if defined(USE_UART0)
    ISR(USART_TX_vect)
    {
      ArduinoDmx0.Process_ISR_TX(0);
    }
  #endif
#endif


void CArduinoDmx::Process_ISR_TX(uint8_t tx_isr_number)
{
  TxState = gTxState;
  
  if(tx_isr_number == 0)
  {
    if (TxState == TXBREAK) //BREAK + MAB
    {
      UBRR0H   = 0;
      UBRR0L   = speed_break;
      UDR0     = 0;								    //send break
      gTxState = TXSTARTB;
    }
    else if (TxState == TXSTARTB)
    {
      UBRR0H   = 0;
      UBRR0L   = speed_dmx;
      UDR0     = 0;								    //send start byte
      gTxState = TXDATA;
      gCurTxCh = 0;
    }
    else
    {
      #if defined(USE_INTERBYTE_DELAY)
        delay_gap();
      #endif    
      CurTxCh = gCurTxCh;
      UDR0 = TxBuffer[CurTxCh++];				//send data
      if (CurTxCh == tx_channels)
      {
        if (*TXisrCallback) TXisrCallback(0); // fire callback for update data
        gTxState = TXBREAK;   // new break if all ch sent
      }
      else 
      {
        gCurTxCh = CurTxCh;
      }
    }
  }
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

  else if(tx_isr_number == 1)
  {
    if (TxState == TXBREAK)
    {
      UBRR1H   = 0;
      UBRR1L   = speed_break;
      UDR1     = 0;								    //send break
      gTxState = TXSTARTB;
    }
    else if (TxState == TXSTARTB)
    {
      UBRR1H   = 0;
      UBRR1L   = speed_dmx;
      UDR1     = 0;								    //send start byte
      gTxState = TXDATA;
      gCurTxCh = 0;
    }
    else
    {
      #if defined(USE_INTERBYTE_DELAY)
        delay_gap();
      #endif    
      CurTxCh = gCurTxCh;
      UDR1 = TxBuffer[CurTxCh++];				//send data
      if (CurTxCh == tx_channels)
      {
        if (*TXisrCallback) TXisrCallback(1); // fire callback for update data
        gTxState = TXBREAK;   // new break if all ch sent
      }
      else 
      {
        gCurTxCh = CurTxCh;
      }
    }
  }
  else if(tx_isr_number == 2)
  {
    if (TxState == TXBREAK)
    {
      UBRR2H   = 0;
      UBRR2L   = speed_break;
      UDR2     = 0;								    //send break
      gTxState = TXSTARTB;
    }
    else if (TxState == TXSTARTB)
    {
      UBRR2H   = 0;
      UBRR2L   = speed_dmx;
      UDR2     = 0;								    //send start byte
      gTxState = TXDATA;
      gCurTxCh = 0;
    }
    else
    {
      #if defined(USE_INTERBYTE_DELAY)
        delay_gap();
      #endif   
      CurTxCh = gCurTxCh;
      UDR2 = TxBuffer[CurTxCh++];				//send data
      if (CurTxCh == tx_channels)
      {
        if (*TXisrCallback) TXisrCallback(2); // fire callback for update data
        gTxState = TXBREAK;   // new break if all ch sent
      }
      else 
      {
        gCurTxCh = CurTxCh;
      }
    }
  }
  else if(tx_isr_number == 3)
  {
    if (TxState == TXBREAK)
    {
      UBRR3H   = 0;
      UBRR3L   = speed_break;
      UDR3     = 0;								    //send break
      gTxState = TXSTARTB;
    }
    else if (TxState == TXSTARTB)
    {
      UBRR3H   = 0;
      UBRR3L   = speed_dmx;
      UDR3     = 0;								    //send start byte
      gTxState = TXDATA;
      gCurTxCh = 0;
    }
    else
    {
      #if defined(USE_INTERBYTE_DELAY)
        delay_gap();
      #endif
      CurTxCh = gCurTxCh;
      UDR3 = TxBuffer[CurTxCh++];				//send data
      if (CurTxCh == tx_channels)
      {
        if (*TXisrCallback) TXisrCallback(3); // fire callback for update data
        gTxState = TXBREAK;   // new break if all ch sent
      }
      else 
      {
        gCurTxCh = CurTxCh;
      }
    }
  }
#endif
}

void CArduinoDmx::set_speed(uint8_t mode)
{
  if(mode == 0)
  {
    speed_dmx   = DMX_512;    // DMX-512  (250 kbaud - 512 channels) Standard USITT DMX-512
    speed_break = BREAK_512;
  }
  else if(mode == 1)
  {
    speed_dmx   = DMX_1024;	  // DMX-1024 (500 kbaud - 1024 channels) Completely non standard, but usefull ;)
    speed_break = BREAK_1024;
  }
  else if(mode == 2)
  {
    speed_dmx   = DMX_2048;   // DMX-2048 (1000 kbaud - 2048 channels) Used by manufacturers as DMX1000K, DMX-4x or DMX-1M ???
    speed_break = BREAK_2048;
  }  
}

#if defined(USE_INTERBYTE_DELAY)

void CArduinoDmx::delay_gap() // rare cases of equipment non full DMX-512 compliant, need this
{
  if(dmx_mode == 0)
  {
    _delay_us(IBG_512);
  }
  else if(dmx_mode == 1)
  {
    _delay_us(IBG_1024);
  }
  else if(dmx_mode == 2)
  {
    _delay_us(IBG_2048);
  }
}
#endif



