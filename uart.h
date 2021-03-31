//
//  uart.h
//  edge-lock
//
//  Created by Frank Boddeke on 29/03/2021.
//

#ifndef uart_h
#define uart_h

extern volatile unsigned short tx_shift_reg;

void uart_init(void);
void uart_tx_str(char* string);
void uart_tx(char character);

#endif /* uart_h */
