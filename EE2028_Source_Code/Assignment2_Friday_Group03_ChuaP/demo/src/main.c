/*****************************************************************************
 *   Lingke & Peiyuan
 *
 *   DO NOT MODIFY WITHOUT SAVING A COPY OF THE PREVIOUS VERSION
 *
 *   VERSION CONTROL IS THE KEY IN OUR PROJECT!!!!!
 *
 ******************************************************************************/

#include "stdio.h"
#include "string.h"
#include "inttypes.h"

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"

#include "joystick.h"
#include "pca9532.h"
#include "acc.h"
#include "oled.h"
#include "led7seg.h"
#include "temp.h"
#include "light.h"
#include "uart2.h"


// ================================= Global Variables =======================================

// static variable

#define NOTE_PIN_HIGH() GPIO_SetValue(0, 1<<26);
#define NOTE_PIN_LOW()  GPIO_ClearValue(0, 1<<26);

// ========================================= think if I really need these global variables =======================================
	// for light sensor
uint32_t light_value;
	// for temperature sensor
float temp_value;

	// for the switch control
uint32_t last_switch_pressed_time;

	// for accelerometer
int32_t accel_x = 0;
int32_t accel_y = 0;
int32_t accel_z = 0;
int32_t accel_xoff = 0;
int32_t accel_yoff = 0;
int32_t accel_zoff = 0;
// uint8_t accel_dir = 1; // this is probably asking if the movement is going up or down

	// for scheduler to keep track of the function
int32_t C_index = 0; //this is a number that points to the current executing function

	// measuring how many times the switch is pressed
uint8_t No_irq = 0;

	// measuring how many times a task is runned
uint8_t loopcount = 0;

	// this is for the light interrupt
uint8_t move_in_dark_flag[2] = {0, 0};

	//msg_num ==> counting how many message have send
uint32_t msg_num = 0;
	// this is for the temperature interrupt
uint8_t temperature_high_flag = 0;

	// systic timer
volatile uint32_t msTicks = 0; // per 10 micro second timer

	// declaration of my scheduler structure
	// TCB = task control block
typedef struct TCBstruct{
	void (*function_ptr) (void *p);
	void *arg_ptr;
	int state;
	int delay; // anything bigger or equal to 0 means this task should be in waiting mode and every loop will reduce the waiting time by 1
}tcb;

#define STATE_RUNNING 0
#define STATE_READY 1
#define STATE_WAITING 2
#define STATE_INACTIVE 3

#define MODE_CARETAKER 0
#define MODE_MONITOR 1

#define CARETAKER_MSG 0
#define MONITOR_MSG 1
#define LEAVING_MONITOR 2

#define N_MAX_TASK 10

#define name_welcome_module 0
#define name_module 1
#define switch_module 2
#define seven_segment_display_module 3
#define accel_value_gather_module 4
#define temp_value_gather_module 5
#define light_value_gather_module 6
#define oled_module_title 7
#define oled_module_value 8

tcb TaskList [N_MAX_TASK];

// =============================================================================================

// declaring functions:
void init();
void NAME_welcome();
void mode_Switch ();
void OLED_display_title ();
void NAME();
void OLED_display_value ();
void seven_segment_display();
void accel_value_gather();
void accel_init();
void temp_value_gather();
void light_value_gather();
void task_init();
void scheduler();
void start_function(void (*) ());
void halt_me();
void start_task(int32_t);
void delay(int32_t);
void halt_task(int32_t task_id);
void CARETAKER_mode();
void MONITOR_mode();
void EINT3_IRQHandler();
uint32_t getTicks(void);
void SysTick_Handler(void);
void alert_sound();


