#include "fsm.h"

/* ---------------------------------------- Globals ----------------------------------------*/
static uint16_t 	  g_curr_state                = 0;
static uint16_t 	  g_num_events                = 0;
static uint16_t       g_num_states                = 0;
static uint8_t		  g_initialized               = 0;
static uint16_t       g_event_mailbox             = 0;
static uint8_t        g_event_triggered           = 0;
static uint16_t**     g_transition_arr            = NULL;
static fsm_function** g_code_ptr_arr              = NULL;
static uint16_t       g_initial_state             = 0;

/* g_transition arr[n][m]: 
    [ [0, 1, 2, 3,...],
      [0, 1, 2, 3,...],
      ...]
    For n states and m events. n maps to the source state, m maps to the destination state. */

/* g_code_ptr_arr [i][3]:
    [ [enter_function, active_function, exit_function],
      [enter_function, active_function, exit_function],
      ...]
    For i states, each state can be assigned an enter function, an active function, and an exit function. */

/* ---------------------------------------- External User Functions ----------------------------------------*/
int fsm_init(uint16_t num_states, uint16_t num_events, uint16_t initial_state)
{
    if (!g_initialized)
    {
        int i = 0;
        int j = 0;
        g_num_states = num_states;
        g_num_events = num_events;
        g_curr_state = initial_state;
        g_initial_state = initial_state;
        
        /* init transition array to make every state point to itself (no transitions) */
        //g_transition_arr =  malloc( g_num_events * g_num_states * sizeof(uint16_t) );
        g_transition_arr =  (uint16_t**) malloc( g_num_states * sizeof(uint16_t*) );
        for (i = 0; i < g_num_states; i++)
        {
            g_transition_arr[i] = (uint16_t*) malloc (g_num_events * sizeof(uint16_t) );
            for (j = 0; j < g_num_events; j++)
            {
                g_transition_arr[i][j] = i;
            }
        }
        
        /* init code pointer array to NULL */
        g_code_ptr_arr = (fsm_function**) malloc( g_num_states * sizeof(fsm_function*) );
        for (i = 0; i < g_num_states; i++)
        {
            g_code_ptr_arr[i] = (fsm_function*) malloc( 3 * sizeof(fsm_function));
            for (j = 0; j < 3; j++)
            {
                g_code_ptr_arr[i][j] = NULL;
            }
        }
    }
    
    g_initialized = 1;
    return 0;
}
int fsm_add_trans(uint16_t start_state, uint16_t end_state, uint16_t event_id)
{
    if (start_state > (g_num_states - 1) ||
        end_state > (g_num_states -1 ) ||
        event_id > (g_num_events -1 ) )
    {
        /* invalid params */
        dprint("invalid params.");
        return -1;
    }
    
    g_transition_arr[start_state][event_id] = end_state;
    return 0;
}
int fsm_rm_trans(uint16_t start_state, uint16_t event_id)
{
    if (start_state > (g_num_states - 1) ||
        event_id > (g_num_events -1 ) )
    {
        /* invalid params */
        dprint("invalid params.");
        return -1;
    }
    
    g_transition_arr[start_state][event_id] = start_state;
    return 0;
}
int fsm_trigger_event(uint16_t event_id)
{
    if (event_id > ( g_num_events - 1 ) )
    {
        /* Invalid params */
        dprint("invalid params.");
        return -1;
    }
    
    g_event_mailbox = event_id;
    g_event_triggered = 1;
    return 0;
}
int fsm_close( void )
{
    if (g_initialized)
    {
        g_curr_state = 0;
        g_num_events = 0;
        g_num_states = 0;
        g_event_mailbox = 0;
        g_initial_state = 0;
        
        if (g_code_ptr_arr != NULL)
        {
            int i;
            for (i = 0; i < g_num_events; i++)
            {
                free(g_code_ptr_arr[i]);
            }
            free(g_code_ptr_arr);
            g_code_ptr_arr = NULL;
        }
        else
        {
            dprint("g_code_ptr_arr is already NULL.");
        }
        
        if (g_transition_arr != NULL)
        {
            int i;
            for (i = 0; i < g_num_states; i++)
            {
                free(g_transition_arr[i]);
            }
            free(g_transition_arr);
            g_transition_arr = NULL;
        }
        else
        {
            dprint("g_transition_arr is already NULL.");
        }
    }
    
    g_initialized = 0;
    return 0;
}
int fsm_reset_state ( void )
{
    g_event_mailbox = 0;
    g_event_triggered = 0;
    g_curr_state = g_initial_state;
    return 0;
}
int fsm_set_code_ptr (uint16_t state, enum code_point my_point, fsm_function function_pointer)
{
    if (g_initialized)
    {
        g_code_ptr_arr[state][my_point] = function_pointer;
    }
    else
    {
        dprint("initialize FSM before setting function pointer.");
    }
    return 0;
}
void fsm_run_fsm( void )
{
    if (g_initialized)
    {
        /* intentional infinite loop */
        while (1)
        {
            /* execute enter code for current state */
            fsm_function to_execute = g_code_ptr_arr[g_curr_state][ENTRY];
            if (to_execute == NULL)
            {
                dprint("no entry function assigned to active state.");
            }
            else
            {
                int result = to_execute();
                if (result != 0)
                {
                    dprint("entry function returned non-zero value.");
                }
            }         
            /* execute active code for current state (infinite loop until an event is triggered) */
            fsm_run_active_state();
            
            /* execute exit code for current state */
            to_execute = g_code_ptr_arr[g_curr_state][EXIT];
            if (to_execute == NULL)
            {
                dprint("no exit function assigned to active state.");
            }
            else
            {
                int result = to_execute();
                if (result != 0)
                {
                    dprint("exit function returned non-zero value.");
                }
            }
            
            /* update current state (active code only ends if a state changes */
            fsm_update_curr_state();
        } /* end while */
    }
    else
    {
        dprint("must call \"fsm_init()\" before trying to run the FSM.");
    }
    
}
/* ---------------------------------------- Internal FSM Functions ----------------------------------------*/
int fsm_run_active_state( void )
{
    uint8_t stay_in_state = 1;
    while (stay_in_state)
    {
        do {
            /* execute active code as specified by user */
            int result = 0;
            fsm_function to_execute = g_code_ptr_arr[g_curr_state][ACTIVE];
            if (to_execute == NULL)
            {
                dprint("no function assigned to active state.");
            }
            result = to_execute();
            if (result != 0)
            {
                dprint("user function returned non-zero value.+-");
            }
        }
        while (g_event_triggered == 0);
        
        /* event triggered, need to determine if we need to change state */
        if (g_event_mailbox > ( g_num_events - 1 ) )
        {
            /* invalid mailbox, clear it and continue executing */
            g_event_mailbox = 0;
            g_event_triggered = 0;
        } else
        {
            /* valid event triggered, see if that causes the state to change */
            if (g_transition_arr[g_curr_state][g_event_mailbox] != g_curr_state)
            { 
                /* prospective state is not equal to current state -> state change! */
                stay_in_state = 0;
            }
            
            /* no state transition on this event, continue executing */
        }
    }
    
    return 0;
}
int fsm_update_curr_state ( void )
{
    if (g_event_triggered)
    {
        if (g_event_mailbox < g_num_events )
        {
            /* update current state according to transition array and event in mailbox */
            g_curr_state = g_transition_arr[g_curr_state][g_event_mailbox];
        }
        else 
        {
            dprint("invalid event passed in.");
        }
        g_event_triggered = 0;
    } 
    
    /* reset mailbox */
    g_event_mailbox = 0;
    return 0;
}

