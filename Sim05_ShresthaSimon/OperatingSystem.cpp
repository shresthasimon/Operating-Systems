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
#include "OperatingSystem.h"
using namespace std;

//global variables
fstream file;
bool write = false;
bool print = false;
bool rrCall = false;
int loadCount = 0;
typedef chrono::high_resolution_clock Time;
chrono::high_resolution_clock::time_point timeInit;
//create the mutexes
pthread_mutex_t mutexMEMORY;
pthread_mutex_t mutexPROCESSID;
pthread_mutex_t mutexHARDDRIVE;
pthread_mutex_t mutexPRINTER;
pthread_mutex_t mutexKEYBOARD;
pthread_mutex_t mutexMONITOR;
pthread_mutex_t mutexMOUSE;
pthread_mutex_t mutexLOADER;
pthread_mutex_t mutexRR;
//create the semaphores
sem_t semHARDDRIVE;
sem_t semPRINTER;
sem_t semKEYBOARD;
sem_t semMONITOR;
sem_t semMOUSE;
sem_t semLOADER;
sem_t semRR;
//constructor
OperatingSystem::OperatingSystem(){

}
//parameterized constructor
OperatingSystem::OperatingSystem(Config *config, MetaData *metadata, Resource *resource){
	configPtr = config;
	metaDataPtr = metadata;
	resourceManager = resource;
}
//setter and getter functions
void OperatingSystem::setTimerStatus(bool t){
	endTimer = t;
}

void OperatingSystem::setProcessorState(string processS){
	processorState = processS;
}

bool OperatingSystem::getTimerStatus() const {
    return endTimer;
}
/**
 * @name threadUsage
 *
 * @details determines how many threads to create and how to use them
 *
 * @param metaVector
 *      vector containing metadata
 * @param process
 *      operating system object
 */
