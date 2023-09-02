#include "ce_scheduler.h"

/* ---------------------------------------- Globals ----------------------------------------*/
static uint32_t 	                g_num_tasks                    = 0;
static uint32_t          	        g_num_frames                   = 0;
static struct ces_task** 	        g_tasks_arr                    = NULL;
static struct ces_task***           g_frames_arr                   = NULL;  // this is the offline schedule, does't change
static struct ces_task**            g_scheduled_frame              = NULL;  // this is the scheduled frame, with aperiodic + sporadic tasks
static uint32_t*                    g_num_frame_items_arr          = NULL;  // size num_frames, each element is how many items are in each frame
static uint32_t          	        g_curr_frame_ctr               = 0;     // tracks which frame in the schedule we are on
// each buffer is used to contain task_function_t objects
// these objects are then used to lookup the task information in the initialized list of tasks
static struct circ_buf              g_recovery_buf;
static struct circ_buf              g_aperiodic_buf;
static struct circ_buf              g_sporadic_buf;
static uint32_t                     g_availale_contexts[7]          = {0};    // tracks which contexts are available
static struct ces_task*             g_running_task                  = NULL; // tracks which task is currently being run
static uint32_t											g_switch_to_context							= NO_CONTEXT_VAL; // mailbox for the scheduler to switch contexts

/* ---------------------------------------- External Functions ----------------------------------------*/
void ces_init(uint32_t num_tasks, uint32_t num_frames)
{
    g_num_tasks = num_tasks;
    g_num_frames = num_frames;

    buf_init(&g_recovery_buf, RECOVERY_BUF_SIZE);
    buf_init(&g_aperiodic_buf, APERIODIC_BUF_SIZE);
    buf_init(&g_sporadic_buf, SPORADIC_BUF_SIZE);

    g_tasks_arr = (struct ces_task**) malloc(sizeof(struct ces_task*) * num_tasks);
    g_frames_arr = (struct ces_task***) malloc(sizeof(struct ces_task**) * num_frames);
    g_num_frame_items_arr = (uint32_t*) malloc(sizeof(uint32_t) * num_frames);

    return;
}
struct ces_task** ces_add_tasks(task_function_t** tasks_to_run, uint32_t* min_int_arr_times, enum ces_periodicty* periodicities, uint32_t* rel_deadlines, uint32_t* wcets)
{
    int i = 0;
    for (i = 0; i < g_num_tasks; i++)
    {
        struct ces_task* new_task = (struct ces_task*) malloc(sizeof(struct ces_task));
        new_task->job = tasks_to_run[i];
        new_task->min_int_arr_time = min_int_arr_times[i];
        new_task->periodicity = periodicities[i];
        new_task->deadline = rel_deadlines[i] * 1000; // stores relative deadline (from release)
        new_task->wcet = wcets[i];
        new_task->context = NO_CONTEXT_VAL;
        g_tasks_arr[i] = new_task;
    }

