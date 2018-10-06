// Created: 10/03/18
// Last Edited: 10/05/18
// Author: Cameron Bendzynski

#include <msp430.h> 

void LEDSetup();                                    // prototyping functions
void ButtonSetup();
void TimerA0Setup();
void TimerA1Setup();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                       // stop watchdog timer
    BCSCTL3 = LFXT1S_2;                             // ACLK (12kHz)
    LEDSetup();                                     // Function for LED setup
    ButtonSetup();                                  // Function for button setup
    TimerA0Setup();                                 // Function for Timer0 setup
    TimerA1Setup();                                 // Function for Timer1 setup

    __bis_SR_register(GIE);                         // Global Interrupt Enable
    while(1);                                       // Continuously runs program
}

void LEDSetup()                                     // All initial settings for LED use
{
    P1DIR |= BIT6;                                  // Set P1.6 to output - Debug LED
    P1DIR |= BIT0;                                  // Set P1.0 to output - LED
}

void ButtonSetup()                                  // All initial settings for button use
{
    P1REN |= BIT3;                                  // Enable resistor on P1.3
    P1OUT |= BIT3;                                  // Set resistor to pull-up
    P1IE  |= BIT3;                                  // P1.3 Interrupt enabled
    P1IES |= BIT3;                                  // P1.3 Falling edge selected
    P1IFG &= ~BIT3;                                 // P1.3 Interrupt Flag cleared
}

void TimerA0Setup()                                 // All initial settings for TimerA0
{
    TA0CTL = TASSEL_1 + MC_1;                       // Set Timer A0 in Up Mode with divider, counter clear, and interrupt enabled
    TA0CCTL0 = CCIE;                                // Capture/Compare enable
    TA0CCR0 = 2000;                                 // Set Capture/Compare register to 2000
}

void TimerA1Setup()                                 // All initial settings for TimerA1
{
    TA1CTL = TASSEL_1 + MC_1;                       // Set Timer A0 in Up Mode with divider, counter clear, and interrupt enabled
    TA1CCTL1 |= CCIE;                               // Capture/Compare enable on Timer1 CCR1
    TA1CCTL0 |= CCIE;                               // Capture/Compare enable on Timer1 CCR0
    TA1CCR0 = 10001;                                // Set Capture/Compare register to 2000
    TA1CCR1 = 5000;
}

#pragma vector = TIMER0_A0_VECTOR                   // Detects interrupt on Timer0
__interrupt void debounce_timer(void)
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
    P1IE &= ~BIT3;                                  // Disable button interrupt on P1.3
    TA0CTL = TASSEL_1 + MC_1;                       // Enable Timer0 in Up Mode

    if (TA1CCR1 < (TA1CCR0 - 1))                    // Increase the duty cycle on button press
        TA1CCR1 += 1000;                            // Increase by 1000
    else
        TA1CCR1 = 0;                                // Reset back to 0
}

#pragma vector = TIMER1_A0_VECTOR                   // Detects interrupt for CCR0 on Timer1
__interrupt void Timer_A10(void)
{
    if (TA1CCR1 != 0)                               // If statement to avoid dim blinking when CCR1 = 0
        P1OUT |= BIT0;                              // Turns the LED on
}

#pragma vector = TIMER1_A1_VECTOR                   // Detects interrupt for CCR1 on Timer1
__interrupt void Timer_A11(void)
{
    if (TA1IV == TA1IV_TACCR1)                      // Checks the interrupt vector to determine if CCR1 was triggered
        P1OUT &= ~BIT0;                             // Turns the LED off
}