void OperatingSystem::threadUsage(vector<string> metaVector, OperatingSystem &process){
	void *threadPtr;
	const int tidTotal = (metaVector.size() / 3);
	const int i = 0;
    //check if set to writing
	if (process.configPtr->getLog() == "Log to Both" || process.configPtr->getLog() == "Log to File"){
		file.open(process.configPtr->getLogFile(), ios::out | ios::binary);
		write = true;
	}
    //check if set to printing
	if (process.configPtr->getLog() == "Log to Both" || process.configPtr->getLog() == "Log to Monitor")
		print = true;

    //initialize the mutexes
	pthread_mutex_init(&mutexMEMORY, NULL);
	pthread_mutex_init(&mutexPROCESSID, NULL);
	pthread_mutex_init(&mutexHARDDRIVE, NULL);
	pthread_mutex_init(&mutexPRINTER, NULL);
	pthread_mutex_init(&mutexKEYBOARD, NULL);
	pthread_mutex_init(&mutexMONITOR, NULL);
	pthread_mutex_init(&mutexMOUSE, NULL);
	pthread_mutex_init(&mutexLOADER, NULL);
	pthread_mutex_init(&mutexRR, NULL);
    //initialize the semaphores
	sem_init(&semHARDDRIVE, 0, process.configPtr->getHarddriveResources());
	sem_init(&semPRINTER, 0, process.configPtr->getPrinterResources());
	sem_init(&semKEYBOARD, 0, process.configPtr->getKeyboardResources());
	sem_init(&semMONITOR, 0, process.configPtr->getMonitorResources());
	sem_init(&semMOUSE, 0, process.configPtr->getMouseResources());
	sem_init(&semLOADER, 0, 1);
	sem_init(&semRR, 0, 1);
    //thread ids that are equal to the number of operations
	pthread_t threadID[tidTotal];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	threadPtr = &process;
    // round robin scheduling
	if (process.configPtr->getCPUSchedulingAlgorithm() == "RR"){
        //creating time points
		chrono::milliseconds ms(100);
		chrono::milliseconds rrMS(process.configPtr->getCPUQuantumNumber());
		chrono::time_point<chrono::high_resolution_clock> endTime;
		chrono::time_point<chrono::high_resolution_clock> rrTime;
		endTime = chrono::high_resolution_clock::now() + ms;
		rrTime = chrono::high_resolution_clock::now() + rrMS;

        //store the start and finish codes
		process.s1 = *(metaVector.begin());
		process.s2 = *(metaVector.begin() + 1);
		process.s3 = *(metaVector.begin() + 2);
		process.s4 = *(metaVector.end() - 3);
		process.s5 = *(metaVector.end() - 2);
		process.s6 = *(metaVector.end() - 1);
		//remove S{begin}0
		metaVector.erase(metaVector.begin(), metaVector.begin() + 3);
		//remove S{finish}0
		metaVector.erase(metaVector.end() - 3, metaVector.end());
        // change the vector after removing the S
		process.metaDataPtr->setmetaVector(metaVector);

		if (loadCount == 0){
			process.scheduleVector.push_back(process.s1);
			process.scheduleVector.push_back(process.s2);
			process.scheduleVector.push_back(process.s3);

			for (unsigned int k = 0; k < metaVector.size(); k++){
				process.scheduleVector.push_back(metaVector[k]);
			}
		}
        //while not empty run the system
		while (!(process.scheduleVector.empty()) || process.endReached != true){
			if (chrono::high_resolution_clock::now() >= endTime && process.endReached == false){
				sem_wait(&semLOADER);
                // if 100ms is reached than call loader
				loaderRR(&process);
				endTime = chrono::high_resolution_clock::now() + ms;
				sem_post(&semLOADER);
			}
            // call on the threads
			while (chrono::high_resolution_clock::now() < rrTime && !(process.scheduleVector.empty())){
				//create a thread depending on operation
			    process.mCode = process.scheduleVector[i];
				process.mDescriptor = process.scheduleVector[i + 1];
				process.mCycle = process.scheduleVector[i + 2];
				pthread_create(&(threadID[i]), &attr, runner, threadPtr);
				pthread_join(threadID[i], NULL);
                //remove operation from vector
				reverse(process.scheduleVector.begin(), process.scheduleVector.end());
				process.scheduleVector.pop_back();
				process.scheduleVector.pop_back();
				process.scheduleVector.pop_back();
				reverse(process.scheduleVector.begin(), process.scheduleVector.end());
			}
            //interrupt due to quantum time
			if (chrono::high_resolution_clock::now() >= rrTime){
				sem_wait(&semRR);
				chrono::high_resolution_clock::time_point tc;
				double timeStamp = 0.000000;
				tc = Time::now();
				timeStamp = calculateTime(timeInit, tc);

				if (print == true)
					cout << timeStamp << " - " << "Process " << process.processID << ": interrupt processing action" << endl;
				if (write == true)
					file << timeStamp << " - " << "Process " << process.processID << ": interrupt processing action" << endl;
                //interrupt thread
				loaderRRInterrupt(&process);
				rrTime = chrono::high_resolution_clock::now() + rrMS;
				sem_post(&semRR);
			}
		}

		process.scheduleVector.push_back(process.s4);
		process.scheduleVector.push_back(process.s5);
		process.scheduleVector.push_back(process.s6);

		process.mCode = process.scheduleVector[i];
		process.mDescriptor = process.scheduleVector[i + 1];
		process.mCycle = process.scheduleVector[i + 2];
		pthread_create(&(threadID[i]), &attr, runner, threadPtr);
		pthread_join(threadID[i], NULL);

		reverse(process.scheduleVector.begin(), process.scheduleVector.end());
		process.scheduleVector.pop_back();
		process.scheduleVector.pop_back();
		process.scheduleVector.pop_back();
		reverse(process.scheduleVector.begin(), process.scheduleVector.end());

	}
	else if (process.configPtr->getCPUSchedulingAlgorithm() == "STR"){ //STR scheduling
		chrono::milliseconds ms(100);
		chrono::time_point<chrono::high_resolution_clock> endTime;
		endTime = chrono::high_resolution_clock::now() + ms;
        // store start and finish codes
		process.s1 = *(metaVector.begin());
		process.s2 = *(metaVector.begin() + 1);
		process.s3 = *(metaVector.begin() + 2);
		process.s4 = *(metaVector.end() - 3);
		process.s5 = *(metaVector.end() - 2);
		process.s6 = *(metaVector.end() - 1);
        //remove s{begin}0
		metaVector.erase(metaVector.begin(), metaVector.begin() + 3);
        //remove S{finish}0
		metaVector.erase(metaVector.end() - 3, metaVector.end());
        //
		process.metaDataPtr->setmetaVector(metaVector);

		if (loadCount == 0){
			process.scheduleVector.push_back(process.s1);
			process.scheduleVector.push_back(process.s2);
			process.scheduleVector.push_back(process.s3);

			for (unsigned int k = 0; k < metaVector.size(); k++){
				process.scheduleVector.push_back(metaVector[k]);
			}
		}
        //while not empty run the program
		while (!(process.scheduleVector.empty())){
			if (process.scheduleVector.at(i) == "A" && process.scheduleVector.at(i + 1) == "finish" && process.scheduleVector.at(i + 2) == "0" && process.endReached == false){
				sem_wait(&semLOADER);
				chrono::high_resolution_clock::time_point tc;
				double timeStamp = 0.000000;
				tc = Time::now();
				timeStamp = calculateTime(timeInit, tc);

				if (print == true)
					cout << timeStamp << " - " << "Process " << process.processID << ": interrupt processing action" << endl;
				if (write == true)
					file << timeStamp << " - " << "Process " << process.processID << ": interrupt processing action" << endl;

				loaderSTR(&process);
				sem_post(&semLOADER);
			}
            // call on the threads
			while (chrono::high_resolution_clock::now() < endTime && !(process.scheduleVector.empty())){
                // create a thread depending on the operation
				process.mCode = process.scheduleVector[i];
				process.mDescriptor = process.scheduleVector[i + 1];
				process.mCycle = process.scheduleVector[i + 2];
				pthread_create(&(threadID[i]), &attr, runner, threadPtr);
				pthread_join(threadID[i], NULL);
                //remove operation from vector
				reverse(process.scheduleVector.begin(), process.scheduleVector.end());
				process.scheduleVector.pop_back();
				process.scheduleVector.pop_back();
				process.scheduleVector.pop_back();
				reverse(process.scheduleVector.begin(), process.scheduleVector.end());
			}
			endTime = chrono::high_resolution_clock::now() + ms;
		}
	}else{
	    cerr << "Error, invalid cpu algorithm" << endl;
	    exit(0);
	}
    // if set to writing than close the file
	if (process.configPtr->getLog() == "Log to Both" || process.configPtr->getLog() == "Log to File"){
		file.close();
		write = false;
	}
    // destroy the sempahores
	sem_destroy(&semHARDDRIVE);
	sem_destroy(&semPRINTER);
	sem_destroy(&semKEYBOARD);
	sem_destroy(&semMONITOR);
	sem_destroy(&semMOUSE);
	sem_destroy(&semLOADER);
	sem_destroy(&semRR);
    //destroy the mutexes
	pthread_mutex_destroy(&mutexMEMORY);
	pthread_mutex_destroy(&mutexPROCESSID);
	pthread_mutex_destroy(&mutexHARDDRIVE);
	pthread_mutex_destroy(&mutexPRINTER);
	pthread_mutex_destroy(&mutexKEYBOARD);
	pthread_mutex_destroy(&mutexMONITOR);
	pthread_mutex_destroy(&mutexMOUSE);
	pthread_mutex_destroy(&mutexLOADER);
	pthread_mutex_destroy(&mutexRR);
}

