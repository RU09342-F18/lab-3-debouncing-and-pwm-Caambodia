// Created: 10/03/18
// Last Edited: 10/05/18
// Author: Cameron Bendzynski

#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                       // stop watchdog timer
    UCSCTL4 = SELA_1;                               // ACLK (10kHz)

    P4DIR |= BIT7;                                  // Set P4.7 to output - Debug LED

    P1DIR |= BIT0;                                  // Set P1.0 to output - LED
    P1REN |= BIT1;                                  // Enable resistor on P1.1
    P1OUT |= BIT1;                                  // Set resistor to pull-up

    P1IE  |= BIT1;                                  // P1.1 Interrupt enabled
    P1IES |= BIT1;                                  // P1.1 Falling edge selected
    P1IFG &= ~BIT1;                                 // P1.1 Interrupt Flag cleared

    TA0CTL = TASSEL_1 + MC_1;                       // Set Timer A0 in Up Mode with divider, counter clear, and interrupt enabled
    TA0CCTL0 = CCIE;                                // Capture/Compare enable
    TA0CCR0 = 2000;                                 // Set Capture/Compare register to 2000

    __bis_SR_register(GIE);                         // Global Interrupt Enable
    while(1);                                       // Continuously runs program
}

int freqToPeriod(int freq)                          // Function for frequency to period conversion
{
    int period = 5000 / freq;                       // Turns a given frequency into the proper period

    return period;                                  // Returns period
}

#pragma vector = TIMER0_A0_VECTOR                   // Detects interrupt on Timer0
__interrupt void Timer_A0(void)
{
    P1IE |= BIT1;                                   // Re-enable the button interrupt
    P1IES |= BIT1;                                  // P1.1 Falling edge selected (just in case)
    TA0CTL = MC_0;                                  // Stop timer
    TA0CTL |= TACLR;                                // Clear timer
    P4OUT |= BIT7;                                  // Enable Debug LED on P4.7
    P1IFG &= ~BIT1;                                 // P1.1 Interrupt Flag cleared
}

#pragma vector = PORT1_VECTOR                       // Detects interrupt on Port 1
__interrupt void Port_1(void)
{
    P4OUT &= ~BIT7;                                 // Disable Debug LED on P4.7
    P1OUT ^= BIT0;                                  // Toggle LED on P1.0
    P1IE &= ~BIT1;                                  // Disable button interrupt on P1.1
    TA0CTL = TASSEL_1 + MC_1;                       // Enable Timer0 in Up Mode
}
