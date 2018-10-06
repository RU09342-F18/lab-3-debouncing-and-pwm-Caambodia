// Created: 10/03/18
// Last Edited: 10/05/18
// Author: Cameron Bendzynski

#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                       // stop watchdog timer
    BCSCTL3 = LFXT1S_2;                             // ACLK (12kHz)

    P1DIR |= BIT6;                                  // Set P1.6 to output - Debug LED

    P1DIR |= BIT0;                                  // Set P1.0 to output - LED
    P1REN |= BIT3;                                  // Enable resistor on P1.3
    P1OUT |= BIT3;                                  // Set resistor to pull-up

    P1IE  |= BIT3;                                  // P1.3 Interrupt enabled
    P1IES |= BIT3;                                  // P1.3 Falling edge selected
    P1IFG &= ~BIT3;                                 // P1.3 Interrupt Flag cleared

    TA0CTL = TASSEL_1 + MC_1;                       // Set Timer A0 in Up Mode with divider, counter clear, and interrupt enabled
    TA0CCTL0 = CCIE;                                // Capture/Compare enable
    TA0CCR0 = 2000;                                 // Set Capture/Compare register to 2000

    __bis_SR_register(GIE);                         // Global Interrupt Enable
    while(1);                                       // Continuously runs program
}

int freqToPeriod(int freq)                          // Function for frequency to period conversion
{
    int period = 6000 / freq;                       // Turns a given frequency into the proper period

    return period;                                  // Returns period
}

#pragma vector = TIMER0_A0_VECTOR                   // Detects interrupt on Timer0
__interrupt void Timer_A0(void)
{
    P1IE |= BIT3;                                   // Re-enable the button interrupt
    P1IES |= BIT3;                                  // P1.3 Falling edge selected (just in case)
    TA0CTL = MC_0;                                  // Stop timer
    TA0CTL |= TACLR;                                // Clear timer
    P1OUT |= BIT6;                                  // Enable Debug LED on P1.6
    P1IFG &= ~BIT3;                                 // P1.3 Interrupt Flag cleared
}

#pragma vector = PORT1_VECTOR                       // Detects interrupt on Port 1
__interrupt void Port_1(void)
{
    P1OUT &= ~BIT6;                                 // Disable Debug LED on P1.6
    P1OUT ^= BIT0;                                  // Toggle LED on P1.0
    P1IE &= ~BIT3;                                  // Disable button interrupt on P1.3
    TA0CTL = TASSEL_1 + MC_1;                       // Enable Timer0 in Up Mode
}