/**
 * @name timerUsage
 *
 * @details creates a thread with defualt initialization
 *          used in runnerTimer to count time
 * @param ptr
 */
void OperatingSystem::timerUsage(OperatingSystem *ptr){
    void *threadPtr;
    pthread_t threadID;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    threadPtr = ptr;
    pthread_create(&threadID, &attr, ptr->runnerTimer, threadPtr);
    pthread_join(threadID, NULL);
}

/**
 * @name runner
 *
 * @details calls on programInfo() to run each thread and passed that to print or write
 * @param parameter
 *        has thread parameters
 * @return
 */
void* OperatingSystem::runner(void *parameter){
    //create pointer to configure member variables
	OperatingSystem *opPtr = static_cast<OperatingSystem*>(parameter);
	//create time object
	typedef chrono::high_resolution_clock Time;
	static auto t0 = Time::now();
	timeInit = t0;
	unsigned seed = time(0);
	srand(seed);
    //run program for each thread
	programInfo(opPtr, t0);

	pthread_exit(0);
}

/**
 * @name getDelay
 *
 * @details calculates delay time and delays the thread
 * @param ptr
 * @param operationTime
 * @param cycleTime
 */
void OperatingSystem::OperatingSystem::getDelay(OperatingSystem *ptr, int operationTime, int cycleTime){
    unsigned int delay;
    //calculate delay time
    delay = (operationTime * cycleTime);
    // delay the thread
    this_thread::sleep_for(chrono::milliseconds(delay));

    timer(ptr, operationTime, cycleTime);
}

/**
 * @name timer
 *
 * @details caclulates the time for how long the program has run
 * @param ptr
 *      operating system pointer
 * @param operationTime
 *      operation time
 * @param cycleTime
 *      cycle time
 */
void OperatingSystem::timer(OperatingSystem *ptr, const int operationTime, const int cycleTime){
    unsigned int delay;
    //caclulate delay
    delay = (operationTime * cycleTime);
    typedef chrono::high_resolution_clock Time;
    //time of program
    auto t0 = ptr->programStart;
    //time of program plus operation
    auto t1 = ptr->programStart + chrono::milliseconds(delay);
    chrono::milliseconds ms;
    bool timerExpired = false;
    //calculate difference
    auto difference = chrono::duration_cast<chrono::milliseconds>(t1 - t0);
    // go until time reaches time+delay
    while (t1 > t0){
        //update time
        t0 = Time::now();
        //check every 100ms if the time is expired
        if (difference.count() == ms.count() && t0 != t1){
            timerExpired = false;
        }else{
            timerExpired = true;
        }
    }
}

/**
 * @name runnerTimer
 *
 * @details calls on the timer and used for timer thread
 *
 * @param parameter
 * @return
 */
void* OperatingSystem::runnerTimer(void *parameter){
    //pointer to access member variables
    OperatingSystem *opPtr = static_cast<OperatingSystem*>(parameter);
    //waits to exit until flag is set
    if (opPtr->endTimer == true){
        opPtr->setTimerStatus(false);
        //exit timer thread
        pthread_exit(0);
    }
}

/**
 * @name calculateTime
 *
 * @details calculates the time taken for a thread
 * @param ti
 * @param tf
 * @return
 */
