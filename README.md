# pid-invertedpendulum

## Method of Operation
There is a single user input button. Its functionality is defined in the Key() method of BALANCE\CONTROL\control.c as follows:
 - Single Click : Forward Direction Balance
 - Double Click : Reverse Direction Balance
 - Long Press : Toggle Auto-Balance Mode

Auto-Balance mode will swing the pendulum back and forward until it reaches a point when:
 - the position is not close to the edge
 - the angle is near the balance point
 - the angular velocity is close to 0
When this position is achieved three times consecutively, the code sets PID parameters based on that as the target point. 
 
The pendulum will _stop_ attempting to balance whenever any of the conditions in the Turn_Off() method of BALANCE\CONTROL\control.c method are met:
 - Voltage falls below 700
 - Flag_Stop = 1
 - Angle_Balance differs from balance point by > 500

## Code Structure
 - The main() program loop is contained in USER\Minibalance.c, but you'll find it quite sparse - it begins by initialising all the hardware and then enters an infinite while() loop whose only function appears to be to update the display.
 - The reason is that the control loop is actually contained in the TIM1_UP_IRQHandler() function defined in BALANCE\CONTROL\control.c, which is an interrupt service routine called every 5ms.
