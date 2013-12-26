#include "msp430.h"
#include "std.h"
#include "fs.h"


void main(void) {
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	P1DIR |= BIT0 + BIT6; // Set the LEDs on P1.0, P1.6 as outputs
	P1OUT = 0;

	/* Configure hardware UART */
	P1SEL = BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2;                    // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
	UCA0BR0 = 0x03;                           // 32kHz/9600 = 3.41
	UCA0BR1 = 0x00;
	UCA0MCTL = UCBRS1 + UCBRS0;               // Modulation UCBRSx = 3
	UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
	IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

	String sh = newString("/bin/sh", 7);
	((Exec*) fopen(&sh))->content(NULL);
}