double OperatingSystem::calculateTime(const chrono::high_resolution_clock::time_point timeInitial, const chrono::high_resolution_clock::time_point timeFinal){
    typedef chrono::milliseconds milli;
    static milli milliSec;
    typedef chrono::duration<double> duration;
    duration durationSec;
    //calculate time difference
    durationSec = timeFinal - timeInitial;
    //convert to milliseconds
    milliSec = chrono::duration_cast<milli>(durationSec);

    return durationSec.count();
}

/**
 * @name memoryManagement
 *
 * @details manages the memory allocated during program run
 * @param ptr
 *      operatingsystem pointer
 * @return
 */
unsigned int OperatingSystem::memoryManagement(OperatingSystem *ptr)
{
    int allocatedMemory = 0;
    static int count = 0;
    // if nothing is allocated, allocate a new block
    if (count != 0 && allocatedMemory < (ptr->configPtr->getSystemMemorySize()) && ptr->resetMemory != true){

        allocatedMemory = (count * (ptr->configPtr->getBlockSize()));
        count++;
    }
    else if (ptr->resetMemory == true){ //reset memory
        allocatedMemory = 0;
        count = 0;
        count++;
        ptr->resetMemory = false;
    }
    else{
        allocatedMemory = 0;
        count = 0;
        count++;
    }

    return allocatedMemory;
}

/**
 * @name programInfo
 *
 * @detials completes operation call depending on operation code
 * @param ptr
 * @param timeInitial
 */
void OperatingSystem::programInfo(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial){
//	typedef chrono::high_resolution_clock Time;
	chrono::high_resolution_clock::time_point tCurr;
	double tStamp = 0.000000;
    //set the right num of zeros
	cout << fixed << showpoint << setprecision(8);
	file << fixed << showpoint << setprecision(8);
    //choose how to complete operation call
	if (ptr->mCode == "S"){
		startOP(ptr, timeInitial, tCurr, tStamp);
	}else if (ptr->mCode == "A"){
		applicationOP(ptr, timeInitial, tCurr, tStamp);
	}else if (ptr->mCode == "P"){
		processOP(ptr, timeInitial, tCurr, tStamp);
	}else if (ptr->mCode == "I"){
		inputOP(ptr, timeInitial, tCurr, tStamp);
	}else if (ptr->mCode == "O"){
		outputOP(ptr, timeInitial, tCurr, tStamp);
	}else if (ptr->mCode == "M"){
		memoryOP(ptr, timeInitial, tCurr, tStamp);
	}else{
	    cerr << "Error, invalid metadata code" << endl;
	    exit(0);
	}
}

/**
 * @name startOP
 *
 * @details handles operations that begin with S
 *
 * @param ptr
 * @param timeInitial
 * @param timeFinal
 * @param timeStamp
 */
void OperatingSystem::startOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp){
	cout << fixed << showpoint << setprecision(6);

	typedef chrono::high_resolution_clock Time;
    //checks if it needs to write, print, or do both
	if (ptr->mDescriptor == "begin" || ptr->mDescriptor == "start"){
		if (ptr->configPtr->getLog() == "Log to Both"){
			ptr->processorState = "READY"; //change state
            timeFinal = Time::now(); //get final time
			timeStamp = calculateTime(timeInitial, timeFinal); //calculate the difference
			if (print == true)
				cout << timeStamp << " - " << "Simulator program starting" << endl;
			if (write == true)
				file << timeStamp << " - " << "Simulator program starting" << endl;
		}
	}else{
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Simulator program ending" << endl;
		if (write == true)
			file << timeStamp << " - " << "Simulator program ending" << endl;
		ptr->processorState = "EXIT";
	}
}

/**
 * @name applicationOP
 *
 * @details handles operations that begin with A
 * @param ptr
 * @param timeInitial
 * @param timeFinal
 * @param timeStamp
 */
void OperatingSystem::applicationOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp){
	cout << fixed << showpoint << setprecision(6);

	typedef chrono::high_resolution_clock Time;

	if (ptr->mDescriptor == "begin" || ptr->mDescriptor == "start"){
		pthread_mutex_lock(&mutexPROCESSID); // lock the mutex

		if (ptr->mCycle == "0"){
			if (rrCall == true){
				ptr->processID = ptr->rrID;
			}

			ptr->processID++;
			rrCall = false;
		}

		if (ptr->mCycle != "0"){
			if (rrCall == false){
				ptr->rrID = ptr->processID;
			}

			ptr->processID = stoi(ptr->mCycle);
			rrCall = true;
		}

		pthread_mutex_unlock(&mutexPROCESSID); //unlock

		ptr->processorState = "READY";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal); //get current time
		if (print == true)
			cout << timeStamp << " - " << "OS: preparing process " << ptr->processID << "" << endl;
		if (write == true)
			file << timeStamp << " - " << "OS: preparing process " << ptr->processID << "" << endl;

		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal); //get current time
		if (print == true)
			cout << timeStamp << " - " << "OS: starting process " << ptr->processID << "" << endl;
		if (write == true)
			file << timeStamp << " - " << "OS: starting process " << ptr->processID << "" << endl;
	}
	else{
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal); //get current time
		if (print == true)
			cout << timeStamp << " - " << "OS: removing process " << ptr->processID << "" << endl;
		if (write == true)
			file << timeStamp << " - " << "OS: removing process " << ptr->processID << "" << endl;

		ptr->resetMemory = true; //reset memory
	}
}

