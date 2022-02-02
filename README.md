# pid-invertedpendulum

This repository contains code to accompany an "Inverted Pendulum" with STM controller I bought on AliExpress.

- [x] The product was described as being provided with source code
- [ ] It was not
- [x] After contacting the seller, they did email me a download
- [ ] The download was a zip archive consisting of a load of various projects, unsorted files, and various directories, in Chinese
- [x] I was able to work out which sections of the download related to the STM32 source code running on the device
- [ ] I'm not that familiar with STM32 project structure
- [X] The code was commented
- [ ] The comments were in Chinese
- [X] Google Translate exists! And I know some Chinese programmers
- [ ] Even when translated, the comments weren't always accurate  

So, I basically ending up having to reverse-engineer the source code from a combination of how the product actually behaved, what the product description said it should do, what the translated Chinese comments said it did, and what the code actually did. And this repository is the result.

## Calibration
Before use, the system should be calibrated as follows:
 1. Ensure the pendulum is stationary and hanging vertically downward
 2. Loosen the top screw connecting the pendulum to the angular encoder 
 3. Twist the central knob (located in the middle of the three screws) to turn the encoder until the ADC value reported on the OLED screen is between 1010 and 1030 (ideal value is 1024)
 4. Tighten the topmost screw to secure the pendulum to the encoder again

## Method of Operation
There are six buttons on the control unit - two on the top (labelled RESET, USER), two on the front (labelled X, M), and two on the back (labelled +/-), with functions as follows:
 - Press RESET button : Resets to default state
 - Single Click USER button : Start / Stop balancing
 - ( Double Click USER : Reverse Direction Balance according to code, but I can't see how you'd be able to do this)
 - Long Press X : Toggle Auto-Balance Mode
 - Press M : Cycle between selected PID parameter (indicated by Y on the OLED screen)
 - Long Press M : Bring up help text
 - +/- : Increase/decrease selected PID parameter by the corresponding A value

When Auto-Balance is selected, the Blue LED L2 will light up. 
Before continuing, **ensure the trolley is located all the way to the left-hand side** (i.e the encoder value P is shown close to 10000).
Starting balancing in this mode will swing the pendulum back and forward until it reaches a point when:
 - the position is not close to the edge
 - the angle is near the balance point
 - the angular velocity is close to 0
When this position is achieved three times consecutively, the code sets PID parameters based on that as the target point. 
 
The pendulum will _stop_ attempting to balance whenever any of the conditions in the Turn_Off() method of BALANCE\CONTROL\control.c method are met:
 - Voltage falls below 700
 - Flag_Stop = 1
 - Angle_Balance differs from balance point by > 500

## Display
OLED readout shows values as follows:
 - B-KP, B-KD: Balance KP, KD PID control values
 - P-KP, P-KD: Position KP, KD PID control values
 - A: Amplitudes, i.e. the amount by which the corresponding PID parameter will be adjusted each time +/- is pressed
 - VOL: Voltage
 - T: Target Position
 - P: Actual Linear Encoder Reading (5,850 at extreme right, 10,000 at extreme left)
 - ADC: Rotary Encoder Reading  (0 when pointing left, increasing anti-clockwise to 4096 after full rotation. So 1024=down, 2048=right, 3072=up)
 - Note that the selected PID parameter that will be edited by the +/- buttons is indicated by a Y, other parameters have an N
![](OLED_output.png)

## Code Structure
 - The main() program loop is contained in USER\Minibalance.c, but you'll find it quite sparse - it begins by initialising all the hardware and then enters an infinite while() loop whose only function appears to be to update the display.
 - The reason is that the control loop is actually contained in the TIM1_UP_IRQHandler() function defined in BALANCE\CONTROL\control.c, which is an interrupt service routine called every 5ms.
 - Input functionality is defined in the Key() method of BALANCE\CONTROL\control.c
 - Some additional input functions are defined in the EXTI15_10_IRQHandler() method contained in HARDWARE\EXTI\exti.c 
 - OLED display is defined in BALANCE\show\show.c
