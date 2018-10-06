// Created: 10/03/18
// Last Edited: 10/05/18
// Author: Cameron Bendzynski

#include <msp430f5529.h>

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
    P4DIR |= BIT7;                                  // Set P4.7 to output - Debug LED

    P1DIR |= BIT2;                                  // Set P1.2 to output - Output Port
    P1SEL |= BIT2;                                  // Set P1.2 for PWM output (no LED)
}

void ButtonSetup()                                  // All initial settings for button use
{
    P1DIR &= ~BIT1;                                 // Set P1.1 to input
    P1REN |= BIT1;                                  // Enable resistor on P1.1
    P1OUT |= BIT1;                                  // Set resistor to pull-up
    P1IE  |= BIT1;                                  // P1.1 Interrupt enabled
    P1IES |= BIT1;                                  // P1.1 Falling edge selected
    P1IFG &= ~BIT1;                                 // P1.1 Interrupt Flag cleared
}

void TimerA0Setup()                                 // All initial settings for TimerA0
{
    TA0CTL = TASSEL_2 + MC_1 + ID_2;                // Set Timer A0 to ACLK in Up Mode
    TA0CCTL1 = OUTMOD_7;                            // Capture/Compare enable
    TA0CCR0 = 1000;                                 // Set Capture/Compare register to 2000
    TA0CCR1 = 500;
}

#pragma vector = PORT1_VECTOR                       // Detects interrupt on Port 1
__interrupt void Port_1(void)
{
    if (P1IES & BIT1)                               // Checks if the button was pressed
    {
        P4OUT |= BIT7;                              // Turn the LED on P4.7 on
        P1IES &= ~BIT1;                             // Set board to detect rising edge (button release)
        if (TA0CCR1 < TA0CCR0)                      // Increase the duty cycle on button press
                TA0CCR1 += 100;                     // Increase by 100
        else
                TA0CCR1 = 0;                        // Reset back to 0
    }
    else                                            // If the button was released
    {
        P4OUT &= ~BIT7;                             // Turn the LED on P4.7 off
        P1IES |= BIT1;                              // Set board to detect falling edge (button press)
    }

    P1IFG &= ~BIT1;                                 // P1.1 Interrupt Flag cleared
}
