/**
 * @author Simon Shrestha
 *
 * @file process.h
 *
 * @brief header for processer class
 *
 * @details categories all tasks read
 *
 */
#ifndef CS446PROJ4_PROCESS_H
#define CS446PROJ4_PROCESS_H

#include <iostream>
#include <string>
#include <deque>

#include "MetaData.h"
#include "Config.h"
using namespace std;

class Process {
    public:
        Process();
        // class for all process states
        enum class ProcessState { START, READY, RUNNING, WAIT, EXIT };
        // associated each task as either IO or not to help count for sceduling algorithms
        static void createProcesses(deque<Process>&, deque<MetaData>);
        void push(MetaData);

        void increaseIOCount();
        int getIOCount();

        void increaseTaskCount();
        int getTaskCount();

        void setProcessCount(int);
        int getProcessCount();

        ProcessState getProcessState();
        void setProcessState(ProcessState);

        deque<MetaData> getOperations();

        MetaData popOperation();

    private:
        ProcessState processState;
        int numOfIO;
        int numOfTasks;
        int processCount;
        //queue of tasks
        deque<MetaData> operations;
};


#endif //CS446PROJ4_PROCESS_H
