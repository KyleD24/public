#include "utility.h"
#include "circ_buf.h"
#include "string.h"
#include "stdio.h"

/* ---------------------------------------- Usage ----------------------------------------*/
/*
    1. call ces_init(num_tasks, num_frames)
    2. call ces_add_tasks(tasks_to_run, min_int_arr_times, periodicities, rel_deadlines, wcets)
    3. call ces_build_frames(num_frame_items, tasks_to_run, durations)
    4. use sch_schedule_ap_task(to_schedule) and sch_schedule_sp_task(to_schedule)  
        as callbacks to release tasks during runtime that you would like the scheduler 
        to schedule at the next frame boundary
    5. call ces_start_scheduler(frame_time) which runs in an infinite loop
*/

/* ---------------------------------------- Typdefs & Defines ----------------------------------------*/
#define RECOVERY_BUF_SIZE           5
#define APERIODIC_BUF_SIZE          20
#define SPORADIC_BUF_SIZE           20
#define NO_CONTEXT_VAL              10 // indicates not initialized state (max is 8 and uint can't be negative)
#define DEFAULT_CONTEXT             0
#define RUNNER_CONTEXT              1
#define FILE_PATH                   "output.log"

typedef enum ces_periodicty{PERIODIC, SPORADIC, APERIODIC, SLACK} my_periodicity;
typedef struct ces_task
{
    task_function_t* job;
    uint32_t min_int_arr_time;
    enum ces_periodicty periodicity;
    uint32_t deadline;
    uint32_t wcet; 
    uint32_t context; // used by the framework when context switching
} my_task;

/* ---------------------------------------- External Functions ----------------------------------------*/
// mallocs the tasks and frames arrays to fit everything
void ces_init(uint32_t num_tasks, uint32_t num_frames);
// adds tasks to array of tasks (including slacks)
// uses arrays of values corresponding to each task
// use min_int_arr_time = 0 for non-sporadic tasks
// returns pointer to array of tasks
// note: all arrays must be of size num_tasks
struct ces_task** ces_add_tasks(task_function_t** tasks_to_run, uint32_t* min_int_arr_times, enum ces_periodicty* periodicities, uint32_t* rel_deadlines, uint32_t* wcets); 
// builds frames (that is, an array of frames where each frame is an array of frame items) and adds them to offline schedule
// num_frame_items = how many items (tasks and slacks) the user wishes to have in a frame
// tasks_to_run = array of array of tasks and slacks to run, slacks are indicated with NULL
// note: the size of tasks_to_run must equal num_frame_items
// returns a pointer to the array that was created
void ces_build_frames(uint32_t* num_frame_items, struct ces_task*** tasks_to_run);
// initializes timer interrupt, call at end of adding tasks/frames
// runs frames in the scheduler                                                           
void ces_start_scheduler(uint32_t frame_time);  
// adds an aperiodic task to the scheduling queue
void sch_schedule_ap_task(task_function_t* to_schedule);
// adds a sporadic task to the scheduling queue
void sch_schedule_sp_task(task_function_t* to_schedule);                             

/* ---------------------------------------- Internal Functions ----------------------------------------*/
// creates all necessary contexts for the scheduler, sets up callbacks
void sch_init_contexts( void );
// callback handler for timer interrupt. Handles scheduling decisions on frame boundaries
void sch_callback( void );
// the runner for the scheduler, does the actual context switching and is reset every time sch_callback() is called
// uses context 1, executes one frame of the schedule
void sch_runner( void );
// searches for a task in the initialized list of tasks that has the query_task as its task_function_t
// returns a pointer to that task, returns NULL if not found
struct ces_task* ces_find_task(task_function_t* query_task);
// returns the first available context that can be used
// removes the returned context from the available list
uint32_t sch_get_context( void );
// adds given context back to available list. New tasks that use that available context will reset it.
void sch_release_context(uint32_t context_num);
// scans the given buffer and removes any tasks that are late
void sch_clean_buf(struct circ_buf* buf, uint32_t curr_time_ms);
void sch_print_scheduled_frame_info( const char* file_path );
