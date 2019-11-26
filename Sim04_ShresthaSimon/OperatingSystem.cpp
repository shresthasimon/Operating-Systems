/**
 * @author Simon Shrestha
 *
 * @file OperatingSystem.cpp
 *
 * @brief does all of the threading work
 *
 * @details completes thread work, mutex and semaphore, and handles most jobs
 *
 */
#include <iostream>
#include <string>
#include <deque>
#include <semaphore.h>
#include <pthread.h>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "Config.h"
#include "MetaData.h"
#include "Process.h"
#include "Log.h"
#include "OperatingSystem.h"

// parameterized constructor the initializes everything
OperatingSystem::OperatingSystem(Config *conf, string config) {
    //reading in configuration file
    conf->readConfigFile(config);
    // create queue for tasks
    deque<MetaData> systemOperations;
    //read in metadata file
    MetaData::readMetaFile(conf->getFilePath(), systemOperations);
    // create processes from metadata file
    Process::createProcesses(this->processes, systemOperations);
    //initalizes all mutexes
    pthread_mutex_init(&this->printerLock, NULL);
    pthread_mutex_init(&this->harddriveLock, NULL);
    pthread_mutex_init(&this->monitorLock, NULL);
    pthread_mutex_init(&this->mouseLock, NULL);
    pthread_mutex_init(&this->keyboardLock, NULL);
    // initialize all semaphores
    sem_init(&this->harddriveSemaphore, 0, conf->getHarddriveResources());
    sem_init(&this->printerSemaphore, 0, conf->getPrinterResources());
    sem_init(&this->monitorSemaphore, 0, 1);
    sem_init(&this->keyboardSempahore, 0, 1);
    sem_init(&this->monitorSemaphore, 0, 1);
    // create start time
    this->START_TIME = chrono::system_clock::now();
    // set counters to 0
    this->harddriveCount = 0;
    this->printerCount = 0;
    this->memoryBlocks = 0;

}
/**
 * @name timer
 *
 * @details timer thread for each process. Calculates duration for each process.
 *
 * @param process
 *        process to calculate time for
 *
 */
void* OperatingSystem::timer(void *emp) {
    // creating start and end points
    chrono::system_clock::time_point start;
    chrono::system_clock::time_point end;
    // Setting a time limit
    double timeLimit = 0.1;
    // time spent variable
    chrono::duration<double> timeSpent;
    //starting the timer
    start = chrono::system_clock::now();
    // while the time limit is not reached, calculate time
    while(timeSpent.count() < timeLimit){
        end = chrono::system_clock::now();
        timeSpent = end - start;
    }
    // delete thread
    pthread_exit(NULL);
    return NULL;
}
/**
 * @name processthread
 *
 * @description creates threads for timer
 *
 * @param timeLimit
 *        The max length a process is allowed to take
 *
 * @return
 */
double OperatingSystem::processThread(int timeLimit) {
//    int time = timeLimit;
    // timerThread variable to track time
    pthread_t timerThread;

    //the time limit that a process can reach
    auto timeEnd = chrono::system_clock::now() + chrono::milliseconds(timeLimit);

    //until the max thread is reached, create a new timer thread and add it to the main
    while(chrono::system_clock::now() < timeEnd){
        pthread_create(&timerThread, NULL, this->timer, NULL);
        pthread_join(timerThread, NULL);
    }
    // current time of the thread
    auto currentTime = chrono::system_clock::now();
    // calculating the amount of time a process has taken
    return chrono::duration<double>(currentTime - this->START_TIME).count();
}
/**
 * @name threadOperation
 *
 * @description runs each individual I/O process
 *
 * @param millisec
 *        max time limit for an i/o process
 *
 * @param metaData
 *        meta data object to access time variables in the class
 *
 * @param semaphore
 *        semaphore that is being used
 *
 * @param mutex
 *        which mutex to unlock and lock
 */
void OperatingSystem::threadOperation(int timeLimit, MetaData & metaData, sem_t &semaphore, pthread_mutex_t &mutex) {
    //signals semaphore
    sem_wait(&semaphore);
    //locks mutex
    pthread_mutex_lock(&mutex);
    //processes the thread
    auto endTime = processThread(timeLimit);
    //unlocks the mutex
    pthread_mutex_unlock(&mutex);
    //sets the time taken
    metaData.setTime(endTime);
    //signals semaphore
    sem_post(&semaphore);
}
/**
 * @name generateMemoryLocation
 *
 * @description set memory to random memory value in hex
 *
 * @param memory
 *        amount of bytes that is available
 * @return
 */
