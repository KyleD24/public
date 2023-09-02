#pragma once
#include "util.h"

/* instructions for use */
/*  0. (recommended) create enums for all your states and events.
    1. call fsm_init(num_states, num_events, initial_state)
    2. create entry/active/exit functions for each of your states
    3. assign the functions to each state using fsm_set_code_ptr(state, code_point, function_pointer)
        Note: code_point = {ENTRY, ACTIVE, EXIT}
        Note: state transitions are done with fsm_trigger_event(event_id) and must only be done in active code
    4. add state transitions using fsm_add_trans(start_state, end_state, event_id);
    5. call fsm_run_fsm() to start the infinite loop of your state machine
*/

/* limitations */
/* * limited to one instance of FSM at a time
   * enter/active/exit code must return an int and take no arguments
   * only active code may trigger events (state changes)
   * triggering multiple events at once will execute only the most recent event
   * warning: failing to set all function pointers for "active" code may result in getting stuck
*/
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

/* ---------------------------------------- Defines ----------------------------------------*/
typedef int (*fsm_function)( void ); /* Functions must take no arguments and return an int for status/results */
typedef enum code_point { ENTRY, ACTIVE, EXIT } my_point;
/* ---------------------------------------- External User Functions ----------------------------------------*/
int fsm_init(uint16_t num_states, uint16_t num_events, uint16_t initial_state);
int fsm_add_trans(uint16_t start_state, uint16_t end_state, uint16_t event_id);
int fsm_rm_trans(uint16_t start_state, uint16_t event_id);
int fsm_trigger_event(uint16_t event_id);
int fsm_close( void );
int fsm_reset_state ( void );
int fsm_set_code_ptr (uint16_t state, enum code_point my_point, fsm_function function_pointer);
void fsm_run_fsm( void );
/* ---------------------------------------- Internal FSM Functions ----------------------------------------*/
int fsm_run_active_state( void );
int fsm_update_curr_state ( void );
