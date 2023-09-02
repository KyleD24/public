#include "engine_controller.h"
#include "ce_scheduler.h"

void airflow_callback(void)
{
    /* release a job in the airflow task to your scheduler here */
		printf("entered airflow_callback!\n");
		sch_schedule_sp_task((task_function_t*) ((int)engine_airflow_monitoring_task - 1));
}

void vibration_callback(void)
{
    /* release a job in the vibration task to your scheduler here */
		printf("entered vibration_callback!\n");
    sch_schedule_ap_task((task_function_t*) ((int)engine_vibration_analysis_task - 1));
}

int main()
{
    // ----------------------------------------------------------------- structures for ces_add_tasks()
    task_function_t* task_list[9] = {0};
    uint32_t min_int_arr_times[9] = {0, 0, 0, 400, 0, 0, 0, 0, 0};
    enum ces_periodicty periodicities[9] = {PERIODIC, PERIODIC, PERIODIC, SPORADIC, APERIODIC, 
                                            SLACK, SLACK, SLACK, SLACK};
    uint32_t rel_deadlines[9] = {600, 300, 150, 200, 500, 0, 0, 0, 0};
    uint32_t wcets[9] = {20, 20, 10, 50, 30, 30, 50, 60, 70};

    // these numbers were calculated by hand
    struct ces_task** list_of_tasks = NULL;
    uint32_t num_frame_items[6] = {3, 3, 3, 3, 3, 3};
    struct ces_task* T1; 
    struct ces_task* T2; 
    struct ces_task* T3; 
    //struct ces_task* T4; 
    //struct ces_task* T5; 
    struct ces_task* s30;
    struct ces_task* s50;
    struct ces_task* s60;
    struct ces_task* s70;
    struct ces_task* f0[3];
    struct ces_task* f1[3];
    struct ces_task* f2[3];
    struct ces_task* f3[3];
    struct ces_task* f4[3];
    struct ces_task* f5[3];
    struct ces_task** tasks_to_run[6] = {0};

    initialize_leds();
    
    engine_define_task_release_callbacks(airflow_callback, vibration_callback);

    /* setup (frames, tasks, jobs, initialization, etc.) and then run your scheduler here */
    // num_tasks = 5 tasks + 4 slacks
    ces_init(9, 6);

    task_list[0] = (task_function_t*) ((int)engine_low_pressure_compression_shaft_task - 1);     // [0] = T1
    task_list[1] = (task_function_t*) ((int)engine_high_pressure_compression_shaft_task - 1);    // [1] = T2
    task_list[2] = (task_function_t*) ((int)engine_fuel_injection_task - 1);                     // [2] = T3
    task_list[3] = (task_function_t*) ((int)engine_airflow_monitoring_task - 1);                 // [3] = T4
    task_list[4] = (task_function_t*) ((int)engine_vibration_analysis_task - 1);                 // [4] = T5
    task_list[5] = NULL;
    task_list[6] = NULL;
    task_list[7] = NULL;
    task_list[8] = NULL;
    list_of_tasks = ces_add_tasks(task_list, min_int_arr_times, periodicities, rel_deadlines, wcets);

    // ----------------------------------------------------------------- structures for ces_build_frames()
    // each task is referenced in the list_of_tasks, use it to specify which tasks to run
    // here I individually name each task for readability
    T1  = list_of_tasks[0];
    T2  = list_of_tasks[1];
    T3  = list_of_tasks[2];
    //T4  = list_of_tasks[3];
    //T5  = list_of_tasks[4];
    s30 = list_of_tasks[5];
    s50 = list_of_tasks[6];
    s60 = list_of_tasks[7];
    s70 = list_of_tasks[8];
		
    // construct frames using list of tasks
    f0[0] = T3;
    f0[1] = T1;
    f0[2] = s70;

    f1[0] = T2;
    f1[1] = T3;
    f1[2] = s70;

    f2[0] = T3;
    f2[1] = s60;
    f2[2] = s30;

    f3[0] = T3;
    f3[1] = s60;
    f3[2] = s30;

    f4[0] = T2;
    f4[1] = T3;
    f4[2] = s70;

    f5[0] = T3;
    f5[1] = s60;
    f5[2] = s30;

    tasks_to_run[0] = f0;
    tasks_to_run[1] = f1;
    tasks_to_run[2] = f2;
    tasks_to_run[3] = f3;
    tasks_to_run[4] = f4;
    tasks_to_run[5] = f5;
    
    ces_build_frames(num_frame_items, tasks_to_run);

    // ----------------------------------------------------------------- ces_start_scheduler()
    ces_start_scheduler(100000);
}