string OperatingSystem::generateMemoryLocation(int memory) {
    string input;
    stringstream ss;
    ss << hex << setw(8) << setfill('0')  << memory;
    return ss.str();
}

/**
 * @description: returns queue of processes
 * @return
 */
deque<Process> OperatingSystem::getProcesses() {
    return this->processes;
}

/**
 * @description: resets memory blocks used to 0
 */
void OperatingSystem::resetMemory() {
        this->memoryBlocks = 0;
}

/**
 * @description: processes the initailization and ending of system
 * @param print
 *        what to print
 * @param conf
 */
void OperatingSystem::processSystem(string print, Config * conf) {
    //get the current time
    auto currentTime = chrono::system_clock::now();
    // get total time
    auto timed = (chrono::duration<double>(currentTime-this->START_TIME).count());
    //print statement
    Log::output(*conf, to_string(timed) + " - " + print);
}
/**
 * @details: handles printing and thread operation of I/O processes
 * @param metaData
 *        metaData class
 * @param conf
 *        configuration class
 * @param semaphore
 *        which semaphore to use
 * @param lock
 *        which mutex to use
 * @param count
 *        count for how many resources of particular I/O device is used
 * @param processNumber
 *        which process this is
 * @param cycleTime
 *        cycle time of the resource
 * @param print
 *        what to print
 */
void OperatingSystem::processIOOperation(MetaData metaData, Config *conf, sem_t &semaphore,
                                         pthread_mutex_t &lock, int &count , int processNumber,
                                         int cycleTime, string print) {
    // set timelimit
    int timeLimit = metaData.getCycles() + cycleTime;
    //get current time
    auto currentTime = chrono::system_clock::now();
    //set the time
    metaData.setStartTime(chrono::duration<double>(currentTime-this->START_TIME).count());
    // what to do if the resource is printer / harddrive or not
    if(count >= 0){
        //output start
        Log::output(*conf, to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNumber) + ": start " + print);
        //process thread
        this->threadOperation(timeLimit, metaData, semaphore, lock);
        //output end
        Log::output(*conf, to_string(metaData.getTime()) + " - " + "Process " + to_string(processNumber) + ": end " + print);
    }else{
        Log::output(*conf, to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNumber) + ": start " + print);
        this->threadOperation(timeLimit, metaData, semaphore, lock);
        Log::output(*conf, to_string(metaData.getTime()) + " - " + "Process " + to_string(processNumber) + ": end " + print);
    }
}
/**
 * @details: handles printing and thread operations for everything besides I/O
 * @param print
 * @param conf
 * @param metaData
 * @param processNumber
 * @param cycleTime
 */
void OperatingSystem::processAction(string print, Config *conf, MetaData metaData, int processNumber, int cycleTime) {
    //set time limit
    auto timeLimit = metaData.getCycles() + cycleTime;
    //get current time
    auto currentTime = chrono::system_clock::now();
    // set start time
    metaData.setStartTime(chrono::duration<double>(currentTime-this->START_TIME).count());
    //output start
    Log::output(*conf, to_string(metaData.getStartTime()) + " - " + "Process " + to_string(processNumber) + ": start " + print);
    //set processing time
    metaData.setTime(this->processThread(timeLimit));
    // output end
    Log::output(*conf, to_string(metaData.getTime()) + " - " + "Process " + to_string(processNumber) + ": end " + print);
}

/**
 * @details: handles all processes with threading and output
 * @param p
 * @param conf
 */
