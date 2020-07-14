// uart-msp430g2553 exercises the hardware UART.
#include <msp430g2553.h>

#define TXLED BIT0	// red LED
#define RXLED BIT6	// green LED
#define TXD BIT2
#define RXD BIT1
#define BUTTON BIT3

void main(void){
	WDTCTL = WDTPW + WDTHOLD;	// disable watchdog timer
	
	BCSCTL1 = CALBC1_1MHZ;	// configure basic clock system for calibrated 1MHz clock operation
	DCOCTL = CALDCO_1MHZ;
	
	P2OUT = 0;		// setup port 2 to output 0
	P2DIR = 0xff;
	
	P1SEL |= RXD + TXD;	// these two lines select the secondary peripheral module function for tx and rx pins
	P1SEL2 |= RXD + TXD;
	P1OUT = 0;
	P1DIR = 0xff & ~(BUTTON + RXD);	// port 1: all outputs except for BUTTON and RXD
	
	/*
	 * UART mode is selected when the UCSYNC bit is cleared.
	 * Clearing UCSWRST releases the USCI for operation.
	 * The recommended USCI initialization/re-configuration process is:
	 * - Set UCSWRST
	 * - Initialize all USCI registers with UCSWRST = 1 (including UCAxCTL1)
	 * - Configure ports.
	 * - Clear UCSWRST via software
	 * - Enable interrupts (optional) via UCAxRXIE and/or UCAxTXIE
	 * 
	 * UCMSB should send least significant bit first.
	 * UC7BIT = 0, for 8 data bits
	 * UCPEN = 0, to disable parity
	 * UCMODEx should be configured for async and not idle-line mode (used in LIN)
	 * UCIREN = 0, to disable IRDA mode
	 * 
	 * A transmission is initiated by writing data to UCAxTXBUF.
	 * UCAxTXIFG is set when new data can be written into UCAxTXBUF
	 * 
	 * UCA0CTL0 PUC default is 8N1 which is OK.
	 * 
	 */
	 UCA0CTL1 |= UCSSEL_2;	// select sub-main clock for UART clock source
	 // 9600: SMCLK=1 MHz, UCOS16=1, UCBRx=6, UCBRSx=0, UCBRFx=8,
	 UCA0BR0 = 6;	// UCBRx is the sum of UCBR0 and UCBR1
	 UCA0BR1 = 0;
	 UCA0MCTL |= UCBRF_8 + UCBRS_0 + UCOS16;	// see setting above
	 
	 UCA0CTL1 &= ~UCSWRST;	// clear the software reset to activate UART
	 
	 while(1) {
	 	UCA0TXBUF = 'A';	// writing to tx buffer sets UCA0TXIFG low
	 	while((IFG2 & UCA0TXIFG) == 0) {}	// wait till character is sent
	 	UCA0TXBUF = 'B';
	 	while((IFG2 & UCA0TXIFG) == 0) {}	// wait till character is sent
	 	_delay_cycles(100000);
	 }
}