/**
 * @name processOP
 *
 * @details handles operations that begin with P
 * @param ptr
 * @param timeInitial
 * @param timeFinal
 * @param timeStamp
 */
void OperatingSystem::processOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp){

	cout << fixed << showpoint << setprecision(6);

	typedef chrono::high_resolution_clock Time;

	if (ptr->mDescriptor == "run"){
		ptr->processorState = "RUNNING"; //set state
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal); //get current time
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": start processing action" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start processing action" << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getProcessorTime(), stoi(ptr->mCycle)); //calculate delay
		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);// get current time again
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": end processing action" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": end processing action" << endl;
	}
}
/**
 * @name inputOP
 *
 * @details handles operations that begin with I
 * @param ptr
 * @param timeInitial
 * @param timeFinal
 * @param timeStamp
 */
void OperatingSystem::inputOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp){

	cout << fixed << showpoint << setprecision(6);

	typedef chrono::high_resolution_clock Time;
	// if descriptor is hard drive
	if (ptr->mDescriptor == "hard drive"){
		sem_wait(&semHARDDRIVE);
		pthread_mutex_lock(&mutexHARDDRIVE);

		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal); //get current time
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " input on HDD " << ptr->resourceManager->harddrive.front() << endl;

		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " input on HDD " << ptr->resourceManager->harddrive.front() << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getHarddriveTime(), stoi(ptr->mCycle)); //delay time

		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal); //get current time 
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " input" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " input" << endl;
		//update the resources for hard drive
		ptr->resourceManager->resourceManagement(ptr->mDescriptor);
		
		pthread_mutex_unlock(&mutexHARDDRIVE);
		sem_post(&semHARDDRIVE);
	}else if (ptr->mDescriptor == "keyboard"){ // if descriptor is keyboard 

		sem_wait(&semKEYBOARD);
		pthread_mutex_lock(&mutexKEYBOARD);

		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " input" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " input" << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getKeyboardTime(), stoi(ptr->mCycle));
		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " input" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " input" << endl;
		
		ptr->resourceManager->resourceManagement(ptr->mDescriptor);
		pthread_mutex_unlock(&mutexKEYBOARD);
		sem_post(&semKEYBOARD);
	}else if (ptr->mDescriptor == "mouse"){ //if descriptor is mouse
		sem_wait(&semMOUSE);
		pthread_mutex_lock(&mutexMOUSE);

		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " input" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " input" << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getMouseTime(), stoi(ptr->mCycle));
		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " input" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " input" << endl;
		
		ptr->resourceManager->resourceManagement(ptr->mDescriptor);
		pthread_mutex_unlock(&mutexMOUSE);
		sem_post(&semMOUSE);
	}
}

/**
 * @name outputOP
 * 
 * @details handles operations that begin with O
 * @param ptr 
 * @param timeInitial 
 * @param timeFinal 
 * @param timeStamp 
 */
void OperatingSystem::outputOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp){

	cout << fixed << showpoint << setprecision(6);

	typedef chrono::high_resolution_clock Time;
    //if the descriptor is hard drive
	if (ptr->mDescriptor == "hard drive"){
		sem_wait(&semHARDDRIVE);
		pthread_mutex_lock(&mutexHARDDRIVE);

		ptr->processorState = "RUNNING";
		timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " output on HDD " << ptr->resourceManager->harddrive.front() << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " output on HDD " << ptr->resourceManager->harddrive.front() << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getHarddriveTime(), stoi(ptr->mCycle));
		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
		timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " output" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " output" << endl;
		
		ptr->resourceManager->resourceManagement(ptr->mDescriptor);
		pthread_mutex_unlock(&mutexHARDDRIVE);
		sem_post(&semHARDDRIVE);
	}else if (ptr->mDescriptor == "monitor"){ //if the descriptor is monitor
		sem_wait(&semMONITOR);
		pthread_mutex_lock(&mutexMONITOR);
		
		ptr->processorState = "RUNNING";
		timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " output" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " output" << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getMonitorTime(), stoi(ptr->mCycle));
		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
		timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " output" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " output" << endl;
		
		ptr->resourceManager->resourceManagement(ptr->mDescriptor);
		pthread_mutex_unlock(&mutexMONITOR);
		sem_post(&semMONITOR);
	}else if (ptr->mDescriptor == "printer"){ //if descriptor is printer
		sem_wait(&semPRINTER);
		pthread_mutex_lock(&mutexPRINTER);

		ptr->processorState = "RUNNING";
		timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " output on PRIN " << ptr->resourceManager->printer.front() << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start " << ptr->mDescriptor << " output on PRIN " << ptr->resourceManager->printer.front() << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getPrinterTime(), stoi(ptr->mCycle));

		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
		timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " output" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": end " << ptr->mDescriptor << " output" << endl;
		
		ptr->resourceManager->resourceManagement(ptr->mDescriptor);
		pthread_mutex_unlock(&mutexPRINTER);
		sem_post(&semPRINTER);
	}
}

