#include "fsm.h"						// FSM Framework - Kyle Dyck

/* ---------------------------------------- Typdefs & Defines ----------------------------------------*/
#define BRAKE_PRESSED   1
#define ABS_ENGAGED     2
#define BRAKE_ENGAGED   4
#define MAX_EVENTS			100 // modern ABS systems engage 15 times / sec -> 15*5sec = 75 events / 5 sec. 100 for extra room.
#define MAX_0V_VAL			0.5
#define FAIL_VAL_MS	  	25000

typedef enum fsm_states
{
    NORMAL,
    COLLISION,
		FAILURE
} my_states;

typedef enum fsm_events
{
    TRIGGER_AIRBAG,
    TRIGGER_FAILURE
} my_events;

/* ---------------------------------------- Globals ----------------------------------------*/
uint8_t g_curr_brake_status = 0;
uint8_t g_prev_brake_status = 0; /* determines if state change in brakes -> need to record brake event */
/* assume we can store max 100 events */
/* [0] -> time stamp from timer (ticks) 
   [1] -> brake event ID (see enumeration) */
uint32_t g_events_arr[MAX_EVENTS][2] = {{0}};
uint16_t g_events_idx = 0; // i.e. "write index"
/* go as far back as this index */
uint16_t g_events_5sec_old_idx = 0; // i.e. "read" index
float g_voltage = 0;
uint32_t g_input_read_time = 0;

/* ---------------------------------------- Helper Functions ----------------------------------------*/
void print_whole_buffer( void )
{
	int i = 0;
	printf("buffer:\n");
	for (i = 0; i < MAX_EVENTS; i++)
	{
		printf("%d %d %d\n", i, g_events_arr[i][0], g_events_arr[i][1]);
	}
}

void update_inputs( void )
{
    uint16_t joy_val = read_joystick();
    uint32_t pot_val = read_potentiometer();
		uint8_t button_val = read_pushbutton();
		g_voltage = pot_val * 3.3 / 4096;
	
		// set brake status bits
		g_curr_brake_status = 0;
		g_curr_brake_status |= (joy_val > 0);
		g_curr_brake_status |= ( (button_val == 1) << 1);
		g_curr_brake_status |= ( (joy_val > 0) && (button_val == 0 ) ) << 2;
    
		//printf("brake status: %x\r\n", g_curr_brake_status);
}

void print_brake_log ( void ) 
{
	/* prints the last 5 seconds of brake events */
	// want to read from 5sec index to current idex
	// 5sec_idx > curr_idx indicates wrap-around occurred
	int i = 0;
	uint32_t time_diff = 0;
	//printf("printing log. input_time = %d curr_idx = %d old_idx = %d\n", g_input_read_time, g_events_idx, g_events_5sec_old_idx);
	if (g_events_5sec_old_idx <= g_events_idx)
	{
		// normal buffer reading
		for (i = g_events_5sec_old_idx; i < g_events_idx; i++)
		{
			// print out brake log
			// [0] -> timestamp
			// [1] -> brake status
			time_diff = g_input_read_time - g_events_arr[i][0];
			if (time_diff < 5000)
			{
				printf("%d %d\n", time_diff, g_events_arr[i][1]);
			}
			
		}
	}
	else
	{
		// extra logic for wrapping around
		//dprint("wrapped!");
		// assume there is > 5 seconds between the read pointer and the write pointer on wraparound
		// print top half of buffer
		for (i = g_events_5sec_old_idx; i < MAX_EVENTS; i++)
		{
			// print out brake log
			// [0] -> timestamp
			// [1] -> brake status
			time_diff = g_input_read_time - g_events_arr[i][0];
			if (time_diff < 5000)
			{
				printf("%d %d\n", time_diff, g_events_arr[i][1]);
			}
		}
		// print lower half up to current index
		for (i = 0; i < g_events_idx; i++)
		{
			// print out brake log
			// [0] -> timestamp
			// [1] -> brake status
			time_diff = g_input_read_time - g_events_arr[i][0];
			if (time_diff < 5000)
			{
				printf("%d %d\n", time_diff, g_events_arr[i][1]);
			}
		}
	}
	
	//print_whole_buffer();
	
}

