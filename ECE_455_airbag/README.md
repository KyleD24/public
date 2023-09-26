# README

This is my implementation of lab 2 for ECE 455 taken in Spring 2023 at the University of Waterloo. 

All program design and implementation was done by me.

Students were required to perform worst-case-execution-time (WCET) analysis on the FSM framework that they designed in lab 1. Then, they were required to implement an airbag and anti-lock brake system (ABS) controller that met provided timing constraints using their FSM framework.

My implementation of the airbag and ABS controller satisfied the timing constraints that were provided in the lab handout.

The airbag and ABS controller acted depending on inputs such as the brake pedal being pressed, the ABS control signal being on, and the ADC value of the accelerometer. The airbag activation was simulated by turning on all the LEDs on the lab board.

When airbag conditions were met (accelerometer value above a certain threshold), all brake activity within the last 5 seconds was to be written to the log within 10ms of the collision occurring.

p2_main.c is the main file for this lab and contains the implementation of the airbag and ABS controller. The FSM framework that is referenced is not included in this directory since it is an updated version of the framework that was implemented in lab 1.
