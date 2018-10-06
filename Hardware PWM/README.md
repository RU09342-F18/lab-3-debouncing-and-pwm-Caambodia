## Overview:
The main.c files function to emulate pulse width modulation using the internal hardware on the boards

## Description of Code:
The major setup portions have been split into individual functions for the sake of organization. These functions are used in the main function and listed later. We then enable Global Interrupt with the line `__bis_SR_register(GIE)` and set it to run continuously with as simple `while(1)` loop.

The line `P1DIR |= BIT0` sets the pin P1.0 as an output. This is the pin on the MSP430G2553 board used where the LED is located, and so we set it as an output so we can control it with the rest of our code. By ORing `P1DIR` with `0x01` or `00000001`, we set the least significant bit in `P1DIR` to 1, which makes it an output. The 0s allow us to avoid disturbing the rest of the bits in `P1DIR`. The same is done with P1.6 on the MSP430G2553 and P4.7 on the MSP430F5529. We also use the line `P1SEL |= BIT6` to enable the LED on P1.6 for PWM. However, due to the limitations on the MSP430F5529, there is no PWM function on the hardware for any pin other than P1.2, which is not connected to an LED. The functionality of the code has been confirmed using an oscillocope.

Instead of using the line `P1DIR &= ~BIT3` to set the button as an input, we use the line `P1IE |= BIT3` to enable the button to send interrupts that we can read instead of constantly checking if the button is pressed (P1.1 is used for the button on the MSP430F5529). This line is followed up with `P1IES |= BIT3` to set the interrupt to trigger on the falling edge of the button, which is when the button is pressed. We still have to enable the pull-up resistor for the button, and preemptively clear the interrupt flag on P1.3 with `P1IFG &= ~BIT3`.

Also, instead of incrementing a counter until a certain limit, we can use the internal clocks of the boards to count for us. We use the line `TA0CTL = TASSEL_1 + MC_1` to set up a timer for both the P1.6 LED and the P1.2 output, and set it to Up Mode. We also enable Capture/Compare on the timer. Timer1, which is used for the actual PWM functionality, sets CCR0 to 1000 and CCR1 to 500. The debouncing function was abandoned in this program due to its unknown incompatibility with the PWM functions.

Since the built-in PWM function does most of the work, all we had to do was configure the button's incrementing of the CCR1 register. Upon pressing the button, LED P1.0/P4.7 turn on, the button interrupt is set to read the rising edge, and CCR1 is incremented by 100 or reset when at attempting to increment beyond 1000. When the button is released, LED P1.0/P4.7 is turned off, just to show when the button is pressed for debugging purposes, and the interrupt is set back to read the falling edge. No other operations are complete here, because we don't want the function to increment when the button is released. The interrupt is also cleared after either a button press or release.

Written by Cameron Bendzynski
