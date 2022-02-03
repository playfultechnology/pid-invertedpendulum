/**************************************************************************
 * "Inverted Pendulum" as advertised by "Wheeltec" or "Minibalance" on AliExpress
 * This code is written for STM32F103RC-based control board. 
 *
 **************************************************************************/

// INCLUDES
#include "sys.h"

// GLOBALS
// Flags
u8 Flag_Stop=1, delay_50, delay_flag;         //停止标志位 50ms精准演示标志位
u8 system_start=0;                          //检测调节函数标志位
u8 tips_flag = 0;                           //OLED提示函数标志位
// Encoder pulse reading and defined zero position - i.e. related to linear position
int Encoder, Position_Zero=10000;
// Motor PWM value
int Moto;
// Battery Voltage
int Voltage;
// Angular encoder reading
float Angle_Balance;
// PID parameters
float Balance_KP=400, Balance_KD=400, Position_KP=20, Position_KD=300;
// PID adjustment parameters
float Menu=1, Amplitude1=5, Amplitude2=20, Amplitude3=1, Amplitude4=10;
// Rate of change of angle of balance rod
extern float D_Angle_Balance;

// MAIN CODE ENTRY POINT
// (but note that much of the control loop is handled by interrupt functions defined elsewhere)
int main(void) { 
	Stm32_Clock_Init(9);            // System clock settings
	delay_init(72);                 // Delayed initialisation
	JTAG_Set(JTAG_SWD_DISABLE);     // Turn off the JTAG interface
	JTAG_Set(SWD_ENABLE);           // Open the SWD interface for debugging
	delay_ms(1000);                 // Pause to let the system stabilise
	delay_ms(1000);                 // Pause for a total of 2 seconds
	LED_Init();                     // Initialise hardware interface to the LED
	EXTI_Init();                    // Input key initialisation (handled by external interrupt)
	OLED_Init();                    // OLED screen initialisation
	uart_init(72,128000);           // Initialise serial port 1
  MiniBalance_PWM_Init(7199,0);   // Initialise PWM 10KHZ for driving the motor 
	Encoder_Init_TIM4();            // Initialise the encoder 
	Adc_Init();                     // Angular displacement sensor uses ADC (analog)
	Timer1_Init(49,7199);           // Timed interrupt initialisation
	while(1) {      
		DataScope();	            // Host computer
		Tips();                   // Update OLED display and prompt
		delay_flag=1;	            // 50ms interrupt precision delay flag   
		while(delay_flag);        // Precise delay is mainly because the waveform display on the host computer needs a strict 50ms transmission cycle 
	} 
}