// Main loop
int main (void) {
	// initialisation and configuration
	init(); // my own initialisation package
	SysTick_Config(SystemCoreClock / 1000); // 10 ms time
	uint32_t msTicks_memory; // per second timer
	msTicks_memory = getTicks();

	int32_t second_counter = -1;
	int32_t *time_ptr = &second_counter;
	TaskList[seven_segment_display_module].arg_ptr = (void *) time_ptr;

	int32_t mode_next = MODE_CARETAKER;
	int32_t good_bye_msg = LEAVING_MONITOR;
	int32_t *msg_ptr = &good_bye_msg;
	int32_t *mode_ptr = &mode_next;
	TaskList[switch_module].arg_ptr = (void *) mode_ptr;
	TaskList[name_welcome_module].arg_ptr = (void *) mode_ptr;

	// testing uart receiver function
	uint8_t data;

    while (1)
    {
    	// mode switching to monitor through sw4
    	uint8_t sw4 = ( (GPIO_ReadValue(1) >> 31) & 0x01);
    	if (*mode_ptr == MODE_CARETAKER){
    		// mode switching
    		if (sw4 == 0){
    			mode_next = MODE_MONITOR;
    			mode_Switch();
    			C_index = 0;
    		}

    		// reset the second counter
    		if (second_counter != -1){
    			second_counter = -1;
				start_task(seven_segment_display_module);
    		}
    	}else if (*mode_ptr == MODE_MONITOR){
    		// mode switching
    		if (No_irq >= 2){
    			No_irq = 0;
    			TaskList[name_welcome_module].arg_ptr = (void *) msg_ptr;
				NAME_welcome ();
				TaskList[name_welcome_module].arg_ptr = (void *) mode_ptr;
				mode_next = MODE_CARETAKER;
				mode_Switch();
				C_index = 0;
    		}else if (No_irq == 1 && (msTicks - last_switch_pressed_time > 1000)){
    			No_irq = 0;
        		/*
        		 * removing the irq flag if the user only press once within 2 seconds
        		 * --> means user dont really want to switch mode but accidental press it
        		 */
    		}

    		// blinking LED
			if (msTicks - msTicks_memory >= 500){
				if (move_in_dark_flag[1]){
					GPIO_SetValue( 0, (1<<26) );
				}
				GPIO_ClearValue( 2, (1<<0));
			}else{
				if (temperature_high_flag){
					GPIO_SetValue( 2, (1<<0));
				}
				GPIO_ClearValue( 0, (1<<26) );
			}

			// alert if all flags are present
			if ( move_in_dark_flag[1] && temperature_high_flag){
				alert_sound();
			}

			if (data == 's'){
				alert_sound();
			}

    		// task conduct
			if (msTicks - msTicks_memory >= 1000){
				msTicks_memory = getTicks();
				second_counter ++;
				loopcount = 0;
				start_task(seven_segment_display_module);
				switch (second_counter){
						case 5: start_task(oled_module_value);
								break;
						case 10: start_task(oled_module_value);
								break;
						case 15: start_task(name_module);
								start_task(oled_module_value);
								break;
						default: break;
				}
			}
    		// reset
    		if (second_counter >= 16){
					second_counter = 0;
				}
    		UART_Receive(LPC_UART3, &data, 1, NONE_BLOCKING);
    	}else if (second_counter != -1){
    			// disable the counter
    		second_counter = -1;
    		start_task(seven_segment_display_module);
    	}

    	// check flag for the accel
    	if (( accel_x > 5 || accel_y > 5 || accel_z > 100) && (move_in_dark_flag[0] == 1) ){
    		move_in_dark_flag[1] = 1;
    	}

		scheduler();
    }
}