/**
 * @name memoryOP
 *
 * @details handles operations that start with M
 * @param ptr
 * @param timeInitial
 * @param timeFinal
 * @param timeStamp
 */
void OperatingSystem::memoryOP(OperatingSystem *ptr, const chrono::high_resolution_clock::time_point timeInitial, chrono::high_resolution_clock::time_point timeFinal, double timeStamp){

	cout << fixed << showpoint << setprecision(6);

	typedef chrono::high_resolution_clock Time;

	unsigned int memoryValue;
    //if the descriptor is block
	if (ptr->mDescriptor == "block"){
		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": start memory " << ptr->mDescriptor << "ing" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": start memory " << ptr->mDescriptor << "ing" << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getMemoryTime(), stoi(ptr->mCycle));

		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": end memory " << ptr->mDescriptor << "ing" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": end memory " << ptr->mDescriptor << "ing" << endl;
	}
	else if (ptr->mDescriptor == "allocate"){ //if the descriptor is allocate
		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);
		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": allocating memory" << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": allocating memory" << endl;

		ptr->processorState = "WAITING";
		ptr->getDelay(ptr, ptr->configPtr->getMemoryTime(), stoi(ptr->mCycle));
		ptr->processorState = "READY";
		ptr->processorState = "RUNNING";
        timeFinal = Time::now();
		timeStamp = calculateTime(timeInitial, timeFinal);

		pthread_mutex_lock(&mutexMEMORY);

		memoryValue = memoryManagement(ptr); //obtain which memory block to go to

		pthread_mutex_unlock(&mutexMEMORY);

		if (print == true)
			cout << timeStamp << " - " << "Process " << ptr->processID << ": memory allocated at 0x" << setfill('0') << setw(8) << hex << memoryValue << endl;
		if (write == true)
			file << timeStamp << " - " << "Process " << ptr->processID << ": memory allocated at 0x" << setfill('0') << setw(8) << hex << memoryValue << endl;

		cout << dec; //print out in decimal
		file << dec; //print out in decimal
	}
}
/**
 * @name runnerSTR
 *
 * @details runner function for STR
 * @param parameter
 * @return
 */
void* OperatingSystem::runnerSTR(void *parameter){
    OperatingSystem *opPtr = static_cast<OperatingSystem*>(parameter);
    // call on the scheduler for STR
    scheduleSTR(opPtr->metaDataPtr->getmetaVector(), opPtr);
    pthread_exit(0);
}

/**
 * @name loaderSTR
 *
 * @detials loader function for STR
 * @param ptr
 */
void OperatingSystem::loaderSTR(OperatingSystem *ptr){
    void *threadPtr;
    pthread_t threadID;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    threadPtr = ptr;
    pthread_create(&threadID, &attr, ptr->runnerSTR, threadPtr);
    pthread_join(threadID, NULL);
}

/**
 * @name scheduleSIOF
 *
 * @details calculates the number of IO process for a thread
 * @param threadVector
 * @param ptr
 */
void OperatingSystem::scheduleSIOF(vector<string> threadVector, OperatingSystem *ptr){

    int startPos, tempStartPos, tempEndPos, endPos;
    vector<string>temp;
    bool startCount = false;
    bool stopCount = true;
    bool finalCount;
    int count = 0, previousCount = threadVector.size();
    unsigned int i = 0;

    temp.push_back(*(threadVector.begin()));
    temp.push_back(*(threadVector.begin() + 1));
    temp.push_back(*(threadVector.begin() + 2));
    //run until the end of the vector
    while (i < threadVector.size()){
        //if the beginning is found than start the count
        if (threadVector.at(i) == "A" && threadVector.at(i + 1) == "begin" && threadVector.at(i + 2) == "0"){
            startCount = true;
            stopCount = false;
            tempStartPos = i;
        }else if (startCount == true && stopCount == false && threadVector.at(i) == "A" && threadVector.at(i + 1) == "finish" && threadVector.at(i + 2) == "0"){
            //if the end in reached then stop and determine which one has the lowest IO count
            startCount = false;
            stopCount = true;
            tempEndPos = i + 2;

            count += std::count(threadVector.begin() + tempStartPos, threadVector.begin() + tempEndPos, "I");
            count += std::count(threadVector.begin() + tempStartPos, threadVector.begin() + tempEndPos, "O");

            if (count != 0 && count < previousCount && count != previousCount){
                startPos = tempStartPos;
                endPos = tempEndPos;
                previousCount = count;
                count = 0;
            }
        }
        i++;
        //store vector based on priority and erase from original
        if (i == threadVector.size() && stopCount == true){
            if ( (find(threadVector.begin(), threadVector.end(), "I") != threadVector.end()) || (find(threadVector.begin(), threadVector.end(), "O") != threadVector.end()) ){
                finalCount = false;
            }
            else{
                finalCount = true;
            }
            if (finalCount == false){
                for (int k = startPos; k <= endPos; k++){
                    temp.push_back(threadVector[k]);
                }

                threadVector.erase( (threadVector.begin() + startPos), (threadVector.begin() + endPos + 1) );

                i = 0;
                count = 0;
                previousCount = threadVector.size();
            }
        }
    }
    // add non IO tasks to end of vector
    if (finalCount == true){
        if (!(threadVector.empty())){
            for (unsigned int k = 0; k < threadVector.size(); k++){
                if (threadVector.at(k) == "A" && threadVector.at(k + 1) == "begin" && threadVector.at(k + 2) == "0"){
                    startCount = true;
                }
                if (startCount == true){
                    temp.push_back(threadVector[k]);
                }
            }
        }
    }

    ptr->scheduleVector = temp;
}

