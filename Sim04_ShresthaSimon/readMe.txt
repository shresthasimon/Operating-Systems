Simon Shrestha CS446 Project 4

Note: The design of this project was discusses with Erik Duong

To run:
-------
1. make sure that all files are in the same folder
2. type make
3. ./Sim04 config_4.conf
4. you can also type
    make clean && make && ./Sim04 config_4.conf

main.cpp
---------------------------------------------
Line 49: scheduleProcess is run to decide on what algorithm to run depending what is read from configuration file

Scheduler.cpp
---------------------------------------------
Line 22: scheduleProcess function definition which decides on which algorithm to run
Line 45: priority function definition that sorts based on IO count
Line 50: shortestJobFirst definition that sorts based on task count
Line 55: sortByPriority definition that compares IO count of two processes
Line 60: sortByShortest definition that compares task count of two processes