// ======================================= Task related modules ==============================================
// scheduler
void scheduler(){
	// task reset
	if (TaskList[C_index].function_ptr == NULL && C_index != 0){
		C_index = 0;
	}

	// task running
	switch (TaskList[C_index].state){
			case 1:
				start_function(TaskList[C_index].function_ptr);
				C_index ++;
				break;
			case 2:
				if (loopcount == 0){
					start_task(C_index);
				}
				C_index++;
				break;
			default:
				C_index ++;
		}

	return;
}
// to start the task
void start_function (void (*functionPTR) () ){
	TaskList[C_index].state = STATE_RUNNING;
	functionPTR();
	return;
}
// halting the task
void halt_me(){
	TaskList[C_index].state = STATE_INACTIVE;
	return;
}
// start the task
void start_task(int32_t task_id){
	TaskList[task_id].state = STATE_READY;
	return;
}
// halting another task
void halt_task(int32_t task_id){
	TaskList[task_id].state = STATE_INACTIVE;
	return;
}
// put the task to sleep for d loops
void delay(int32_t d){
	TaskList[C_index].delay = d;
	TaskList[C_index].state = STATE_WAITING;
	return;
}
// this is the switch module which controls which mode of operation to switch into based on control flags
void mode_Switch (){
	int *ptr = (int *) TaskList[switch_module].arg_ptr;

	if (*ptr == MODE_CARETAKER){
		// clearing unwanted OLED display
		oled_clearScreen(OLED_COLOR_BLACK);
		GPIO_ClearValue( 2, 0 );
		GPIO_ClearValue( 0, (1<<26) );
		move_in_dark_flag[1] = 0;
		temperature_high_flag = 0;
		NOTE_PIN_LOW(); // if the speaker is blasting, remove it
		CARETAKER_mode();
	}else if (*ptr == MODE_MONITOR){
		MONITOR_mode();
	}
	halt_me();
	return;
}
void CARETAKER_mode(){
	start_task(name_welcome_module); // Sending the mode related message

	halt_task(name_module);
	halt_task(oled_module_title);
	halt_task(oled_module_value);
	halt_task(accel_value_gather_module);
	halt_task(temp_value_gather_module);
	halt_task(light_value_gather_module);

	return;
}
void MONITOR_mode(){
	start_task(name_welcome_module);
	start_task(oled_module_title);
	start_task(oled_module_value);
	start_task(temp_value_gather_module);
	start_task(light_value_gather_module);
	start_task(accel_value_gather_module);

	return;
}
void SysTick_Handler(void){
	msTicks ++;
}
uint32_t getTicks(void){
	return msTicks;
}
// ================================================================
	// sensor & actuator related functions
// this is the communication between LPC baseboard and the teraterm software through UART communication
void NAME_welcome () {

	/*
	 * msg_index table
	 * 		0 ==> Entering CARETAKER mode\r\n
	 * 		1 ==> Entering MONITOR mode\r\n
	 * 		2 ==> Leaving MONITOR mode
	 */
	// non-static variable
	static char* msg;

	int msg_index;

	// getting the argument value as it is set elsewhere
	int *ptr = (int *) TaskList[name_welcome_module].arg_ptr;
	msg_index = *ptr;

	switch (msg_index){
			case 0: msg = "Entering CARETAKER mode\r\n"; break;
			case 1: msg = "Entering MONITOR mode\r\n"; break;
			case 2: msg = "Leaving MONITOR mode\r\n"; break;
			default: msg = "\r\n";
	}

	UART_Send (LPC_UART3, (uint8_t *) msg, strlen(msg), BLOCKING);

	halt_me();
	return;
}

void NAME(){
	// for UART communication

	uint8_t msg[64];

	/*
	 * uint32_t UART_Send(LPC_UART_TypeDef *UARTx, uint8_t *txbuf, uint32_t buflen, TRANSFER_BLOCK_Type flag)
	 *
	 * uint32_t UART_SendString(LPC_UART_TypeDef *UARTx, uint8_t *txbuf) --> UART_SendString (LPC_UART3, &line);
	 */


	if (temperature_high_flag){
		uint8_t temp_msg[20];
		sprintf(temp_msg, "Fire detected\r\n");
		UART_SendString (LPC_UART3, &temp_msg);
	}

	if (move_in_dark_flag[1]){
		uint8_t move_dark_msg[30];
		sprintf(move_dark_msg, "Movement in darkness detected\r\n");
		UART_SendString (LPC_UART3, &move_dark_msg);
	}

	float *temp_ptr = (float *) TaskList[temp_value_gather_module].arg_ptr;

	//char light_value[5] = "";
	int *light_ptr = (int *) TaskList[light_value_gather_module].arg_ptr;

	switch (msg_num){
	case 0 ... 9:
		sprintf(msg, "NO:00%d T%0.1f L%d AX%d AY%d AZ%d\r\n", msg_num, *temp_ptr, *light_ptr, accel_x, accel_y, accel_z);
		break;
	case 10 ... 99:
		sprintf(msg, "NO:0%d T%0.1f L%d AX%d AY%d AZ%d\r\n", msg_num, *temp_ptr, *light_ptr, accel_x, accel_y, accel_z);
		break;
	default:
		sprintf(msg, "NO:%d T%0.1f L%d AX%d AY%d AZ%d\r\n", msg_num, *temp_ptr, *light_ptr, accel_x, accel_y, accel_z);
		break;
	}
	//sprintf(msg, "NO:%d: T%0.1f L%d AX%d AY%d AZ%d\r\n", msg_num, *temp_ptr, *light_ptr, accel_x, accel_y, accel_z);
	msg_num ++;
	UART_SendString (LPC_UART3, &msg);

	halt_me();
	return;
}

