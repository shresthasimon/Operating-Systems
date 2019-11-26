/**
 * @author Simon Shrestha
 *
 * @file OperatingSystem.h
 *
 * @brief does all of the threading work
 *
 * @details completes thread work, mutex and semaphore, and handles most jobs
 *
 */
#ifndef CS446PROJ4_OPERATINGSYSTEM_H
#define CS446PROJ4_OPERATINGSYSTEM_H

#include <iostream>
#include <string>
#include <deque>
#include <semaphore.h>
#include <pthread.h>
#include <chrono>

#include "Config.h"
#include "MetaData.h"
#include "Process.h"
#include "Log.h"

using namespace std;

class OperatingSystem {
    public:
        // initialize OP by reading in files, setting up threads, mutexes, and semaphores
        OperatingSystem(Config*, string);
        // handles all P, I, O, M processes
        void process(Process&, Config*);
        // handles initiating and ending simulator
        void processSystem(string, Config*);
        // resets memory
        void resetMemory();
        // queue with all processes
        deque<Process> getProcesses();

    private:
        // timer function
        static void* timer(void*);
        //thread handling
        double processThread(int);
        // operating thread using mutex and semaphore
        void threadOperation(int, MetaData&, sem_t&, pthread_mutex_t&);
        // processes and prints I/O operations
        void processIOOperation(MetaData, Config*, sem_t&, pthread_mutex_t&, int&, int, int, string);
        // processes and prints operations besides I/O
        void processAction(string, Config*, MetaData, int, int);
        //create memory location
        string generateMemoryLocation(int);
        // create starting time
        chrono::time_point<chrono::system_clock> START_TIME;
        //semaphore objects
        sem_t printerSemaphore;
        sem_t harddriveSemaphore;
        sem_t mouseSemaphore;
        sem_t monitorSemaphore;
        sem_t keyboardSempahore;
        //mutex objects
        pthread_mutex_t printerLock;
        pthread_mutex_t harddriveLock;
        pthread_mutex_t mouseLock;
        pthread_mutex_t monitorLock;
        pthread_mutex_t keyboardLock;
        //resource counters
        int harddriveCount;
        int printerCount;
        int memoryBlocks;
        // process queue
        deque<Process> processes;


};


#endif //CS446PROJ4_OPERATINGSYSTEM_H
