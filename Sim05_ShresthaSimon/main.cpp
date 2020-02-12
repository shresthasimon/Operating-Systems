/**
 * @author Simon Shrestha
 *
 * @file main.cpp
 *
 * @brief Driver program
 *
 * @details runs the operating system
 *
 * @note requires operatingsystem class
 */
#include <iostream>
#include "OperatingSystem.h"

using namespace std;

int main(int argc, char *argv[])
{
    //checks if an argument was passed in
    if(argc != 2){
        cerr << "Error! You must use a .conf file as an input for this application." << endl;
        return 0;
    }
    // only run if argument is passed in
	if (argc == 2)
	{
		typedef std::chrono::high_resolution_clock Time;
		//config object
		Config config;
		//config pointer
		Config *configPtr = &config;
		//metadata object
		MetaData metaData;
		//metadata pointer
		MetaData *metaDataPtr = &metaData;
		//resource object
		Resource resourceManager;
		//resource pointer
		Resource *resourcePtr = &resourceManager;
		//operating system object
		OperatingSystem OpSystem(configPtr, metaDataPtr, resourcePtr);
		//operating system pointer
		OperatingSystem *procPtr = &OpSystem;
		string configFileName,
			metadataFileName,
			metadataData;
        //set quantity for each resources
		config.setHarddriveResources("1");
		config.setKeyboardResources("1");
		config.setMonitorResources("1");
		config.setPrinterResources("1");
		config.setMouseResources("1");

        // start timer
		OpSystem.programStart = Time::now();
        //create thread for timer
		OpSystem.timerUsage(procPtr);
        //set process state to start
		OpSystem.setProcessorState("START");
        // parse through config file
        configFileName = argv[1];
        config.readConfigFile(configFileName);
        //parse through metadata file
		metadataFileName = config.getMetadataFile();
		metadataData = metaData.loadMetadataFile(metadataFileName);
		metaData.readMetadataFile(metadataData);
		metaData.getMetadataComponents(metaData.getmetaVector());

		OpSystem.setProcessorState("READY");
        // initialize resources depending on what is read from config file
		resourceManager.resourceStart(config);

		OpSystem.setProcessorState("RUNNING");
        // creates threads and runs them
		OpSystem.threadUsage(metaData.getmetaVector(), OpSystem);

		OpSystem.setProcessorState("WAITING");
		OpSystem.setProcessorState("READY");
		OpSystem.setProcessorState("RUNNING");
		OpSystem.setProcessorState("EXIT");
        //end timer
		OpSystem.setTimerStatus(true);
	}
	else if (argc < 2)
	{
	    cerr << "Error, wrong number of arguments" << endl;
	    exit(0);
	}
	else
	{
        cerr << "Error, wrong number of arguments" << endl;
        exit(0);
	}

	return 0;
}