void OperatingSystem::process(Process &p, Config *conf) {
    //get the tasks
    auto processOperations = p.getOperations();
    // for I/O that don't have resource count
    int noResources = -1;
    //starting processs
    this->processSystem("OS: Preparing process " + to_string(p.getProcessCount()), conf);
    this->processSystem("OS: Starting process " + to_string(p.getProcessCount()), conf);
    //set the ready state
    p.setProcessState(Process::ProcessState::READY);
    // go through queue
    for(auto metaData : processOperations){
        // if code is p
        if(metaData.getCode() == 'P' && metaData.getDescriptor() == "run"){
            // process
            this->processAction("processing action", conf, metaData, p.getProcessCount(), conf->getProcessorCycleTime());
        // if code is I
        }else if(metaData.getCode() == 'I'){
            if(metaData.getDescriptor() == "hard drive") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(metaData, conf, this->harddriveSemaphore, this->harddriveLock, this->harddriveCount, p.getProcessCount(), conf->getHarddriveCycleTime(), "hard drive input");
                p.setProcessState(Process::ProcessState::READY);
            }else if(metaData.getDescriptor() == "keyboard") {
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(metaData, conf, this->keyboardSempahore, this->keyboardLock, noResources, p.getProcessCount(), conf->getKeyboardCycleTime(), "keyboard input");
                p.setProcessState(Process::ProcessState::READY);
            }else if(metaData.getDescriptor() == "mouse"){
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(metaData, conf, this->mouseSemaphore, this->mouseLock, noResources, p.getProcessCount(), conf->getMouseCycleTime(), "mouse input");
                p.setProcessState(Process::ProcessState::RUNNING);
            }else{
                cerr << "Wrong descriptor for I" << endl;
                exit(0);
            }
            //if code is O
        }else if(metaData.getCode() == 'O'){
            if(metaData.getDescriptor() == "hard drive"){
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(metaData, conf, this->harddriveSemaphore, this->harddriveLock, this->harddriveCount, p.getProcessCount(), conf->getHarddriveCycleTime(), "hard drive output");
                p.setProcessState(Process::ProcessState::READY);
            }else if(metaData.getDescriptor() == "monitor"){
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(metaData, conf, this->monitorSemaphore, this->monitorLock, noResources, p.getProcessCount(), conf->getMoniterDisplayTime(), "monitor input");
                p.setProcessState(Process::ProcessState::READY);
            }else if(metaData.getDescriptor() == "printer"){
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processIOOperation(metaData, conf, this->printerSemaphore, this->printerLock, this->printerCount, p.getProcessCount(), conf->getPrinterCycleTime(), "printer output");
                p.setProcessState(Process::ProcessState::READY);
            }else{
                cerr << "Wrong descriptor for O" << endl;
                exit(0);
            }
            //if code is M
        }else if(metaData.getCode() == 'M'){
            if(metaData.getDescriptor() == "block"){
                p.setProcessState(Process::ProcessState::RUNNING);
                this->processAction("memory blocking", conf, metaData, p.getProcessCount(), conf->getMemoryCycleTime());
                p.setProcessState(Process::ProcessState::READY);
            }else if(metaData.getDescriptor() == "allocate"){
                // for allocate, do the same thing as processIOOperation function but add memoryblocks
                p.setProcessState(Process::ProcessState::RUNNING);
                auto timeLimit = metaData.getCycles() + conf->getMemoryCycleTime();
                auto currentTime = chrono::system_clock::now();
                this->memoryBlocks = (this->memoryBlocks > ((float) conf->getMemory()/(float) conf->getMemBlockSize())) ? 0 : this->memoryBlocks;
                auto memory = this->memoryBlocks * conf->getMemBlockSize();

                metaData.setStartTime(chrono::duration<double>(currentTime-START_TIME).count());
                Log::output(*conf, to_string(metaData.getStartTime()) + " - " + "Process " + to_string(p.getProcessCount()) + ": " + "allocating memory");
                metaData.setTime(this->processThread(timeLimit));
                Log::output(*conf, to_string(metaData.getTime()) + " - " + "Process " + to_string(p.getProcessCount()) + ": " + "memory allocated at 0x" + this->generateMemoryLocation(memory));
                // if there is not enough memory.
                this->memoryBlocks++;
                int totalBlocks = conf->getMemory()/conf->getMemBlockSize();
                if(this->memoryBlocks > totalBlocks){
                    this->memoryBlocks = 0;
                }

                p.setProcessState(Process::ProcessState::READY);
            }else{
                cerr << "Wrong descriptor for M" << endl;
                exit(0);
            }
        }
    }
    this->processSystem("OS: removing process " + to_string(p.getProcessCount()), conf);
    p.setProcessState(Process::ProcessState::EXIT);
}
