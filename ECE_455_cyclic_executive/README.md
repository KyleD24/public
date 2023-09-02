# README

This is my implementation of lab 3 for ECE 455 taken in Spring 2023 at the University of Waterloo. 

All program design and implementation was done by me.

Students were required to implement a cyclic executive scheduler framework. Then, they were to use this framework to implement a turbofan jet engine controller.

## Cyclic Executive Scheduler Framework

The user of this framework is able to do the following:

* define a set of tasks
* for each task the user can.
  * write code that runs as a job in that task.
  * specify the WCET of the task.
  * specify its relative deadline.
  * specifiy whether the task is periodic, sporadic, or aperiodic.
* specify a frame time f
* specify an ordered sequence of frames
* each frame would include an ordered sequence of periodic jobs and slack times in that frame.

The framework had the following design requirements:

* the framework shall use a timer interrupt at interval f to verify that all the jobs in the previous frame executed correctly.
* the framework shall recover jobs that did not complete by the frame boundary by continuing their execution in the next sufficient slack time.
* Sporadic and aperiodic jobs shall be scheduled into slack times.

## Turbofan Jet Engine Controller

The controller had to handle 5 unique safety-critical tasks. Some tasks were periodic, some were sporadic, and some were aperiodic.

The controller is implemented in "main.c" and uses the framework that is specified in "ce_scheduler.c/h".
