## Overview:
The main.c files function to emulate pulse width modulation using software

## Description of Code:
The major setup portions have been split into individual functions for the sake of organization. These functions are used in the main function and listed later. We then enable Global Interrupt with the line `__bis_SR_register(GIE)` and set it to run continuously with as simple `while(1)` loop.

The line `P1DIR |= BIT0` sets the pin P1.0 as an output. This is the pin on both boards used where the LED is located, and so we set it as an output so we can control it with the rest of our code. By ORing `P1DIR` with `0x01` or `00000001`, we set the least significant bit in `P1DIR` to 1, which makes it an output. The 0s allow us to avoid disturbing the rest of the bits in `P1DIR`. The same is done with P1.6 on the MSP430G2553 and P4.7 on the MSP430F5529.

Instead of using the line `P1DIR &= ~BIT3` to set the button as an input, we use the line `P1IE |= BIT3` to enable the button to send interrupts that we can read instead of constantly checking if the button is pressed (P1.1 is used for the button on the MSP430F5529). This line is followed up with `P1IES |= BIT3` to set the interrupt to trigger on the falling edge of the button, which is when the button is pressed. We still have to enable the pull-up resistor for the button, and preemptively clear the interrupt flag on P1.3 with `P1IFG &= ~BIT3`.

Also, instead of incrementing a counter until a certain limit, we can use the internal clocks of the boards to count for us. We use the lines `TA0CTL = TASSEL_1 + MC_1` and `TA1CTL = TASSEL_1 + MC_1` to set up a timer for both the P1.0 LED and the P1.6/P4.7 LED, and set it to Up Mode. We also enable Capture/Compare on each timer, and set the upper limit of Timer0 to 2000, which was determined using an oscilloscope to be a good value to debounce the signal. Timer1, which is used for the actual PWM functionality, sets CCR0 to 10001 and CCR1 to 5000. There were issues with using 10000 for CCR0 where the LED would switch off, so that extra 1 fixes that issue.

The interrupt portion of the code does not use the timer at first, but looks for a button input. Once the button is pressed, since we set it to detect a falling edge, an interrupt flag is queued, the LED on P1.6 (P4.7 on MSP430F5529) is disabled for debugging purposes, the LED on P1.0 is toggled, interrupts are disabled on button press, and the timer is started. The interrupt function also uses this button press to increment CCR1 by 1000, resetting it to 0 when 10000 has been surpassed.

The "debug LED" then remains off until the timer counts to the value of CCR0, which is set to be 2000. Once this number is reached, button interrupt is re-enabled, the timer is stopped and cleared, the "debug LED" is enabled to show that the button can be pressed again, and the interrupt flag is cleared with the line `P1IFG &= ~BIT3`.

The two Timer1 functions simply turn the LED on when CCR0 has been reached, and off when CCR1 has been reached. Since we are able to change the value of CCR1, this creates a PWM effect.

Written by Cameron Bendzynski
