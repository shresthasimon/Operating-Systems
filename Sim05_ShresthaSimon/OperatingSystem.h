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

#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

#include "config.h"
#include "metadata.h"
#include "resourceManager.h"
#include <thread>				
#include <pthread.h>			
#include <semaphore.h>			

using namespace std;
class OperatingSystem
{

public:
    chrono::high_resolution_clock::time_point programStart;
    //constructor
    OperatingSystem();
    OperatingSystem(Config *config, MetaData *metadata, Resource *resource);
    //setter and getter functions
    void setProcessorState(string processS);
    void setTimerStatus(bool t);
    bool getTimerStatus() const;
    //thread functions
    static void threadUsage(vector<string> metaVect, OperatingSystem &process);
    static void timerUsage(OperatingSystem* ptr);
    static void *runner(void *parameter);
    //timer functions
    static void getDelay(OperatingSystem* ptr, const int operationTime, const int cycleTime);
    static void timer(OperatingSystem* ptr, const int operationTime, const int cycleTime);
    static void *runnerTimer(void *parameter);
    static double calculateTime(const chrono::high_resolution_clock::time_point timeInitial, const chrono::high_resolution_clock::time_point timeFinal);
    //memory functions
    static unsigned int memoryManagement(OperatingSystem *ptr);
    // process functions
    static void programInfo(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial);
    static void startOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp);
    static void applicationOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp);
    static void processOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp);
    static void inputOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp);
    static void outputOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp);
    static void memoryOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp);
    //STR functions
    static void *runnerSTR(void *parameter);
    static void loaderSTR(OperatingSystem* ptr);
    static void scheduleSIOF(vector<string> threadVector, OperatingSystem *ptr);
    static void scheduleSTR(vector<string> threadVector, OperatingSystem *ptr);
    // RR functions
    static void *runnerRR(void *parameter);
    static void loaderRR(OperatingSystem* ptr);
    static void scheduleRR(vector<string> threadVector, OperatingSystem *ptr);
    static void scheduleRRInterrupt(vector<string> threadVector, OperatingSystem *ptr);
    static void loaderRRInterrupt(OperatingSystem* ptr);
    static void *runnerRRInterrupt(void *parameter);

private:
	int processID = 0;
	int rrID = 0;
	bool resetMemory;
	bool endReached = false;
	bool endTimer = false;
	Config *configPtr;
	MetaData *metaDataPtr;
	Resource *resourceManager;
	string mCode;
	string mDescriptor;
	string mCycle;
	string processorState;
	string s1, s2, s3, s4, s5, s6;
	vector<string> scheduleVector;
};

#endif