/**
 * @name scheduleSTR
 *
 * @details schedule function for STR
 * @param threadVector
 * @param ptr
 */
void OperatingSystem::scheduleSTR(vector<string> threadVector, OperatingSystem *ptr){

    pthread_mutex_lock(&mutexLOADER);
    //load new processes to metadata vector
    for (unsigned int j = 0; j < ptr->metaDataPtr->getmetaVector().size(); j++){
        ptr->scheduleVector.push_back(ptr->metaDataPtr->getmetaVector()[j]);
    }
    //call upon the IO scheduler
    scheduleSIOF(ptr->scheduleVector, ptr);
    //increment load count
    loadCount++;
    // if load count is 5, end the program
    if (loadCount == 4 && ptr->endReached == false){
        ptr->scheduleVector.push_back(ptr->s4);
        ptr->scheduleVector.push_back(ptr->s5);
        ptr->scheduleVector.push_back(ptr->s6);
        ptr->endReached = true;
    }
    pthread_mutex_unlock(&mutexLOADER);
}

/**
 * @name runnerRR
 *
 * @details runner function for RR
 * @param parameter
 * @return
 */
void* OperatingSystem::runnerRR(void *parameter){
    OperatingSystem *opPtr = static_cast<OperatingSystem*>(parameter);
    //use scheduler function for RR
    scheduleRR(opPtr->metaDataPtr->getmetaVector(), opPtr);
    pthread_exit(0);
}

/**
 * @name loaderRR
 *
 * @details loader function for RR
 * @param ptr
 */
void OperatingSystem::loaderRR(OperatingSystem *ptr){
    void *threadPtr;
    pthread_t threadID;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    threadPtr = ptr;
    pthread_create(&threadID, &attr, ptr->runnerRR, threadPtr);
    pthread_join(threadID, NULL);
}

/**
 * @name scheduleRR
 *
 * @details scheduler for RR
 * @param threadVector
 * @param ptr
 */
void OperatingSystem::scheduleRR(vector<string> threadVector, OperatingSystem *ptr){
	pthread_mutex_lock(&mutexLOADER);
	//load processes into vector
	for (unsigned int j = 0; j < ptr->metaDataPtr->getmetaVector().size(); j++){
		ptr->scheduleVector.push_back(ptr->metaDataPtr->getmetaVector()[j]);
	}
    //increase load count
	loadCount++;
	//if load count is 5 then end
	if (loadCount == 4 && ptr->endReached == false){
		ptr->endReached = true;
	}
	pthread_mutex_unlock(&mutexLOADER);
}

/**
 * @name scheduleRRInterrupt
 *
 * @detials scheduler for when RR is interrupted
 * @param threadVector
 * @param ptr
 */
