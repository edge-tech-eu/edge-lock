#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include "board.h"
#include "uart.h"
#include "build.h"
#include "version.h"

#include <util/delay.h>

int main(void) {
    
    unsigned char reset_source = MCUSR;   // save reset source
    MCUSR = 0x00;  // cleared for next reset detection
    
    // wdt_enable(WDTO_2S);
    
    board_init();
    
    uart_init();

    uart_tx_str("lock test:\r\n");
    
    if(reset_source&0x08) {
        uart_tx_str("W");
    }
    
    while(1) {
        
        //wdt_reset();
        
        if(interval_timer_signal) {
            
            interval_timer_signal=0;
            
            sprintf(str, "%d\r\n", (int)board_measure);
            uart_tx_str(str);
        }
        
        if(board_signal_in) {
            
            board_signal_in = 0;
            
            uart_tx_str("signal_in\r\n");
        }
        
        if(board_power_loss) {
            
            board_power_loss = 0;
            
            uart_tx_str("power_loss\r\n");
        }
    }
}
