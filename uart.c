//
//  uart.c
//  edge-lock
//
//  Created by Frank Boddeke on 29/03/2021.
//
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

volatile unsigned short tx_shift_reg;


void uart_tx(char character) {
    
    unsigned short local_tx_shift_reg = tx_shift_reg;
   
    // wait till previous is finished
    while(tx_shift_reg);
    
   //fill the TX shift register witch the character to be sent and the start & stop bits (start bit (1<<0) is already 0)
   local_tx_shift_reg = (character<<1) | (1<<9); //stop bit (1<<9)
   
    tx_shift_reg = local_tx_shift_reg;
    
   //start timer0 with a prescaler of 8
   TCCR0B = (1<<CS01);
}

void uart_tx_str(char* string) {
    
    while( *string ) {
        
        uart_tx( *string++ );
    }
}

void uart_init() {
    
    tx_shift_reg = 0;
}