// OLED module for display the title
void OLED_display_title(){
	uint8_t string[7] = "MONITOR";
	//int y = 0;
	oled_putString(20, 0, string, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	//y+=8;

	uint8_t t[12];
	sprintf(t, "Temperature:");
	oled_putString(0, 8, t, OLED_COLOR_WHITE, OLED_COLOR_BLACK);
	//y+=16;

	uint8_t a[15];
	sprintf(a, "Accelerometer: ");
	oled_putString(0, 32, a, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

	halt_me();
	return;
}

// OLED module for display the changing values only
void OLED_display_value(){ // can seperate this into a header file and a value file, the only thing that needs to change is the value file
	/*
	 * fb ==> may be front color --> OLED_COLOR_BLACK
	 * bg ==> may be background color --> OLED_COLOR_WHITE
	 * sprintf ( char *buffer, const char *format-string, argument-list);
	 * oled_putString (x_coordinate, y_coordinate, string_ptr, fb, bg);
	 */

	//char temp_value[5] = "";
	float *temp_ptr = (float *) TaskList[temp_value_gather_module].arg_ptr;

	uint32_t *light_ptr = (uint32_t *) TaskList[light_value_gather_module].arg_ptr;

	//int y = 16;

	uint8_t t_value[3];
	sprintf(t_value, "%0.1f", *temp_ptr);
	oled_putString(10, 16, t_value, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

	//y = 24;
	uint8_t l_value[7];
	if (light_value < 100){
		oled_putChar(35, 24, '0', OLED_COLOR_BLACK, OLED_COLOR_BLACK);
	}
	sprintf(l_value, "LUX:%d", light_value); // Here got problem, figure out if clear value is needed
	oled_putString(0, 24, l_value, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

	//y = 40;
	uint8_t axy_value[12];
	sprintf(axy_value,"X:%d  Y:%d", accel_x, accel_y);
	oled_putString(20, 40, axy_value, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

	//y+=8;
	uint8_t az_value[5];
	sprintf(az_value,"Z:%d", accel_z);
	oled_putString(20, 48, az_value, OLED_COLOR_WHITE, OLED_COLOR_BLACK);

	halt_me();
	return;
}

// seven segment display module
void seven_segment_display(){
	/*
	 * led7seg_setChar (ch, rawMode);
	 * rawMode = 1 --> all 8 segments can be controlled individually
	 * rawMode = 0 --> ASCII character can be displayed
	 * ch = 'character'
	 */
		// for 7 segment display
	int character_to_display;
	int msg_index;
	int *ptr = (int *) TaskList[seven_segment_display_module].arg_ptr;
	msg_index = *ptr;

	// table mapping
	switch (msg_index){
			case 0: character_to_display = '0'; break;
			case 1: character_to_display = '1'; break;
			case 2: character_to_display = '2'; break;
			case 3: character_to_display = '3'; break;
			case 4: character_to_display = '4'; break;
			case 5: character_to_display = '5'; break;
			case 6: character_to_display = '6'; break;
			case 7: character_to_display = '7'; break;
			case 8: character_to_display = '8'; break;
			case 9: character_to_display = '9'; break;
			case 10: character_to_display = 'A'; break;
			case 11: character_to_display = 'B'; break;
			case 12: character_to_display = 'C'; break;
			case 13: character_to_display = 'D'; break;
			case 14: character_to_display = 'E'; break;
			case 15: character_to_display = 'F'; break;
			default: character_to_display = ':';
	}

	led7seg_setChar (character_to_display, 0);
	halt_me();
	return;
}

// accelerometer value gather
void accel_value_gather(){
	int8_t x, y, z;
	acc_read(&x, &y, &z);
	accel_x = (int)x + accel_xoff;
	accel_y = (int)y + accel_yoff;
	accel_z = (int)z + accel_zoff;

	TaskList[C_index].state = STATE_READY;
	return;
}

// temperature value gather
void temp_value_gather(){
	int32_t temp_value_10_times_larger = temp_read();
	temp_value = temp_value_10_times_larger / 10.0;
	if (temp_value > 30){
		temperature_high_flag = 1;
	}
	float *ptr_1 = &temp_value;
	TaskList[temp_value_gather_module].arg_ptr = (void *) ptr_1;

	delay (1);
	return;
}

// light value gather
void light_value_gather(){
	// for light sensor
	light_value = light_read();
	if (light_value < 50){
		move_in_dark_flag[0] = 1;
	}else{
		move_in_dark_flag[0] = 0;
	}

	loopcount = 1;
	delay (1);
	return;
}

void EINT3_IRQHandler(){
	// enable GPIO interrupt p2.10
	if ((LPC_GPIOINT -> IO2IntStatR >> 10) & 0x1){
		No_irq ++;
		last_switch_pressed_time = getTicks();
	}

	// clear EINT3 interrupt after handled
	LPC_GPIOINT -> IO2IntClr = 1 << 10;

	return;
}

void alert_sound() {
	//int i;

	//for (i = 0; i < 5; i ++){
		NOTE_PIN_HIGH();
		Timer0_us_Wait(1275 / 2);

		NOTE_PIN_LOW();
		Timer0_us_Wait(1275 / 2);
	//}
    return;
}
// =================================== initialisation ===========================================
void init(){
	int j = 0; // value loader

	// SPI

	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg[3];
	/*
	 * Initialize SPI pin connect for OLED
	 * P0.7 - SCK;
	 * P0.8 - MISO
	 * P0.9 - MOSI
	 * P2.2 - SSEL - used as GPIO
	 */
	PinCfg[j].Funcnum = 2;
	PinCfg[j].OpenDrain = 0;
	PinCfg[j].Pinmode = 0;
	PinCfg[j].Portnum = 0;
	PinCfg[j].Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg[j]);
	PinCfg[j].Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg[j]);
	PinCfg[j].Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg[j]);
	PinCfg[j].Funcnum = 0;
	PinCfg[j].Portnum = 2;
	PinCfg[j].Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg[j]);
	j ++;

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);

// ==========================================================

	// I2C

	/* Initialize I2C2 pin connect */
		/*  This is for light sensor
		 * 	light sensor -> PIO0.5 SCL => P0.10
		 * 	light sensor -> PIO0.4 SDA => P0.11
		 * 	function 2
		 */
	PinCfg[j].Funcnum = 2;
	PinCfg[j].Pinnum = 10;
	PinCfg[j].Portnum = 0;
	PINSEL_ConfigPin(&PinCfg[j]);
	PinCfg[j].Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg[j]);
	j++;

	// Initialize I2C peripheral
	I2C_Init(LPC_I2C2, 100000);

	/* Enable I2C1 operation */
	I2C_Cmd(LPC_I2C2, ENABLE);



