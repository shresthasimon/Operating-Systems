/**
 * @author Simon Shrestha
 *
 * @file process.cpp
 *
 * @brief implementation file for process class
 *
 * @details assigns processes different roles 
 *
 */
#include <string>
#include <deque>

#include "Process.h"
#include "MetaData.h"
using namespace std;

Process::Process() {
    this->processState = ProcessState::START;
    this->numOfIO = 0;
    this->numOfTasks = 0;
}

/**
 * @details: function to creates processes and associates their operations to different lists depending on code
 * @param processes
 *        queue of processes that need there operations to be sorted
 * @param operations
 *        queue of operations within a process
 */
void Process::createProcesses(deque<Process>& processes, deque<MetaData> operations) {
    //set num of processes to 1
    int numOfProcesses = 1;

    for(auto current = operations.begin(); current != operations.end(); ++current){
        // if the code is A then set process num to 1
        if(current->getCode() == 'A' && current->getDescriptor() == "begin"){
            Process p;
            p.setProcessCount(numOfProcesses);

            auto temp = current;
            // as long as S is not seen, keep adding 1
            if(temp->getCode() != 'S' && temp->getDescriptor() != "finish"){
                temp = current+1;
            }

            while(temp->getDescriptor() != "finish"){
                // if I or O are seen, add one to IO count 
                if(temp->getCode() == 'O' || temp->getCode() == 'I'){
                    p.increaseIOCount();
                }
                //anything else add to task count 
                p.increaseTaskCount();
                // add to queue of tasks
                p.push(*temp);
                //move to next item in MetaData queue 
                advance(temp, 1);
            }

            processes.push_back(p);
            numOfProcesses++;
        }
    }
}
// adds process to end of queue 
void Process::push(MetaData metaData) {
    this->operations.push_back(metaData);
}

//increases IO count
void Process::increaseIOCount() {
    this->numOfIO++;
}
//increases regular task count
void Process::increaseTaskCount() {
    this->numOfTasks++;
}
// increases counter for number of processes
void Process::setProcessCount(int count) {
    this->processCount = count;
}
// get the number of processes 
int Process::getProcessCount() {
    return this->processCount;
}

// remove an operation from the front of queue
MetaData Process::popOperation() {
    auto metaData = this->operations.front();
    this->operations.pop_front();
    return metaData;
}

// get the operation 
deque<MetaData> Process::getOperations() {
    return this->operations;
}

// get the current state of a process
Process::ProcessState Process::getProcessState() {
    return this->processState;
}
// set a state to a process 
void Process::setProcessState(Process::ProcessState stateChange) {
    this->processState = stateChange;
}
//get the number of IO operations 
int Process::getIOCount() {
    return this->numOfIO;
}
//get number of tasks
int Process::getTaskCount() {
    return this->numOfTasks;
}

