/**
 * @author Simon Shrestha
 *
 * @file Scheduler.cpp
 *
 * @brief runs scheduling algorithms
 *
 * @details runs processes in order depending on scheduling algorithm
 *
 */
#include <iostream>
#include <string>
#include <deque>
#include <algorithm>

#include "Config.h"
#include "Process.h"
#include "Scheduler.h"
using namespace std;

/**
 * @details: decides on which scheduling algorithm to use depending on input from configuration file
 * @param conf
 *        configuration class
 * @param processes
 *        queue of processes
 */
void Scheduler::scheduleProcess(Config* conf, deque<Process> &processes) {
    //if empty then return error
    if(conf->getSchedulingAlgorithm() == ""){
        cerr << "No scheduling algorithm selected" << endl;
        exit(0);
    }

    // if FIFO then do nothing because it is already in order
    if(conf->getSchedulingAlgorithm() == "FIFO"){

    }else if(conf->getSchedulingAlgorithm() == "PS"){
        // if PS then sort using priority
        priority(processes);
    }else if(conf->getSchedulingAlgorithm() == "SJF"){
        //if SJF then sort using IO count
        shortestJobFirst(processes);
    }else{
        cerr << "Not a valid scheduling algorithm" << endl;
        exit(0);
    }
}

// sorts queue using IO count
void Scheduler::priority(deque<Process> &processes) {
    sort(processes.begin(), processes.end(), sortByPriority);
}

//sorts queue using task count
void Scheduler::shortestJobFirst(deque<Process> &processes) {
    sort(processes.begin(), processes.end(), sortByShortest);
}

//using IO count to compare processes
bool Scheduler::sortByPriority(Process p1, Process p2) {
    return p1.getIOCount() > p2.getIOCount();
}

//using task count to sort processes
bool Scheduler::sortByShortest(Process p1, Process p2) {
    return p1.getTaskCount() < p2.getTaskCount();
}