// ==========================================================

	// UART

	// this is for the UART3 pins
	// this is for RXTX connection
	PinCfg[j].Funcnum = 2;
	PinCfg[j].Portnum = 0;
	PinCfg[j].Pinnum = 0;
	PINSEL_ConfigPin (&PinCfg[j]);

	PinCfg[j].Pinnum = 1;
	PINSEL_ConfigPin (&PinCfg[j]);
	j++;

	// this is for the UART configuration stuff
	UART_CFG_Type uartCfg;
	uartCfg.Baud_rate = 115200;
	uartCfg.Databits = UART_DATABIT_8;
	uartCfg.Parity = UART_PARITY_NONE;
	uartCfg.Stopbits = UART_STOPBIT_1;

	// supply power & setup working parameters for uart3;
	UART_Init (LPC_UART3, &uartCfg);

	// enable transit for UART3
	UART_TxCmd (LPC_UART3, ENABLE);

// ==========================================================

	// Switch 3 interrupt (mode switch to caretaker)
		// enable GPIO interrupt for P2.10 (PIO2_9)
	LPC_GPIOINT -> IO2IntEnR |= 1 << 10;

		// clear & enable EINT3 interrupt
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);


// =========================================================
	// other initialisation
	pca9532_init();
	joystick_init();
	acc_init();
	oled_init();
	led7seg_init();
	temp_init(getTicks);
	light_init();
	light_enable();

	// our LED
	GPIO_SetDir( 2, (1 << 0), 1 );
	GPIO_SetDir( 0, (1 << 26), 1 );

	GPIO_ClearValue( 2, 0 );
	GPIO_ClearValue( 0, (1<<26) );

	// SW4 as the monitor switch PIO1_4 (P1.31) as input
	GPIO_SetDir(1, 1 << 31, 0);
	GPIO_ClearValue(1, 1<<31);

	// clearing unwanted values
	oled_clearScreen(OLED_COLOR_BLACK);

	// accel initialisation
	accel_init();

	// task initialisation
	task_init();

    /* ---- Speaker ------> */

    GPIO_SetDir(2, 1<<0, 1);
    GPIO_SetDir(2, 1<<1, 1);

    GPIO_SetDir(0, 1<<27, 1);
    GPIO_SetDir(0, 1<<28, 1);
    GPIO_SetDir(2, 1<<13, 1);
    GPIO_SetDir(0, 1<<26, 1);

    GPIO_ClearValue(0, 1<<27); //LM4811-clk
    GPIO_ClearValue(0, 1<<28); //LM4811-up/dn
    GPIO_ClearValue(2, 1<<13); //LM4811-shutdn

    /* <---- Speaker ------ */


	return;
}