/* ---------------------------------------- FSM Functions ----------------------------------------*/
/* NORMAL state */
int normal_enter( void )
{
    reset_all_LEDS();
    update_inputs();
    return 0;
}
int normal_active ( void )
{
		static uint32_t accel_start_time = 0;
		static uint32_t twoV_start_time = 0;
    //dprint("NORMAL");
	
		while (1)
		{
			g_input_read_time = read_timer_ms();
			update_inputs();
			//printf("g_voltage value: %f\r\n", g_voltage);
			
			// ------------------------------------------------------ airbag checking for 3V
			if (g_voltage > 3)
			{
				fsm_trigger_event(TRIGGER_AIRBAG);
				return 0;
			}
			
			// ------------------------------------------------------ checking 2V press for 5 ms (assuming previous steps take < 5 ms to execute
			if (g_voltage > 2)
			{
				if (twoV_start_time == 0)
				{
					// mark down first recording of acceleration > 0
					twoV_start_time = g_input_read_time;
				}
				else
				{
					// check if 2V for > 5 ms
					// assuming timer won't wrap around during run time for simplicity
					if (read_timer_ms() - twoV_start_time >= 5)
					{
						// collision
						fsm_trigger_event(TRIGGER_AIRBAG);
						return 0;
					}
				}
			}
			else
			{
				// reset 2V timer
				twoV_start_time = 0;
			}
			
			// ------------------------------------------------------ failure checking ( >0 for 25 s )
			if (g_voltage > MAX_0V_VAL)
			{
				if (accel_start_time == 0)
				{
					// mark down first recording of acceleration > 0
					accel_start_time = g_input_read_time;
				}
				else
				{
					// check if accel for > 25 s
					// assuming timer won't wrap around during run time for simplicity
					if (read_timer_ms() - accel_start_time >= FAIL_VAL_MS)
					{
						// broken accelerometer //
						fsm_trigger_event(TRIGGER_FAILURE);
						return 0;
					}
				}
			}
			else
			{
				// accel stopped, reset timer
				accel_start_time = 0;
			}
			
			// ------------------------------------------------------ brake event recording
			if (g_prev_brake_status != g_curr_brake_status)
			{
				// record change of state of brake status along with time stamp
				//printf("recording brake status at %d: %d curr idx: %d 5sec idx: %d\n", g_input_read_time, g_curr_brake_status, g_events_idx, g_events_5sec_old_idx);
				g_events_arr[g_events_idx][0] = g_input_read_time;
				g_events_arr[g_events_idx][1] = g_curr_brake_status;
				g_prev_brake_status = g_curr_brake_status;
				// update index
				g_events_idx = (g_events_idx < (MAX_EVENTS - 1))? g_events_idx + 1 : 0;
				
				// update 5 sec pointer
				if (g_input_read_time - g_events_arr[g_events_5sec_old_idx][0] > 5000)
				{
					// update 5 sec index to drop old data
					if (g_events_5sec_old_idx < MAX_EVENTS)
					{
						g_events_5sec_old_idx += 1;
						if (g_events_5sec_old_idx < (g_events_idx - 1) )
						{
							// Don't let read pointer == write pointer
							
						}
						
					}
					else
					{
						g_events_5sec_old_idx = 0;
					}
				}
			}
			
			//delayms(500);
		}
}

/* COLLISION state */
int collision_enter( void )
{
	/* turn on all LEDs and print last 5 sec of log */
		set_all_LEDS();
		print_brake_log();
    return 0;
}
int collision_active ( void )
{
  //dprint("COLLISION");  
	while(1);
}

/* FAILURE state */
int failure_enter( void )
{
    /* set LEDs to alternating on */
		set_LED(0);
		set_LED(2);
		set_LED(4);
		set_LED(6);
    return 0;
}
int failure_active ( void )
{
  //dprint("FAILURE");  
	while(1);
}

//This is C. The expected function heading is int main(void)
int main( void ) 
{
	//Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
	init_led_adc();
	init_timer();
    fsm_init(3, 2, NORMAL);
    dprint("initialization complete.");
    
    fsm_set_code_ptr(NORMAL, ENTRY, normal_enter);
    fsm_set_code_ptr(NORMAL, ACTIVE, normal_active);
		fsm_set_code_ptr(COLLISION, ENTRY, collision_enter);
    fsm_set_code_ptr(COLLISION, ACTIVE, collision_active);
		fsm_set_code_ptr(FAILURE, ENTRY, failure_enter);
    fsm_set_code_ptr(FAILURE, ACTIVE, failure_active);
    
    fsm_add_trans(NORMAL, COLLISION, TRIGGER_AIRBAG);
		fsm_add_trans(NORMAL, FAILURE, TRIGGER_FAILURE);
    
    fsm_run_fsm();
}
