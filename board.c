#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "board.h"
#include "uart.h"

volatile unsigned long milliseconds;
volatile unsigned long interval_timer;
volatile unsigned char interval_timer_signal;
volatile unsigned char board_signal_in;
volatile unsigned char board_power_loss;
volatile unsigned short board_measure;

// uart: timer0 compare A match interrupt
ISR(TIM0_COMPA_vect ) {
    
   unsigned short local_tx_shift_reg = tx_shift_reg;
   //output LSB of the TX shift register at the TX pin
   if( local_tx_shift_reg & 0x01 ) {
       PORTA |= (1<<PA6);
   } else {
       PORTA &=~ (1<<PA6);
   }
   //shift the TX shift register one bit to the right
   local_tx_shift_reg >>= 1;
   tx_shift_reg = local_tx_shift_reg;
   //if the stop bit has been sent, the shift register will be 0
   //and the transmission is completed, so we can stop & reset timer0
   if(!local_tx_shift_reg) {
      TCCR0B = 0;
      TCNT0 = 0;
   }
}


// pin change interrupt:
// SIGNAL_IN: PA7/PCINT7
// POWER_LOSS: PA1/PCINT1

ISR(PCINT0_vect) {
  
    unsigned char p = PINA;
    
    if(!(p & (1<<PA7))) {
        board_signal_in = 1;
    }
    
    if(!(p & (1<<PA1))) {
        board_power_loss = 1;
    }
}


// timer compare interrupt
ISR(TIM1_COMPA_vect) {
    
    milliseconds++;
    
    if(!interval_timer--) {
        
        interval_timer=INTERVAL_TIME_MS;
        interval_timer_signal=1;
    }
}


ISR(ADC_vect) {
    
    board_measure = ADC;
}



void board_init(void) {
    
    cli();
    
    // initialize timer stuff
    // ms since boot
    milliseconds = 0;
    // interval timer
    interval_timer = INTERVAL_TIME_MS;
    interval_timer_signal = 0;

    
    // using timer 1 (16-bit) count to 100 = 0.1ms, 1000 = 1ms
     OCR1A  = 1000;             // number to count up to
    // WGM12 = 1 <-- Clear Timer on Compare Match (CTC)
    // CS11 = 1 <-- clk/8
    TCCR1B = (1<<WGM12)|(1<<CS11);
    
    TIMSK1 = (1<<OCIE1A);   // TC0 compare match A interrupt enable
    TIFR1 |= (1<<OCF1A);
    
    // pin-change interrupts:
    
    GIMSK |= (1<<PCIE0);       // enable PC interrupt 0
    PCMSK0 |= (1<<PCINT7);    // enable PCINT7 for PA7/SIGNAL_IN
    PCMSK0 |= (1<<PCINT1);    // enable PCINT1 for PA1/POWER_LOSS
    GIFR |= (1<<PCIF0);       // clear interrupt flag
    
    board_signal_in = 0;
    board_power_loss = 0;
    
    // should not need to do
    PORTA |= (1<<PA7);
    
    // serial output high
    //PORTA |= (1<<PA6);
    //DDRA |= (1<<PA6);
    
    /* UART */
    
    //set TX pin as output
    PORTA |= (1<<PA6);
    DDRA |= (1<<PA6);
   
   //set timer0 to CTC mode
   TCCR0A = (1<<WGM01);
   //enable output compare 0 A interrupt
   TIMSK0 |= (1<<OCF0A);
    
   // BAUD = F_CPU / ( TIMER0_PRESCALER * (OCR0A + 1)
   //set compare value to 103 to achieve a 9600 baud rate (i.e. 104µs)
   //together with the 8MHz/8=1MHz timer0 clock
   /*NOTE: since the internal 8MHz oscillator is not very accurate, this value can be tuned
     to achieve the desired baud rate, so if it doesn't work with the nominal value (103), try
     increasing or decreasing the value by 1 or 2 */
   //OCR0A = 103;
    OCR0A = 103;
    
    // lock
    PORTB &= ~(1<<PB0);
    PORTB &= ~(1<<PB1);
    DDRB |= (1<<PB0);
    DDRB |= (1<<PB1);
    
    // measure
    // free running mode, /128, pa7 = adc1
    ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADIE);
    ADMUX = (1<<MUX0);
    
    // start conversion: ADCSRA |= (1<<ADSC)
    // enable ints
    sei();
}

void lock_up() {
    
    PORTB |= (1<<PB0);
    PORTB &= ~(1<<PB1);
}

void lock_down() {
    
    PORTB &= ~(1<<PB0);
    PORTB |= (1<<PB1);
}

void lock_release() {
    
    PORTB &= ~(1<<PB0);
    PORTB &= ~(1<<PB1);
}
