#ifndef board_h
#define board_h


//                1 - VCC
// LOCK_UP        2 - PB0
// LOCK_DOWN      3 - PB1
//                4 - PB3/nRESET
//                5 - PB2
// SIGNAL_IN      6 - PA7/PCINT7
//                7 - PA6/SPI_MOSI/TXD

//                8 - PA5/SPI_MISO/RXD
//                9 - PA4/SPI_CLK
//               10 - PA3
// V_CAP_MEASURE 11 - PA2/ADC2
// POWER_LOSS    12 - PA1/PCINT1
//               13 - PA0
//               14 - GND

#define INTERVAL_TIME_MS    1000


extern volatile unsigned long milliseconds;
extern volatile unsigned long interval_timer;
extern volatile unsigned char interval_timer_signal;
extern volatile unsigned long interval_time_ms;
extern volatile unsigned char board_signal_in;
extern volatile unsigned char board_power_loss;

void board_init(void);

#endif /* board_h */