    return g_tasks_arr;
} 
void ces_build_frames(uint32_t* num_frame_items, struct ces_task*** tasks_to_run)
{
    int i = 0;
    int j = 0;
		struct ces_task** new_frame = NULL;
    for (i = 0; i < g_num_frames; i++)
    {
        // build the frame items for each frame
        g_num_frame_items_arr[i] = num_frame_items[i]; // record number of items for later
        new_frame = (struct ces_task**) malloc(sizeof(struct ces_task*) * num_frame_items[i]);
        for (j = 0; j < num_frame_items[i]; j++)
        {
            // populate new frame with each frame item
            new_frame[j] = tasks_to_run[i][j];
        }

        // place the newly built frame into the offline schedule
        g_frames_arr[i] = new_frame;
    }

    return;
}
void ces_start_scheduler(uint32_t frame_time)
{
		int temp = 0;
    sch_init_contexts();

    // start interrupt that occurs on frames
    initialize_timer(frame_time, sch_callback);
    g_curr_frame_ctr = g_num_frames - 1; // trick scheduler to reset frame counter (set it to 0)
    //sch_callback(); // after timer starts, get the first frame going
    // busy wait in this context (context 0) so that interrupts can deal with the work
  printf("scheduler initialized\n");  
	while(1)
		{
			// wait for scheduler to call for a context switch
			if (g_switch_to_context != NO_CONTEXT_VAL)
			{
				sch_print_scheduled_frame_info(FILE_PATH);
				//printf("start_scheduler: switching to runner!\n");
				temp = run_in_context(RUNNER_CONTEXT, (task_function_t) sch_runner);
				//printf("start_scheduler: returned from running the runner with value %d\n", temp);
				g_switch_to_context = NO_CONTEXT_VAL;
			}
		}
}
void sch_schedule_ap_task(task_function_t* to_schedule)
{
    struct ces_task* listed_task = NULL;
    listed_task = ces_find_task(to_schedule);
    if (listed_task != NULL)
    {
				listed_task->deadline = timer_read() + listed_task->deadline; // set actual deadline
        buf_write(&g_aperiodic_buf, (void*)listed_task);
    }
}
void sch_schedule_sp_task(task_function_t* to_schedule)
{
    struct ces_task* listed_task = NULL;
    listed_task = ces_find_task(to_schedule);
    if (listed_task != NULL)
    {
      listed_task->deadline = timer_read() + listed_task->deadline;  // set actual deadline
			buf_write(&g_sporadic_buf, (void*)listed_task);
    }
}

