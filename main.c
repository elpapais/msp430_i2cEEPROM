#include <msp430.h>
#include "i2c.h"
#include "serial.h"

void printf(char *, ...);


u8 txdata[] = {'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D'};
u8 rxdata[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void main(void)
{
    char c;
    char dev_address = 0x50;
    char reg_address = 0x00;
    int i;

    // Disable watchdog 
    WDTCTL = WDTPW + WDTHOLD;

    // Use 1 MHz DCO factory calibration
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
            
    // Setup the serial port
    // Serial out: P1.1 (BIT1)
    // Serial in:  P1.2 (BIT2)
    // Bit rate:   9600 (CPU freq / bit rate)  
    serial_setup (BIT1, BIT2, 1000000 / 9600);
    
    printf("rxdata: ");
		for (i=0;i<12;i++) {
		  printf("%i ",rxdata[i]);
		}
		printf("\r\n");

		i2c_init();
		__delay_cycles(20000);

		i2c_rx(11, 0xA0, rxdata, 0, 0);

    printf("rxdata: ");
		for (i=0;i<12;i++) {
		  printf("%i ",rxdata[i]);
		}
		printf("\r\n");

    i2c_tx(11, 0xA0, txdata, 0, 0); //i2c TX 11 bytes("HELLO WORLD")
    __delay_cycles(20000); //Allow EEPROM to write data

    i2c_rx(11, 0xA0, rxdata, 0, 0); //i2c RX data

    printf("rxdata: ");
		for (i=0;i<12;i++) {
		  printf("%i ",rxdata[i]);
		}
		printf("\r\n");



    for (;;) {                  // Do forever
        c = serial_getc ();     // Get a char
        serial_putc (c);        // Echo it back
    }
}

// I2C data transfer vector
#pragma vector = USCIAB0TX_VECTOR

__interrupt void USCIAB0TX_ISR(void) {
    if (i2c_int()) __bic_SR_register_on_exit(CPUOFF); //Exit LPM0;
}

// I2C status vector
#pragma vector = USCIAB0RX_VECTOR

__interrupt void USCIAB0RX_ISR(void) {
    if (i2c_eint()) {
        while (!(IFG2 & UCA0TXIFG)); // send error via UART
        UCA0TXBUF = '#';
    }
}

