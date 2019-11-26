/**
 * @author Simon Shrestha
 *
 * @file Scheduler.h
 *
 * @brief runs scheduling algorithms
 *
 * @details runs processes in order depending on scheduling algorithm
 *
 */
#ifndef CS446PROJ4_SCHEDULER_H
#define CS446PROJ4_SCHEDULER_H

#include <iostream>
#include <string>
#include <deque>

#include "Config.h"
#include "Process.h"
using namespace std;

class Scheduler {
    public:
        //decide on which scheduling algorithm
        static void scheduleProcess(Config*, deque<Process>&);

    private:
        //sort processes by PS
        static void priority(deque<Process>&);
        // sort processes by SJF
        static void shortestJobFirst(deque<Process>&);
        // compare processes depending on IO count
        static bool sortByPriority(Process, Process);
        // compare processes depending on task count
        static bool sortByShortest(Process, Process);

};


#endif //CS446PROJ4_SCHEDULER_H