void OperatingSystem::scheduleRRInterrupt(vector<string> threadVector, OperatingSystem *ptr){

	pthread_mutex_lock(&mutexRR);
    //if the vector is not empty or has not been read 5 times
	if ( !(ptr->scheduleVector.empty()) || ptr->endReached != true ){
		vector<string>temp;
		int count = 0;
		string currentprocessID;
		int quantity = 0;
		temp.push_back("A");
		temp.push_back("begin");
		temp.push_back(to_string(ptr->processID));
        // if operation was I than calculate new run time
		if (ptr->mCode == "I"){
			if (ptr->mDescriptor == "hard drive"){
				quantity = ((ptr->configPtr->getHarddriveTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getHarddriveTime()));

				quantity = (((ptr->configPtr->getHarddriveTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber())) / (ptr->configPtr->getHarddriveTime()));

				if (quantity > 0){
					temp.push_back("I");
					temp.push_back("hard drive");
					temp.push_back(to_string(quantity));
				}
			}else if (ptr->mDescriptor == "keyboard"){
				quantity = ( (ptr->configPtr->getKeyboardTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getKeyboardTime()) );

				quantity = ( ((ptr->configPtr->getKeyboardTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber()) ) / (ptr->configPtr->getKeyboardTime()) );

				if (quantity > 0){
					temp.push_back("I");
					temp.push_back("keyboard");
					temp.push_back(to_string(quantity));
				}
			}else if (ptr->mDescriptor == "mouse"){
				quantity = ((ptr->configPtr->getMouseTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getMouseTime()));

				quantity = (((ptr->configPtr->getMouseTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber())) / (ptr->configPtr->getMouseTime()));

				if (quantity > 0){
					temp.push_back("I");
					temp.push_back("mouse");
					temp.push_back(to_string(quantity));
				}
			}
		}else if (ptr->mCode == "O"){ //if operation was O, calculate new run time
			if (ptr->mDescriptor == "hard drive"){
				quantity = ((ptr->configPtr->getHarddriveTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getHarddriveTime()));

				quantity = (((ptr->configPtr->getHarddriveTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber())) / (ptr->configPtr->getHarddriveTime()));

				if (quantity > 0){
					temp.push_back("O");
					temp.push_back("hard drive");
					temp.push_back(to_string(quantity));
				}
			}else if (ptr->mDescriptor == "monitor"){
				quantity = ((ptr->configPtr->getMonitorTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getMonitorTime()));

				quantity = (((ptr->configPtr->getMonitorTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber())) / (ptr->configPtr->getMonitorTime()));

				if (quantity > 0){
					temp.push_back("O");
					temp.push_back("monitor");
					temp.push_back(to_string(quantity));
				}
			}else if (ptr->mDescriptor == "printer"){
				quantity = ((ptr->configPtr->getPrinterTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getPrinterTime()));

				quantity = (((ptr->configPtr->getPrinterTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber())) / (ptr->configPtr->getPrinterTime()));

				if (quantity > 0){
					temp.push_back("O");
					temp.push_back("printer");
					temp.push_back(to_string(quantity));
				}
			}
		}else if (ptr->mCode == "M"){ //if operation was M, calculate new run time
			if (ptr->mDescriptor == "allocate"){
				quantity = ((ptr->configPtr->getMemoryTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getMemoryTime()));

				quantity = (((ptr->configPtr->getMemoryTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber())) / (ptr->configPtr->getMemoryTime()));

				if (quantity > 0){
					temp.push_back("M");
					temp.push_back("allocate");
					temp.push_back(to_string(quantity));
				}
			}else if (ptr->mDescriptor == "block"){
				quantity = ((ptr->configPtr->getMemoryTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getMemoryTime()));

				quantity = (((ptr->configPtr->getMemoryTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber())) / (ptr->configPtr->getMemoryTime()));

				if (quantity > 0){
					temp.push_back("M");
					temp.push_back("block");
					temp.push_back(to_string(quantity));
				}
			}
		}else if (ptr->mCode == "P"){ //if operation is P, calculate new run time
			if (ptr->mDescriptor == "run"){
				quantity = ((ptr->configPtr->getProcessorTime() * stoi(ptr->mCycle)) / (ptr->configPtr->getProcessorTime()));

				quantity = (((ptr->configPtr->getProcessorTime() * stoi(ptr->mCycle)) - (ptr->configPtr->getCPUQuantumNumber())) / (ptr->configPtr->getProcessorTime()));

				if (quantity > 0){
					temp.push_back("P");
					temp.push_back("run");
					temp.push_back(to_string(quantity));
				}
			}
		}
        // for all remaining operations, place at the end of vector
		for (unsigned int j = 0; j < ptr->scheduleVector.size(); j++){
			if (ptr->scheduleVector[j] == "A" && ptr->scheduleVector[j + 1] == "begin"){
				count = j;
				break;
			}
			temp.push_back(ptr->scheduleVector[j]);	
		}
        // pop all operations that were just placed at the end of vector
		for (int j = 0; j < count; j++){
			reverse(ptr->scheduleVector.begin(), ptr->scheduleVector.end());
			ptr->scheduleVector.pop_back();
			reverse(ptr->scheduleVector.begin(), ptr->scheduleVector.end());
		}
        //push everything to schedule vector
		for (unsigned int j = 0; j < temp.size(); j++){
			ptr->scheduleVector.push_back(temp[j]);
		}
	}
	pthread_mutex_unlock(&mutexRR);
}
/**
 * @name loaderRRInterrupt
 *
 * @details loader function for RR interrupt
 * @param ptr
 */
void OperatingSystem::loaderRRInterrupt(OperatingSystem* ptr){
	void *threadPtr;
	pthread_t threadID;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	threadPtr = ptr;
	pthread_create(&threadID, &attr, ptr->runnerRRInterrupt, threadPtr);
	pthread_join(threadID, NULL);
}

/**
 * @name runnerRRInterrupt
 *
 * @details runner for RR interrupt
 * @param parameter
 * @return
 */
void* OperatingSystem::runnerRRInterrupt(void *parameter){
	OperatingSystem *opPtr = static_cast<OperatingSystem*>(parameter);
	//run scheduler function for RR interrupt
	scheduleRRInterrupt(opPtr->scheduleVector, opPtr);
	pthread_exit(0);
}