// Created: 10/03/18
// Last Edited: 10/05/18
// Author: Cameron Bendzynski

#include <msp430g2553.h>

void LEDSetup();                                    // prototyping functions
void ButtonSetup();
void TimerA0Setup();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                       // stop watchdog timer
    LEDSetup();                                     // Function for LED setup
    ButtonSetup();                                  // Function for button setup
    TimerA0Setup();                                 // Function for Timer0 setup

    __bis_SR_register(GIE);                         // Global Interrupt Enable
    while(1);                                       // Continuously runs program
}

void LEDSetup()                                     // All initial settings for LED use
{
    P1DIR |= BIT6;                                  // Set P1.6 to output - LED
    P1SEL |= BIT6;                                  // Set P1.6 as PWM output
    P1DIR |= BIT0;                                  // Set P1.0 to output - Debug LED
}

void ButtonSetup()                                  // All initial settings for button use
{
    P1DIR &= ~BIT3;                                 // Set P1.3 to an input (just in case)
    P1REN |= BIT3;                                  // Enable resistor on P1.3
    P1OUT |= BIT3;                                  // Set resistor to pull-up
    P1IE  |= BIT3;                                  // P1.3 Interrupt enabled
    P1IES |= BIT3;                                  // P1.3 Falling edge selected
    P1IFG &= ~BIT3;                                 // P1.3 Interrupt Flag cleared
}

void TimerA0Setup()                                 // All initial settings for TimerA0
{
    TA0CCR0 = 1000;                                 // Set Capture/Compare register to 1000
    TA0CCTL1 = OUTMOD_7;                            // PWM set/reset enabled on Timer0
    TA0CCR1 = 500;                                  // Set Capture/Compare register to 500
    TA0CTL = TASSEL_2 + MC_1 + ID_2;                // Set Timer A0 set to SMCLK in Up Mode with divider 4
}

#pragma vector = PORT1_VECTOR                       // Detects interrupt on Port 1
__interrupt void Port_1(void)
{
    if (P1IES & BIT3)                               // Checks if the button was pressed
    {
        P1OUT |= BIT0;                              // Turn the LED on P1.0 on
        P1IES &= ~BIT3;                             // Set board to detect rising edge (button release)
        if (TA0CCR1 < TA0CCR0)                      // Increase the duty cycle on button press
                TA0CCR1 += 100;                     // Increase by 100
        else
                TA0CCR1 = 0;                        // Reset back to 0
    }
    else                                            // If the button was released
    {
        P1OUT &= ~BIT0;                             // Turn the LED on P1.0 off
        P1IES |= BIT3;                              // Set board to detect falling edge (button press)
    }

    P1IFG &= ~BIT3;                                 // P1.3 Interrupt Flag cleared
}
