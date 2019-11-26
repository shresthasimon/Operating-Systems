/**
 * @author Simon Shrestha
 *
 * @file main.cpp
 *
 * @brief Driver program
 *
 * @details runs the operating system
 *
 * @note requires alll class files
 */
#include <iostream>
#include <string>

#include "Config.h"
#include "MetaData.h"
#include "Log.h"
#include "OperatingSystem.h"
#include "Process.h"
#include "Scheduler.h"

int main(int argc, char* argv[]){

    //checks if an argument was passed in
    if(argc != 2){
        cerr << "Error! You must use a .conf file as an input for this application." << endl;
        return 0;
    }

    //reads in first argument as file and checks if it is a .conf file
    string file(argv[1]);
    Config conf;

    if(file.substr(file.find_first_of(".") + 1) != "conf") {
        cerr << "Error! Only a .conf file is applicable as the argument." << endl;
        return 0;
    }
    // reads configuration and metadata file and initializes everything including
    // mutexes and semaphores.
    OperatingSystem os(&conf, file);

    //creates a queue of all the tasks created from reading metadata file
    auto processQueue = os.getProcesses();

    //starts the program
    os.processSystem("Simulator program starting", &conf);

    //orders the processes depending the scheduling algorithm chosen
    Scheduler::scheduleProcess(&conf, processQueue);

    // runs the process
    for(auto process: processQueue){
        // will also reset memory if there is not enough
        os.process(process, &conf);
    }

    //ends the program
    os.processSystem("Simulator program ending", &conf);

    //reset memory after program ends
    os.resetMemory();

    return 0;
}