/* ---------------------------------------- Internal Functions ----------------------------------------*/
void sch_init_contexts( void )
{
    int i = 2;

    initialize_context_switching_utility();

    g_availale_contexts[DEFAULT_CONTEXT] = 0; // context 0 is never available
    g_availale_contexts[RUNNER_CONTEXT] = 0; // context 1 is reserved for the runner
    
    for (i = 2; i < MAX_CONTEXT; i++)
    {
        create_new_context();
        g_availale_contexts[i] = 1; // mark this context as available
    }
}        
void sch_callback( void )
{
    int i = 0;
    uint32_t prev_context = get_currently_active_context();
		uint32_t curr_num_frame_items = 0;
		uint32_t time_us = 0;
		//printf("sch_callback() triggered!\n");

    // ----------------------------------------------------------------- check previous context
    if (prev_context != DEFAULT_CONTEXT)
    {
        // task was tardy since we weren't idle
        // assumes this interrupt won't trigger while in RUNNER_CONTEXT
        if (g_running_task->periodicity == PERIODIC)
        {
            // add tardy periodic task to recovery queue
            buf_write(&g_recovery_buf, (void*)g_running_task);
        }
        else if (g_running_task->periodicity == APERIODIC || g_running_task->periodicity == SPORADIC)
        {
            // throw away the task
            sch_release_context(g_running_task->context);
            g_scheduled_frame[i]->context = NO_CONTEXT_VAL;
        }
    }

    // ----------------------------------------------------------------- clean up previous frame
    if (g_scheduled_frame != NULL)
    {
        free(g_scheduled_frame);
        g_scheduled_frame = NULL;
    }

    // ----------------------------------------------------------------- create new frame
    // update frame counter (wrap if needed)
    //printf("g_curr_frame_ctr before: %u\n", g_curr_frame_ctr);
		g_curr_frame_ctr = (g_curr_frame_ctr == (g_num_frames - 1))? 0 : g_curr_frame_ctr + 1;
    //printf("g_curr_frame_ctr after: %u\n", g_curr_frame_ctr);
    curr_num_frame_items = g_num_frame_items_arr[g_curr_frame_ctr];
    // allocate new frame, will hold as many frame items as specified in the offline schedule
    g_scheduled_frame = (struct ces_task**) malloc(sizeof(struct ces_task*) * curr_num_frame_items);
		
		print_circular_buffer(&g_recovery_buf);
		print_circular_buffer(&g_aperiodic_buf);
		print_circular_buffer(&g_sporadic_buf);
    for (i = 0; i < curr_num_frame_items; i++)
    {
        // update schedule to be run 
				struct ces_task* curr_frame_item = NULL;
				uint32_t curr_time = 0;
				curr_time = timer_read();	
        g_scheduled_frame[i] = g_frames_arr[g_curr_frame_ctr][i];
				g_scheduled_frame[i]->deadline = curr_time + g_scheduled_frame[i]->deadline;

        // ----------------------------------------------------------------- schedule task into slack 
        // if task is slack, schedule a queued task in its place

        curr_frame_item = g_scheduled_frame[i];
        if (curr_frame_item->periodicity == SLACK)
        {
            // slack item, see if we can schedule something here
            // only looking at the first items in the queue
            struct ces_task* recovery_task = buf_peek(&g_recovery_buf);
            struct ces_task* aperiodic_task = buf_peek(&g_aperiodic_buf);
            struct ces_task* sporadic_task = buf_peek(&g_sporadic_buf);
            
            // priority: recovering a periodic task > sporadic > aperiodic
            if ( ( recovery_task != NULL ) &&
								 ( recovery_task->wcet <= curr_frame_item->wcet ) &&
                 ( recovery_task->deadline >= (curr_time + curr_frame_item->wcet) ) )
            {
                // schedule the recovery task
              printf("scheduling %x into slack!\n", (int)recovery_task->job);  
							g_scheduled_frame[i] = buf_read(&g_recovery_buf);
            } 
            else if ( ( sporadic_task != NULL ) &&
											( sporadic_task->wcet <= curr_frame_item->wcet ) &&
                      ( sporadic_task->deadline >= (curr_time + curr_frame_item->wcet) ) )
            {
                // schedule the sporadic task
								printf("scheduling %x into slack!\n", (int)sporadic_task->job);  
                g_scheduled_frame[i] = buf_read(&g_sporadic_buf);
            }
            else if ( ( aperiodic_task != NULL ) &&
											( aperiodic_task->wcet <= curr_frame_item->wcet ) && 
                      ( aperiodic_task->deadline >= (curr_time + curr_frame_item->wcet) ) )
            {
                // schedule the aperiodic task
								printf("scheduling %x into slack!\n", (int)aperiodic_task->job);  
                g_scheduled_frame[i] = buf_read(&g_aperiodic_buf);
            }
            // else can't schedule anything into the slack at this time
        }
    }
		// clean buffers
		time_us = timer_read();
		sch_clean_buf(&g_recovery_buf, time_us);
		sch_clean_buf(&g_aperiodic_buf, time_us);
		sch_clean_buf(&g_sporadic_buf, time_us);

    // g_scheduled_frame is now ready to be executed by the runner
		g_switch_to_context = RUNNER_CONTEXT;

    return;
}
void sch_runner( void )
{
    int i = 0;
	int res_temp = 0;
    // execute all the frame items at the current frame counter
    // frame counter gets updated by the scheduler
    for (i = 0; i < g_num_frame_items_arr[g_curr_frame_ctr]; i++)
    {
        struct ces_task* curr_frame = g_scheduled_frame[i];
        g_running_task = curr_frame;

        // execute frame item
        if (curr_frame->context == NO_CONTEXT_VAL)
        {
            // no context yet, assign it one
            curr_frame->context = sch_get_context();
            // run task if there is one, else proceed to next frame item
            if (curr_frame->job != NULL)
            {
              printf("sch_runner() running job %x in context %d\n", (int)curr_frame->job, curr_frame->context);  
							res_temp = run_in_context(curr_frame->context, (task_function_t)curr_frame->job);
                // if we make it here, task ran to completion, release the context
                //printf("sch_runner() returned from run_in_context() with %d\n", res_temp);
								sch_release_context(curr_frame->context);
            }
            else
            {
                // no task to run, release the context
                sch_release_context(curr_frame->context);
            }
        }
        else
        {
            // has existing context, switch to it to allow it to resume
            context_switch_to(curr_frame->context);
						//printf("sch_runner() returned from context_switch_to()\n");
        }

        curr_frame->context = NO_CONTEXT_VAL;
        g_running_task = NULL;
    }
    // execution done for this frame
		printf("sch_runner execution done\n");
    return;
}
struct ces_task* ces_find_task(task_function_t* query_task)
{
    int i = 0;
    struct ces_task* to_return = NULL;

