#include "fsm.h"						// FSM Framework - Kyle Dyck

/* ---------------------------------------- Typdefs & Defines ----------------------------------------*/
#define HYSTER_UPPER            10
#define HYSTER_LOWER            10
#define MAX_MS_FURNACE_ON       4000
#define MIN_MS_FURNACE_OFF      1000

typedef enum states
{
    FURNACE_OFF,
    FURNACE_ON
} my_states;

typedef enum events
{
    TURN_FURNACE_ON,
    TURN_FURNACE_OFF
} my_events;

/* ---------------------------------------- Globals ----------------------------------------*/
static float g_set_temp = 0;
static float g_curr_temp = 0;

/* ---------------------------------------- Helper Functions ----------------------------------------*/
void print_temp( void )
{
    printf("SET TEMP: %+.2f CURRENT TEMP: %+.2f\r\n", g_set_temp, g_curr_temp);
}
void update_inputs( void )
{
    uint16_t joy_val = read_joystick();
    uint32_t pot_val = read_potentiometer();
    g_curr_temp = ((float)pot_val - 2048) * 0.05;
    if (joy_val == 16)
    {
        /* increment set temp */
        g_set_temp += 0.05;
    } 
    else if (joy_val == 64)
    {
        /* decrement set temp */
        g_set_temp -= 0.05;
    }
    
    
}

/* ---------------------------------------- FSM Functions ----------------------------------------*/
/* FURNACE_OFF state */
int furn_off_enter( void )
{
    reset_LED(0);
    update_inputs();
    return 0;
}
int furn_off_active ( void )
{
    uint8_t stay_active = 1;
    uint32_t furn_off_start_time = read_timer_ms();
    uint32_t second_timer = read_timer_ms();
    dprint("FURNACE_OFF");
    while (stay_active)
    {   
        uint32_t curr_time = read_timer_ms();
        uint32_t total_time_off = curr_time - furn_off_start_time;
        uint8_t min_time_off_reached = ( total_time_off >= MIN_MS_FURNACE_OFF );
        uint8_t temp_below_threshold = ( g_curr_temp < ( g_set_temp - HYSTER_LOWER ) );
        
        if (min_time_off_reached && temp_below_threshold)
        {
            stay_active = 0;
        } 
        else
        {
            if ( ( curr_time - second_timer ) >= 1000)
            {
                /* display temp every second */
                print_temp();
                second_timer = curr_time;
            }
        }
        
        update_inputs();
        delayms(50);
    }

    fsm_trigger_event(TURN_FURNACE_ON);
    return 0;
}
int furn_off_exit ( void )
{
    update_inputs();
    set_LED(0);
    return 0;
}

/* FURNACE_ON state */
int furn_on_enter( void )
{
    set_LED(0);
    update_inputs();
    return 0;
}
int furn_on_active ( void )
{
    uint8_t stay_active = 1;
    uint32_t furn_on_start_time = read_timer_ms();
    uint32_t second_timer = read_timer_ms();
    dprint("FURNACE_ON");
    while (stay_active)
    {   
        uint32_t curr_time = read_timer_ms();
        uint32_t total_time_on = curr_time - furn_on_start_time;
        uint8_t max_time_on_reached = ( total_time_on >= MAX_MS_FURNACE_ON );
        uint8_t temp_above_threshold = ( g_curr_temp > ( g_set_temp + HYSTER_UPPER ) );
        
        if (max_time_on_reached || temp_above_threshold)
        {
            stay_active = 0;
        } 
        else
        {
            if ( ( curr_time - second_timer ) >= 1000)
            {
                /* display temp every second */
                print_temp();
                second_timer = curr_time;
            }
            update_inputs();
            delayms(50);
        }
    }
    
    fsm_trigger_event(TURN_FURNACE_OFF);
    return 0;
}
int furn_on_exit ( void )
{
    update_inputs();
    reset_LED(0);
    return 0;
}

int main( void ) 
{
	//Always call this function at the start. It sets up various peripherals, the clock etc.
	SystemInit();
	init_led_adc();
	init_timer();
    fsm_init(2, 2, FURNACE_OFF);
    dprint("initialization complete.");
    
    fsm_set_code_ptr(FURNACE_OFF, ENTRY, furn_off_enter);
    fsm_set_code_ptr(FURNACE_OFF, ACTIVE, furn_off_active);
    fsm_set_code_ptr(FURNACE_OFF, EXIT, furn_off_exit);
    
    fsm_set_code_ptr(FURNACE_ON, ENTRY, furn_on_enter);
    fsm_set_code_ptr(FURNACE_ON, ACTIVE, furn_on_active);
    fsm_set_code_ptr(FURNACE_ON, EXIT, furn_on_exit);
    
    fsm_add_trans(FURNACE_OFF, FURNACE_ON, TURN_FURNACE_ON);
    fsm_add_trans(FURNACE_ON, FURNACE_OFF, TURN_FURNACE_OFF);
    
    fsm_run_fsm();
}