void task_init(){
	C_index = 0;

		//initialising the argument for the switch statement
	int switch_arg = MODE_CARETAKER;
	int *int_ptr = &switch_arg;
	TaskList[C_index].function_ptr = NAME_welcome;
	TaskList[C_index].arg_ptr = (void *) int_ptr;
	TaskList[C_index].state = STATE_READY;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = NAME;
	TaskList[C_index].arg_ptr = (void *) int_ptr;
	TaskList[C_index].state = STATE_INACTIVE;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = mode_Switch;
	TaskList[C_index].arg_ptr = NULL;
	TaskList[C_index].state = STATE_INACTIVE;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = seven_segment_display;
	TaskList[C_index].arg_ptr = NULL;
	TaskList[C_index].state = STATE_READY;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = accel_value_gather;
	TaskList[C_index].arg_ptr = NULL;
	TaskList[C_index].state = STATE_INACTIVE;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = temp_value_gather;
	TaskList[C_index].arg_ptr = NULL;
	TaskList[C_index].state = STATE_INACTIVE;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = light_value_gather;
	TaskList[C_index].arg_ptr = NULL;
	TaskList[C_index].state = STATE_INACTIVE;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = OLED_display_title;
	TaskList[C_index].arg_ptr = NULL;
	TaskList[C_index].state = STATE_INACTIVE;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = OLED_display_value;
	TaskList[C_index].arg_ptr = NULL;
	TaskList[C_index].state = STATE_INACTIVE;
	TaskList[C_index].delay = -1;
	C_index++;

	TaskList[C_index].function_ptr = NULL;
	C_index++;

	uint32_t *ptr = &light_value;
	TaskList[light_value_gather_module].arg_ptr = (void *) ptr;
	return;
}

// initialising the x_off y_off and z_off
void accel_init(){
	int8_t x, y, z;
	acc_read(&x, &y, &z);
	accel_xoff = 0 - (int)x;
	accel_yoff = 0 - (int)y;
	accel_zoff = 64 - (int)z;
	return;
}