    // search through task list
    for (i = 0; i < g_num_tasks; i++)
    {
        if (*g_tasks_arr[i]->job == *query_task)
        {
            // found a match, return the corresponding task
            to_return = g_tasks_arr[i];
            break;
        }
    }

    return to_return;
}   
uint32_t sch_get_context( void )
{
    int i = 2;
    // If no contexts remain, give the last context and boot out whatever has it
    // not ideal, but there should always be a context available so you shouldn't run into this
    uint32_t to_return = MAX_CONTEXT - 1;
    for (i = 2; i < MAX_CONTEXT; i++)
    {
        // search for an available context
        if (g_availale_contexts[i] == 1)
        {
            // found an available context
            g_availale_contexts[i] = 0;
            to_return = i;
            break;
        }
    }
		
		//printf("sch_get_context() returning %d\n", to_return);
    return to_return;
}
void sch_release_context(uint32_t context_num)
{
    if (context_num < MAX_CONTEXT)
    {
        g_availale_contexts[context_num] = 1;
    }
    
    return;
}
void sch_clean_buf(struct circ_buf* buf, uint32_t curr_time_us)
{
    struct ces_task* top_task = NULL;
    top_task = buf_peek(buf);
		if (top_task != NULL)
		{
			if (top_task->deadline < curr_time_us)
			{
					// late, remove from queue
					sch_release_context(top_task->context);
					buf_read(buf);
					top_task->context = NO_CONTEXT_VAL;
			}
			// else its fine
		}

    return;

}                                   
void sch_print_scheduled_frame_info(const char* file_path)
{
    //FILE* file = fopen(file_path, "w");
		uint32_t i = 0;

    //fprintf(file, "Scheduled Frame Information:\n");
		printf("-------------------------------------- Information for frame #%u:\n", g_curr_frame_ctr);
    for (i = 0; i < g_num_frame_items_arr[g_curr_frame_ctr]; i++)
    {
        struct ces_task* curr_frame = g_scheduled_frame[i];
        struct ces_task* curr_task = curr_frame;

        // fprintf(file, "Frame Item %d:\n", i + 1);
        // fprintf(file, "  Deadline: %u\n", curr_task != NULL ? curr_task->deadline : 0);
        // fprintf(file, "  WCET: %u\n", curr_task != NULL ? curr_task->wcet : curr_frame->wcet);
        // fprintf(file, "  Periodicity: ");
        printf("Frame Item %d:\n", i + 1);
				printf("  Job: %x\n", (int)curr_task->job);
				printf("  Deadline: %u\n", curr_task != NULL ? curr_task->deadline : 0);
        printf("  WCET: %u\n", curr_task != NULL ? curr_task->wcet : curr_frame->wcet);
        printf("  Periodicity: ");

        switch (curr_task != NULL ? curr_task->periodicity : APERIODIC)
        {
            // case PERIODIC:
            //     fprintf(file, "Periodic\n");
            //     break;
            // case SPORADIC:
            //     fprintf(file, "Sporadic\n");
            //     break;
            // case APERIODIC:
            //     fprintf(file, "Aperiodic\n");
            //     break;
            // default:
            //     fprintf(file, "Unknown\n");
            case PERIODIC:
                printf("Periodic\n");
                break;
            case SPORADIC:
                printf("Sporadic\n");
                break;
            case APERIODIC:
                printf("Aperiodic\n");
                break;
            case SLACK:
                printf("Slack\n");
								break;
            default:
                printf("Unknown\n");
        }
        //fprintf(file, "\n");
        printf("\n");
    }

    //fclose(file);
}
